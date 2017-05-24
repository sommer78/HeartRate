enum { __FILE_NUM__ = 0 };

/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    profile_client.c
  * @brief   the general implementation for profile client.
  * @details
  * @author  Ethan_su
  * @date    2015-07-15
  * @version v0.1
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
  ******************************************************************************
  */

/** Add Includes here **/
#include <string.h>
#include <FreeRTOS.h>
#include "blueapi.h"
#include "blueapi_types.h"
#include "trace.h"
#include "gap.h"
#include "central.h"
#include "profile_client.h"

extern void *pvPortMalloc( size_t xSize,RAM_TYPE ramType ) ;

/** @cond private**/
/** @addtogroup GATT_Client_Common
  * @{
  */

/** @defgroup TRegClientCBsInfo TRegClientCBsInfo
  * @{ Callback information for each specific client module
  */
typedef struct
{
    TClientID   bClientID;       /**< specific client id. */
    uint16_t    *pSrvStartHandle;
    uint16_t    *pSrvEndHandle;
    CONST TGATTClientCBs *pCBs; /**< specific service call backs, registered from application. */
} TRegClientCBsInfo, * PRegClientCBsInfo;
/** End of TRegClientCBsInfo * @} */

/** @defgroup TClientCBsList TClientCBsList
  * @{ List that store the all registered specific client module information
  */
typedef struct _ClientCBsList
{
  struct _ClientCBsList *next;     // pointer to next client callbacks record
  TRegClientCBsInfo clientCBsInfo; // client callbacks
} TClientCBsList;
/** End of TClientCBsList * @} */

/** @defgroup Client_Private_Variables Client Private Variables
  * @{
  */
/** Add all private variables here **/
/**<  These two handle used to control continue discovery procedure. */
static uint16_t    disc_start_handle = 0;
static uint16_t    disc_end_handle = 0;
/* Indicate the current discovery state, may be sent to upper app. */
static TDiscoveryState disc_state = DISC_STATE_IDLE;
/* Identification of each specific client. */
static uint8_t     clientRegNum = 1;
static TClientID   disc_client_id = AppProcessGeneralClientMsgID;
static TClientID   read_client_id = AppProcessGeneralClientMsgID;
static TClientID   write_client_id = AppProcessGeneralClientMsgID;
/* Callbacks used for sending data to upper app. */
static TClientCBsList   *pClientCBsList = NULL;
static pfnSpecificClientAppCB_t pfnClientAppCB = NULL;
/** End of Client_Private_Variables
  * @}
  */

/** End of GATT_Client_Common
  * @}
  */
/** @endcond **/

/**
  * @brief  Used by application, register app callback.
  * @param  pFunc: fuction offered by application.
  * @retval None
  */
void clientAPI_RegisterAppCB( pfnSpecificClientAppCB_t pFunc )
{
    /* when specific callbacks not found, we will use this app callback to info upper app. */
    pfnClientAppCB = pFunc;
}

/**
  * @brief  Used by specific client, register callback.
  * @param[out]  outClientId: Client ID generated for registered specific client module.
  * @param  pSrvHandle: handle value of service on peer device.
  * @param  pClientCBs: callback functions implemented in specific client module.
  * @retval TRUE: register successful.
  * @retval FALSE: register failed.
  */
bool clientAPI_RegisterSpecClientCB( TClientID *outClientId, uint16_t *pSrvHandle, CONST TGATTClientCBs *pClientCBs )
{
    bool bRetStatus = TRUE;
    TClientCBsList  *pNewListItem;
    *outClientId = clientRegNum;

    pNewListItem = (TClientCBsList *)pvPortMalloc( sizeof( TClientCBsList ), RAM_TYPE_DATA_ON );
    if((pNewListItem == NULL) || (clientRegNum == 0xFF))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "clientAPI_RegisterSpecClientCB: fail!", 0);
        return FALSE;
    }

    // Set up new client CBs item
    pNewListItem->next = NULL;
    pNewListItem->clientCBsInfo.bClientID = *outClientId;
    pNewListItem->clientCBsInfo.pSrvStartHandle = pSrvHandle;
    pNewListItem->clientCBsInfo.pSrvEndHandle = pSrvHandle + 1;
    pNewListItem->clientCBsInfo.pCBs = pClientCBs;

    // Find spot in list
    if ( pClientCBsList == NULL )
    {
        // First item in list
        pClientCBsList = pNewListItem;
    }
    else
    {
        TClientCBsList *pLoop = pClientCBsList;

        // Look for end of list
        while ( pLoop->next != NULL )
        {
            pLoop = pLoop->next;
        }

        // Put new item at end of list
        pLoop->next = pNewListItem;
    }
    clientRegNum++;

    return bRetStatus;
}

/**
  * @brief  Used internal, find callback info for one specific client, using client ID.
  * @param  clientID: client ID of one specific client module.
  * @retval TRegClientCBsInfo: callback structure found.
  */
static TRegClientCBsInfo *client_FindCBsInfoByID( TClientID clientID )
{
    TClientCBsList *pLoop = pClientCBsList;

    while ( pLoop != NULL )
    {
        if ( pLoop->clientCBsInfo.bClientID == clientID )
        {
            return ( &(pLoop->clientCBsInfo) );
        }

        // Try next service
        pLoop = pLoop->next;
    }

    return ( (TRegClientCBsInfo *)NULL );
}

/**
  * @brief  Used internal, find callback info for one specific client, using handle.
  * @param  wHandle: service handle of one specific service.
  * @retval TRegClientCBsInfo: callback structure found.
  */
static TRegClientCBsInfo *client_FindCBsInfoByHandle( uint16_t wHandle )
{
    TClientCBsList *pLoop = pClientCBsList;

    while ( pLoop != NULL )
    {
        if ( (*(pLoop->clientCBsInfo.pSrvStartHandle) <= wHandle)
            && (*(pLoop->clientCBsInfo.pSrvEndHandle) >= wHandle) )
        {
            return ( &(pLoop->clientCBsInfo) );
        }

        // Try next service
        pLoop = pLoop->next;
    }

    return ( (TRegClientCBsInfo *)NULL );
}

/**
  * @brief  send GATTDiscoveryReq through BlueAPI.
  * @param  discoveryType: discovery type, service or characteristic, or descriptor...
  * @param  discoveryState: state used to distinguish different discovery mode.
  * @param  clientIDx: to flag which specific module send the discovery request.
  * @param  uuidValue: when discovery by uuid, must input the appropriate uuid value.
  * @param  wStartHandle: start handle of the discovery handle range.
  * @param  wEndHandle: end handle of the discovery handle range.
  * @retval ProfileResult_AppErr: discovery request failed.
  *         ProfileResult_Success: discovery request success.
  */
static TProfileResult client_SendGATTDiscoveryReq( TBlueAPI_GATTDiscoveryType discoveryType,
                                                   TDiscoveryState discoveryState,
                                                   TClientID  clientIDx,
                                                   TUuid    uuidValue,
                                                   uint16_t wStartHandle,
                                                   uint16_t wEndHandle )
{
    uint8_t *       pUUID128 = NULL;
    uint16_t        UUID16 = 0x0000;
    gaprole_States_t  gap_state;
    uint16_t        local_MDL_ID;
    TProfileResult  Result = ProfileResult_InvalidParameter;

    /* get UUID according to UUID type. */
    switch(uuidValue.UUIDType)
    {
        case UUID_TYPE_16:
            UUID16 = uuidValue.UUIDData.UUID16;
            break;
        case UUID_TYPE_128:
            pUUID128 = uuidValue.UUIDData.UUID128;
            break;
        default:
            break;
    }

    /* check if client ID is valid or not. */
    if(clientIDx >= clientRegNum)
    {
        DBG_BUFFER(MODULE_PROFILE,LEVEL_ERROR,"client_SendGATTDiscoveryReq: Client ID input error!",0);
        return(ProfileResult_AppErr);
    }

    centralGetGapParameter(GAPPRRA_ROLE_STATE, &gap_state);
    centralGetGapParameter(GAPPRRA_CONNHANDLE, &local_MDL_ID);

    if ( gap_state == GAP_CENTRAL_STATE_CONNECTED )
    {
        switch( discoveryType )
        {
            case blueAPI_GATTDiscoveryServices:
            case blueAPI_GATTDiscoveryServiceByUUID:
                /* If disc_start_handle = disc_end_handle = 0. */
                /* we should set them to defaults. */
                if ( wStartHandle == 0 )
                {
                  wStartHandle = 0x0001;
                }
                if ( wEndHandle == 0 )
                {
                  wEndHandle = 0xFFFF;
                }
                break;
			default:
                break;
        }

        if ( blueAPI_GATTDiscoveryReq(local_MDL_ID,
                                      discoveryType,
                                      wStartHandle, /* start */
                                      wEndHandle, /* end   */
                                      UUID16,     /* uuid16 */
                                      pUUID128   /* pUuid128 */
                                     )
           )
        {
            Result = ProfileResult_Success;
            /* switch discovery state. */
            disc_state = discoveryState;
            /* Set start handle and end handle to control continue discovery procedure. */
            disc_start_handle = wStartHandle;
            disc_end_handle = wEndHandle;
            /* remember the client id for this time request, will used in response handle later. */
            disc_client_id = clientIDx;
#ifdef PROFILE_DEBUG
            DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"client_SendGATTDiscoveryReq: local_MDL_ID=%d, type=%d, start=0x%4.4x, end=0x%4.4x, UUID16=0x%4.4x, pUUID128=%p",
                    5,local_MDL_ID,discoveryType,wStartHandle,wEndHandle,UUID16,pUUID128);
#endif
        }
        else
        {
            Result = ProfileResult_AppErr;
            DBG_BUFFER(MODULE_PROFILE,LEVEL_ERROR,"client_SendGATTDiscoveryReq: Fail! Please don't send multiple discover request one time.",0);
        }
    }
    else
    {
        DBG_BUFFER(MODULE_PROFILE,LEVEL_WARN,"client_SendGATTDiscoveryReq: Please make your device connected when send discovery request!",0);
    }

    return( Result );
}

/**
  * @brief  send discovery all primary services request.
  * @retval ProfileResult_Success: discovery request success.
  * @retval other: discovery request failed.
  */
TProfileResult clientAPI_AllPrimarySrvDiscovery( void )
{
    TProfileResult Result;
    TUuid    uuidValue;
    uint16_t wStartHandle = 0x0001;
    uint16_t wEndHandle = 0xFFFF;
    uuidValue.UUIDType = UUID_TYPE_UNUSED;
    Result = client_SendGATTDiscoveryReq( blueAPI_GATTDiscoveryServices,
                                          DISC_STATE_SRV,
                                          AppProcessGeneralClientMsgID,
                                          uuidValue,
                                          wStartHandle,
                                          wEndHandle );
    return( Result );
}

/**
  * @brief  send discovery services by 16 bit UUID request.
  * @param  clientID: client ID of specific client module.
  * @param  wUUID16: 16 bit UUID.
  * @retval ProfileResult_Success: discovery request success.
  * @retval other: discovery request failed.
  */
TProfileResult clientAPI_ByUuidSrvDiscovery( TClientID clientID, uint16_t wUUID16 )
{
    TUuid uuidValue;
    TProfileResult Result;
    
    uuidValue.UUIDType = UUID_TYPE_16;
    uuidValue.UUIDData.UUID16 = wUUID16;
    uint16_t wStartHandle = 0x0001;
    uint16_t wEndHandle = 0xFFFF;
    Result = client_SendGATTDiscoveryReq(blueAPI_GATTDiscoveryServiceByUUID,
                                         DISC_STATE_SRV,
                                         clientID,
                                         uuidValue,
                                         wStartHandle,
                                         wEndHandle );
    return( Result );
}

/**
  * @brief  send discovery services by 128 bit UUID request.
  * @param  clientID: client ID of specific client module.
  * @param  pUUID128: 128 bit UUID.
  * @retval ProfileResult_Success: discovery request success.
  * @retval other: discovery request failed.
  */
TProfileResult clientAPI_ByUuid128SrvDiscovery( TClientID clientID, uint8_t *pUUID128 )
{
    TUuid uuidValue;
    TProfileResult Result;

    uuidValue.UUIDType = UUID_TYPE_128;
    memcpy(uuidValue.UUIDData.UUID128, pUUID128, 16);
    uint16_t wStartHandle = 0x0001;
    uint16_t wEndHandle = 0xFFFF;
    Result = client_SendGATTDiscoveryReq(blueAPI_GATTDiscoveryServiceByUUID,
                                         DISC_STATE_SRV,
                                         clientID,
                                         uuidValue,
                                         wStartHandle,
                                         wEndHandle );
    return( Result );
}

/**
  * @brief  send discovery relationship services request.
  * @param  clientID: client ID of specific client module.
  * @param  relationReq: request data.
  * @retval ProfileResult_Success: discovery request success.
  * @retval other: discovery request failed.
  */
TProfileResult clientAPI_RelationshipDiscovery( TClientID clientID, TRelationDiscReq relationReq )
{
    TUuid    uuidValue;
    uuidValue.UUIDType = UUID_TYPE_UNUSED;
    return( client_SendGATTDiscoveryReq( blueAPI_GATTDiscoveryRelationship, 
                                         DISC_STATE_RELATION,
                                         clientID,
                                         uuidValue,
                                         relationReq.wStartHandle,
                                         relationReq.wEndHandle ) );
}

/**
  * @brief  send discovery characteristics request.
  * @param  clientID: client ID of specific client module.
  * @param  charReq: request data.
  * @retval ProfileResult_Success: discovery request success.
  * @retval other: discovery request failed.
  */
TProfileResult clientAPI_AllCharDiscovery( TClientID clientID, TCharDiscReq charReq )
{
    TUuid    uuidValue;
    uuidValue.UUIDType = UUID_TYPE_UNUSED;
    return( client_SendGATTDiscoveryReq( blueAPI_GATTDiscoveryCharacteristics,
                                         DISC_STATE_CHAR,
                                         clientID,
                                         uuidValue,
                                         charReq.wStartHandle,
                                         charReq.wEndHandle ) );
}

/**
  * @brief  send discovery characteristics descriptor request.
  * @param  clientID: client ID of specific client module.
  * @param  charDescriptorReq: request data.
  * @retval ProfileResult_Success: discovery request success.
  * @retval other: discovery request failed.
  */
TProfileResult clientAPI_AllCharDescriptorDiscovery( TClientID clientID, TCharDescriptorDiscReq charDescriptorReq )
{
    TUuid    uuidValue;
    uuidValue.UUIDType = UUID_TYPE_UNUSED;
    return( client_SendGATTDiscoveryReq( blueAPI_GATTDiscoveryCharacDescriptors,
                                         DISC_STATE_CHAR_DESCRIPTOR,
                                         clientID,
                                         uuidValue,
                                         charDescriptorReq.wStartHandle,
                                         charDescriptorReq.wEndHandle ) );
}

/**
  * @brief  read characteristic by handle request.
  * @param  clientID: client ID of specific client module.
  * @param  readHandle: request handle.
  * @retval ProfileResult_Success: discovery request success.
  * @retval other: discovery request failed.
  */
TProfileResult clientAPI_AttribRead( TClientID clientID, TReadHandleReq readHandle )
{
    /* Ethan: to do, we havent consider Blob Read here yet. */
    uint16_t readOffset = 0;
    gaprole_States_t  gap_state;
    uint16_t        local_MDL_ID;
    TProfileResult Result = ProfileResult_InvalidParameter;

    centralGetGapParameter(GAPPRRA_ROLE_STATE, &gap_state);
    centralGetGapParameter(GAPPRRA_CONNHANDLE, &local_MDL_ID);
    //centralGetGapParameter(GAPPRRA_DSDATAOFFSET, &wOffset);

    /* check if client ID is valid or not. */
    if(clientID >= clientRegNum)
    {
        DBG_BUFFER(MODULE_PROFILE,LEVEL_ERROR,"clientAPI_AttribRead: Client ID input error!",0);
        return(ProfileResult_AppErr);
    }

    if ( gap_state == GAP_CENTRAL_STATE_CONNECTED )
    {
        if ( blueAPI_GATTAttributeReadReq(local_MDL_ID,
                                         blueAPI_GATTReadTypeBasic,
                                         readOffset,  /* readOffset */
                                         readHandle.wHandle,  /* startHandle     */
                                         0,          /* endHandle   */
                                         0,          /* UUID16      */
                                         NULL        /* pUUID128    */
                                        )
           )
        {
            Result = ProfileResult_Success;
            /* remember the client id for this time request, will used in response handle later. */
            read_client_id = clientID;
#ifdef PROFILE_DEBUG
            DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"clientAPI_AttribRead: local_MDL_ID=%d, type=%d, readOffset=%d, handle=0x%4.4x",
                    4,local_MDL_ID,blueAPI_GATTReadTypeBasic,readOffset,readHandle.wHandle);
#endif
        }
        else
        {
            DBG_BUFFER(MODULE_PROFILE,LEVEL_ERROR,"clientAPI_AttribRead: request fail!",0);
        }
    }
    else
    {
        DBG_BUFFER(MODULE_PROFILE,LEVEL_WARN,"clientAPI_AttribRead: Please make your device connected when send discovery request!",0);
    }

    return( Result );
}

/**
  * @brief  read characteristic by 16 bit UUID request.
  * @param  clientID: client ID of specific client module.
  * @param  readUUIDReq: request 16 bit UUID.
  * @retval ProfileResult_Success: discovery request success.
  * @retval other: discovery request failed.
  */
TProfileResult clientAPI_AttribReadUsingUUID( TClientID clientID, TReadUUIDReq readUUIDReq )
{
    gaprole_States_t  gap_state;
    uint16_t        local_MDL_ID;
    TProfileResult Result = ProfileResult_InvalidParameter;

    centralGetGapParameter(GAPPRRA_ROLE_STATE, &gap_state);
    centralGetGapParameter(GAPPRRA_CONNHANDLE, &local_MDL_ID);

    /* check if client ID is valid or not. */
    if(clientID >= clientRegNum)
    {
        DBG_BUFFER(MODULE_PROFILE,LEVEL_ERROR,"clientAPI_AttribReadUsingUUID: Client ID input error!",0);
        return(ProfileResult_AppErr);
    }

    if ( gap_state == GAP_CENTRAL_STATE_CONNECTED )
    {
        /* set handle range defaults: */
        if ( readUUIDReq.wStartHandle == 0 )
        {
            readUUIDReq.wStartHandle = 0x0001;
        }
        if ( readUUIDReq.wEndHandle == 0 )
        {
            readUUIDReq.wEndHandle = 0xFFFF;
        }

        if ( blueAPI_GATTAttributeReadReq(local_MDL_ID,
                                         blueAPI_GATTReadTypeByUUID,
                                         0,  /* readOffset */
                                         readUUIDReq.wStartHandle,  /* startHandle */
                                         readUUIDReq.wEndHandle,  /* endHandle   */
                                         readUUIDReq.UUID16,          /* UUID16      */
                                         NULL        /* pUUID128    */
                                        )
           )
        {
            Result = ProfileResult_Success;
            /* remember the client id for this time request, will used in response handle later. */
            read_client_id = clientID;
#ifdef PROFILE_DEBUG
            DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"clientAPI_AttribReadUsingUUID: local_MDL_ID=%d, type=%d, start=0x%4.4x, end=0x%4.4x, UUID16=0x%4.4x",
                    5,local_MDL_ID,blueAPI_GATTReadTypeByUUID,readUUIDReq.wStartHandle,readUUIDReq.wEndHandle,readUUIDReq.UUID16);
#endif
        }
        else
        {
            DBG_BUFFER(MODULE_PROFILE,LEVEL_ERROR,"clientAPI_AttribReadUsingUUID: request fail!",0);
        }
    }
    else
    {
        DBG_BUFFER(MODULE_PROFILE,LEVEL_WARN,"clientAPI_AttribReadUsingUUID: Please make your device connected when send discovery request!",0);
    }

    return( Result );
}

/**
  * @brief  read characteristic by 128 bit UUID request.
  * @param  clientID: client ID of specific client module.
  * @param  readUUID128Req: request 128 bit UUID.
  * @retval ProfileResult_Success: discovery request success.
  * @retval other: discovery request failed.
  */
TProfileResult clientAPI_AttribReadUsingUUID128( TClientID clientID, TReadUUID128Req readUUID128Req )
{
    uint8_t *       pUUID128 = readUUID128Req.UUID128;
    gaprole_States_t  gap_state;
    uint16_t        local_MDL_ID;
    TProfileResult Result = ProfileResult_InvalidParameter;

    centralGetGapParameter(GAPPRRA_ROLE_STATE, &gap_state);
    centralGetGapParameter(GAPPRRA_CONNHANDLE, &local_MDL_ID);

    /* check if client ID is valid or not. */
    if(clientID >= clientRegNum)
    {
        DBG_BUFFER(MODULE_PROFILE,LEVEL_ERROR,"clientAPI_AttribReadUsingUUID128: Client ID input error!",0);
        return(ProfileResult_AppErr);
    }

    if ( gap_state == GAP_CENTRAL_STATE_CONNECTED )
    {
        /* set handle range defaults: */
        if ( readUUID128Req.wStartHandle == 0 )
        {
            readUUID128Req.wStartHandle = 0x0001;
        }
        if ( readUUID128Req.wEndHandle == 0 )
        {
            readUUID128Req.wEndHandle = 0xFFFF;
        }

        if ( blueAPI_GATTAttributeReadReq(local_MDL_ID,
                                         blueAPI_GATTReadTypeByUUID,
                                         0,  /* readOffset */
                                         readUUID128Req.wStartHandle,  /* startHandle */
                                         readUUID128Req.wEndHandle,  /* endHandle   */
                                         0,          /* UUID16      */
                                         pUUID128        /* pUUID128    */
                                        )
           )
        {
            Result = ProfileResult_Success;
            /* remember the client id for this time request, will used in response handle later. */
            read_client_id = clientID;
#ifdef PROFILE_DEBUG
            DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"clientAPI_AttribReadUsingUUID128: local_MDL_ID=%d, type=%d, start=0x%4.4x, end=0x%4.4x, pUUID128=%p",
                    5,local_MDL_ID,blueAPI_GATTReadTypeByUUID,readUUID128Req.wStartHandle,readUUID128Req.wEndHandle,pUUID128);
#endif
        }
        else
        {
            DBG_BUFFER(MODULE_PROFILE,LEVEL_ERROR,"clientAPI_AttribReadUsingUUID128: request fail!",0);
        }
    }
    else
    {
        DBG_BUFFER(MODULE_PROFILE,LEVEL_WARN,"clientAPI_AttribReadUsingUUID128: Please make your device connected when send discovery request!",0);
    }

    return( Result );
}

/**
  * @brief  write characteristic request.
  * @param  clientID: client ID of specific client module.
  * @param  pWriteReq: write request data.
  * @retval ProfileResult_Success: discovery request success.
  * @retval other: discovery request failed.
  */
TProfileResult clientAPI_AttribWrite( TClientID clientID, PWriteReq pWriteReq )
{
    TProfileResult Result = ProfileResult_InvalidParameter;
    gaprole_States_t  gap_state;
    centralGetGapParameter(GAPPRRA_ROLE_STATE, &gap_state);

    /* check if client ID is valid or not. */
    if(clientID >= clientRegNum)
    {
        DBG_BUFFER(MODULE_PROFILE,LEVEL_ERROR,"clientAPI_AttribWrite: Client ID input error!",0);
        return(ProfileResult_AppErr);
    }

    if ( gap_state == GAP_CENTRAL_STATE_CONNECTED )
    {
        void       * pBuffer;
        uint16_t     local_MDL_ID;
        uint16_t     wDsPoolId;
        uint16_t     wDsOffset;
        uint16_t     wOffset;
        uint8_t      wDsCredits;

        centralGetGapParameter(GAPPRRA_CONNHANDLE, &local_MDL_ID);
        centralGetGapParameter(GAPPRRA_DSPOOLID, &wDsPoolId);
        centralGetGapParameter(GAPPRRA_DSDATAOFFSET, &wDsOffset);
        centralGetGapParameter(GAPPRRA_MAXTPDUDSCREDITS, &wDsCredits);
        wOffset = wDsOffset + 3;

        /* check if wDsCredits is valid or not. */
        if(wDsCredits == 0)
        {
            DBG_BUFFER(MODULE_PROFILE,LEVEL_ERROR,"clientAPI_AttribWrite: DsCredits zero!",0);
            return(ProfileResult_AppErr);
        }

        /* copy attribute value to buffer position that allows re-usage by stack */
        /* without copying ..                                                   */
        if ( blueAPI_BufferGet(wDsPoolId,
                               pWriteReq->wLength,
                               wOffset,
                               (void **)&pBuffer) == blueAPI_CauseSuccess )
        {
            memcpy( ((uint8_t *)pBuffer)+wOffset, pWriteReq->pData, pWriteReq->wLength );

            if ( blueAPI_GATTAttributeWriteReq(
                                     pBuffer,
                                     local_MDL_ID,
                                     pWriteReq->wType,
                                     pWriteReq->wHandle,           /* handle */
                                     pWriteReq->wLength,           /* length */
                                     wOffset                       /* offset */
                                              )
               )
            {
                Result = ProfileResult_Success;
                /* decrease wDsCredits value when write command send to upper stack success. */
                if(pWriteReq->wType == blueAPI_GATTWriteTypeCommand)
                {
                    wDsCredits--;
                    centralSetGapParameter(GAPPRRA_MAXTPDUDSCREDITS, sizeof(uint8_t), &wDsCredits);
                }
                /* remember the client id for this time request, will used in response handle later. */
                write_client_id = clientID;
#ifdef PROFILE_DEBUG
                DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"clientAPI_AttribWrite: local_MDL_ID=%d, wType=%d, wHandle=0x%4.4x, wLength=%d",
                        4,local_MDL_ID,pWriteReq->wType,pWriteReq->wHandle,pWriteReq->wLength);
#endif
            }
            else
            {
                blueAPI_BufferRelease( pBuffer );
                DBG_BUFFER(MODULE_PROFILE,LEVEL_ERROR,"clientAPI_AttribWrite: request fail!",0);
            }
        }
        else
        {
            DBG_BUFFER(MODULE_PROFILE,LEVEL_ERROR,"clientAPI_AttribWrite: blueAPI_BufferGet fail!",0);
        }
    }
    else
    {
        DBG_BUFFER(MODULE_PROFILE,LEVEL_WARN,"clientAPI_AttribWrite: Please make your device connected when send discovery request!",0);
    }

    return( Result );
}

/**
  * @brief  confirm from application when receive indication of server.
  * @param  local_MDL_id: local MDL id indicate which link is.
  * @retval TRUE: confirm OK.
  * @retval FALSE: confirm failed.
  */
bool clientAPI_AttribIndConf( uint16_t local_MDL_id )
{
    bool result = FALSE;
    /* send response/ack */
    if ( blueAPI_GATTAttributeConf(local_MDL_id) )
    {
        result = TRUE;
#ifdef PROFILE_DEBUG
        DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"clientAPI_AttribIndConf: success.",0);
#endif
    }
    return result;
}

/**
  * @brief  switch app result to Profile result.
  * @param  appResult: app callback result.
  * @retval profileResult.
  */
TProfileResult  clientAPI_GetProfileResult(TAppResult appResult)
{
    TProfileResult profileResult = ProfileResult_Success;
    switch(appResult)
    {
        case AppResult_Success:
            profileResult = ProfileResult_Success;
            break;

        case AppResult_Pending:
            profileResult = ProfileResult_AppErr_Pending;
            break;
        default:
             profileResult = ProfileResult_Unlikely;            
    }
    return profileResult;
}

/**
  * @brief  handle DisconnectMDLInd.
  * @param  pDisconnectMDLInd: message from upper stack.
  * @retval TRUE: need to release buffer.
  *         FALSE: no need to release buffer.
  */
static bool clientHandle_DisconnectMDLInd(PBlueAPI_DisconnectMDLInd pDisconnectMDLInd)
{
    TClientID tmp_client_id;
    TRegClientCBsInfo *pCBsInfo = NULL;
    
#ifdef PROFILE_DEBUG
    DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"clientHandle_DisconnectMDLInd: local_MDL_ID = %d, cause = %d",
                    2, pDisconnectMDLInd->local_MDL_ID, pDisconnectMDLInd->cause);
#endif
    
    /* Reset the static state variables when disconnect. */
    disc_start_handle = 0;
    disc_end_handle = 0;
    disc_state = DISC_STATE_IDLE;
    disc_client_id = AppProcessGeneralClientMsgID;
    read_client_id = AppProcessGeneralClientMsgID;
    write_client_id = AppProcessGeneralClientMsgID;
    
    /* Inform each specific client to reset it's state. */
    for(tmp_client_id = 1; tmp_client_id < clientRegNum; tmp_client_id++)
    {
        pCBsInfo = client_FindCBsInfoByID( tmp_client_id );
        if( pCBsInfo && pCBsInfo->pCBs->pfnDisconnectCB )
        {
            (*(pCBsInfo->pCBs->pfnDisconnectCB))();
        }
        else
        {
            /* Do nothing. */
        }
    }
    return( false );
}

/**
  * @brief  handle blueAPI_Event_GATTDiscoveryRsp.
  * @param  pGATTDiscoveryRsp: message from upper stack.
  * @retval TRUE: need to release buffer.
  *         FALSE: no need to release buffer.
  */
static bool clientHandle_GATTDiscoveryRsp(PBlueAPI_GATTDiscoveryRsp pGATTDiscoveryRsp)
{
#ifdef PROFILE_DEBUG
    DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"clientHandle_GATTDiscoveryRsp: subCause=0x%x",1,pGATTDiscoveryRsp->subCause);
#endif
    return( true );
}

/**
  * @brief  handle blueAPI_Event_GATTDiscoveryInd.
  * @param  pGATTDiscoveryInd: message from upper stack.
  * @retval TRUE: need to release buffer.
  *         FALSE: no need to release buffer.
  */
static bool clientHandle_GATTDiscoveryInd(PBlueAPI_GATTDiscoveryInd pGATTDiscoveryInd)
{
    int        i, j;
    uint16_t   wEndingHandle = 0;   /* used for continuation search */
    TDiscoverResultType     resultType;     /* data can be sent to upper app. */
    TDiscoverResultData     resultData;     /* data can be sent to upper app. */
    bool       discComplete = FALSE;        /* used to indicate a discover procedure completed. */

#ifdef PROFILE_DEBUG
    DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"clientHandle_GATTDiscoveryInd: subCause=0x%x, type=%d, count=%d",
            3,pGATTDiscoveryInd->subCause,pGATTDiscoveryInd->discoveryType,pGATTDiscoveryInd->elementCount);
#endif

    if ( pGATTDiscoveryInd->subCause == GATT_SUCCESS )
    {
        uint32_t dwUUID128[4];

        switch( pGATTDiscoveryInd->discoveryType )
        {
            case blueAPI_GATTDiscoveryServices:
                /* discover all primary services */
                if ( pGATTDiscoveryInd->elementLength == sizeof(TBlueAPI_GATTGenericElement16) )
                {
                    /* 2 handles + 16 bit UUID */
                    PBlueAPI_GATTServicesElement16   pElement16;

                    pElement16 = (PBlueAPI_GATTServicesElement16)(pGATTDiscoveryInd->list
                                                                 + pGATTDiscoveryInd->gap);
                    for ( i=0; i<pGATTDiscoveryInd->elementCount; i++ )
                    {
                        /* XXXX some output may be lost due to limited buffer pool ... */
#ifdef PROFILE_DEBUG
                        DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO," range: 0x%4.4x-0x%4.4x, service=<0x%4.4x>",
                                    3,pElement16->attHandle,pElement16->endGroupHandle,pElement16->UUID16);
#endif
                        wEndingHandle = pElement16->endGroupHandle;
                        
                        /* Discover result can be sent to upper app to store. */
                        if(pfnClientAppCB != NULL)
                        {
                            TClientAppCB_Data cb_data;
                            cb_data.cb_type = CLIENT_APP_CB_TYPE_DISC_RESULT;
                            cb_data.cb_content.discResultCB_Data.resultType = DISC_RESULT_ALL_SRV_UUID16;
                            cb_data.cb_content.discResultCB_Data.resultData.pSrvUUID16DiscData = pElement16;
                            (*pfnClientAppCB)(AppProcessGeneralClientMsgID, &cb_data);
                        }

                        /* Pointer to the next element found. */
                        pElement16++;
                    }
                }
                else
                {
                    /* 2 handles + 128 bit UUID */
                    PBlueAPI_GATTServicesElement128  pElement128;

                    pElement128 = (PBlueAPI_GATTServicesElement128)(pGATTDiscoveryInd->list
                                                                     + pGATTDiscoveryInd->gap);
                    for ( i=0; i<pGATTDiscoveryInd->elementCount; i++ )
                    {
                        /* XXXX some output may be lost due to limited buffer pool ... */
                        memcpy( dwUUID128, &pElement128->UUID128[0], sizeof(dwUUID128));
#ifdef PROFILE_DEBUG
                        DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO," range: 0x%4.4x-0x%4.4x, service=<0x%8.8x%8.8x%8.8x%8.8x>",
                                    6,pElement128->attHandle,pElement128->endGroupHandle,
                                    dwUUID128[3], dwUUID128[2], dwUUID128[1], dwUUID128[0]);
#endif
                        wEndingHandle = pElement128->endGroupHandle;

                        /* Discover result can be sent to upper app to store. */
                        if(pfnClientAppCB != NULL)
                        {
                            TClientAppCB_Data cb_data;
                            cb_data.cb_type = CLIENT_APP_CB_TYPE_DISC_RESULT;
                            cb_data.cb_content.discResultCB_Data.resultType = DISC_RESULT_ALL_SRV_UUID128;
                            cb_data.cb_content.discResultCB_Data.resultData.pSrvUUID128DiscData = pElement128;
                            (*pfnClientAppCB)(AppProcessGeneralClientMsgID, &cb_data);
                        }

                        /* Pointer to the next element found. */
                        pElement128++;
                    }
                }
                break;

            case blueAPI_GATTDiscoveryServiceByUUID:
                /* discover primary services by UUID */
                {
                    /* 2 handles */
                    PBlueAPI_GATTServiceByUUIDElement  pElementUUID;
                    TRegClientCBsInfo *pCBsInfo;

                    pElementUUID = (PBlueAPI_GATTServiceByUUIDElement)(pGATTDiscoveryInd->list
                                                                          + pGATTDiscoveryInd->gap);
                    pCBsInfo = client_FindCBsInfoByID( disc_client_id );
                    for ( i=0; i<pGATTDiscoveryInd->elementCount; i++ )
                    {
                        /* XXXX some output may be lost due to limited buffer pool ... */
#ifdef PROFILE_DEBUG
                        DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO," range: 0x%4.4x-0x%4.4x",
                                    2,pElementUUID->attHandle,pElementUUID->endGroupHandle);
#endif
                        wEndingHandle = pElementUUID->endGroupHandle;

                        /* Discover result can be sent to upper app to store. */
                        if( pCBsInfo && pCBsInfo->pCBs->pfnDiscoverResultCB )
                        {
                            resultType = DISC_RESULT_SRV_DATA;
                            resultData.pSrvDiscData = pElementUUID;
                            (*(pCBsInfo->pCBs->pfnDiscoverResultCB))( resultType, resultData );
                        }
                        else if(pfnClientAppCB)
                        {
                            /* if specific client callback not found, send the data to app. */
                            TClientAppCB_Data cb_data;
                            cb_data.cb_type = CLIENT_APP_CB_TYPE_DISC_RESULT;
                            cb_data.cb_content.discResultCB_Data.resultType = DISC_RESULT_SRV_DATA;
                            cb_data.cb_content.discResultCB_Data.resultData.pSrvDiscData = pElementUUID;
                            (*pfnClientAppCB)(AppProcessGeneralClientMsgID, &cb_data);
                        }

                        /* Pointer to the next element found. */
                        pElementUUID++;
                    }
                }
                break;

            case blueAPI_GATTDiscoveryRelationship:
                /* discover relationships */
                if ( pGATTDiscoveryInd->elementLength == sizeof(TBlueAPI_GATTRelationshipElement16) )
                {
                    /* 3 handles + 16 bit UUID */
                    PBlueAPI_GATTRelationshipElement16   pElement16;
                    TRegClientCBsInfo *pCBsInfo;

                    pElement16 = (PBlueAPI_GATTRelationshipElement16)(pGATTDiscoveryInd->list
                                                                          + pGATTDiscoveryInd->gap);
                    pCBsInfo = client_FindCBsInfoByID( disc_client_id );
                    for ( i=0; i<pGATTDiscoveryInd->elementCount; i++ )
                    {
                        /* XXXX some output may be lost due to limited buffer pool ... */
#ifdef PROFILE_DEBUG
                        DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO," decl=0x%4.4x, range: 0x%4.4x-0x%4.4x, service=<0x%4.4x>",
                                    4,pElement16->declHandle,
                                      pElement16->attHandle,
                                      pElement16->endGroupHandle,
                                      pElement16->UUID16);
#endif
                        wEndingHandle = pElement16->declHandle;

                        /* Discover result can be sent to upper app to store. */
                        if( pCBsInfo && pCBsInfo->pCBs->pfnDiscoverResultCB )
                        {
                            resultType = DISC_RESULT_RELATION_UUID16;
                            resultData.pRelationUUID16DiscData = pElement16;
                            (*(pCBsInfo->pCBs->pfnDiscoverResultCB))( resultType, resultData );
                        }
                        else if(pfnClientAppCB)
                        {
                            /* if specific client callback not found, send the data to app. */
                            TClientAppCB_Data cb_data;
                            cb_data.cb_type = CLIENT_APP_CB_TYPE_DISC_RESULT;
                            cb_data.cb_content.discResultCB_Data.resultType = DISC_RESULT_RELATION_UUID16;
                            cb_data.cb_content.discResultCB_Data.resultData.pRelationUUID16DiscData = pElement16;
                            (*pfnClientAppCB)(AppProcessGeneralClientMsgID, &cb_data);
                        }

                        /* Pointer to the next element found. */
                        pElement16++;
                    }
                }
                else
                {
                    /* 3 handles + 128 bit UUID */
                    PBlueAPI_GATTRelationshipElement128  pElement128;
                    TRegClientCBsInfo *pCBsInfo;

                    pElement128 = (PBlueAPI_GATTRelationshipElement128)(pGATTDiscoveryInd->list
                                                                          + pGATTDiscoveryInd->gap);
                    pCBsInfo = client_FindCBsInfoByID( disc_client_id );
                    for ( i=0; i<pGATTDiscoveryInd->elementCount; i++ )
                    {
                        /* XXXX some output may be lost due to limited buffer pool ... */
                        memcpy( dwUUID128, &pElement128->UUID128[0], sizeof(dwUUID128));
#ifdef PROFILE_DEBUG
                        DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO," decl=0x%4.4x, range: 0x%4.4x-0x%4.4x, service=<0x%8.8x%8.8x%8.8x%8.8x>",
                                    7,pElement128->declHandle,
                                      pElement128->attHandle,
                                      pElement128->endGroupHandle,
                                      dwUUID128[3], dwUUID128[2], dwUUID128[1], dwUUID128[0]);
#endif

                        /* XXXXMJMJ: for type=GATT_TYPE_DISCOVERY_RELATION not yet fully
                                         *  implemented: applic. can retrieve the 128 bit UUID thru
                                         *  GATT_ATTRIBUTE_READ_REQ with the appropriate attribute handle
                                         *  (just as on ATT layer ...) and an updated search with start handle
                                         *  increased by 1 in a new GATT_DISCOVERY_REQ.
                                         *  work-around check:
                                         */
                        for ( j=0; j<UUID_128BIT_SIZE; j++ )
                        {
                            if ( pElement128->UUID128[j] != 0 )
                                break;
                        }
                        if ( j == UUID_128BIT_SIZE )
                        {
                            /* UUID received is 0 */
                            DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO," !! get 128 bit UUID with command \"read x%4.4x\" !!",
                                    1,pElement128->attHandle);
                        }
                        /* end of work-around check */
                        wEndingHandle = pElement128->declHandle;

                        /* Discover result can be sent to upper app to store. */
                        if( pCBsInfo && pCBsInfo->pCBs->pfnDiscoverResultCB )
                        {
                            resultType = DISC_RESULT_RELATION_UUID128;
                            resultData.pRelationUUID128DiscData = pElement128;
                            (*(pCBsInfo->pCBs->pfnDiscoverResultCB))( resultType, resultData );
                        }
                        else if(pfnClientAppCB)
                        {
                            /* if specific client callback not found, send the data to app. */
                            TClientAppCB_Data cb_data;
                            cb_data.cb_type = CLIENT_APP_CB_TYPE_DISC_RESULT;
                            cb_data.cb_content.discResultCB_Data.resultType = DISC_RESULT_RELATION_UUID128;
                            cb_data.cb_content.discResultCB_Data.resultData.pRelationUUID128DiscData = pElement128;
                            (*pfnClientAppCB)(AppProcessGeneralClientMsgID, &cb_data);
                        }

                        /* Pointer to the next element found. */
                        pElement128++;
                    }
                }
                break;

            case blueAPI_GATTDiscoveryCharacteristics:
                /* discover all characteristics of a service */
                if ( pGATTDiscoveryInd->elementLength == sizeof(TBlueAPI_GATTCharacteristicsElement16) )
                {
                    /* declaration handle, properties, value handle + 16 bit UUID */
                    PBlueAPI_GATTCharacteristicsElement16   pElement16;
                    TRegClientCBsInfo *pCBsInfo;

                    pElement16 = (PBlueAPI_GATTCharacteristicsElement16)(pGATTDiscoveryInd->list
                                                                          + pGATTDiscoveryInd->gap);
                    pCBsInfo = client_FindCBsInfoByID( disc_client_id );
                    for ( i=0; i<pGATTDiscoveryInd->elementCount; i++ )
                    {
                        /* XXXX some output may be lost due to limited buffer pool ... */
#ifdef PROFILE_DEBUG
                        DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO," decl hndl=0x%4.4x, prop=0x%2.2x, value hndl=0x%4.4x, UUID=<0x%4.4x>",
                                    4,pElement16->declHandle,
                                      pElement16->properties,
                                      pElement16->valueHandle,
                                      pElement16->UUID16);
#endif
                        wEndingHandle = pElement16->valueHandle;

                        /* Discover result can be sent to upper app to store. */
                        if( pCBsInfo && pCBsInfo->pCBs->pfnDiscoverResultCB )
                        {
                            resultType = DISC_RESULT_CHAR_UUID16;
                            resultData.pCharUUID16DiscData = pElement16;
                            (*(pCBsInfo->pCBs->pfnDiscoverResultCB))( resultType, resultData );
                        }
                        else if(pfnClientAppCB)
                        {
                            /* if specific client callback not found, send the data to app. */
                            TClientAppCB_Data cb_data;
                            cb_data.cb_type = CLIENT_APP_CB_TYPE_DISC_RESULT;
                            cb_data.cb_content.discResultCB_Data.resultType = DISC_RESULT_CHAR_UUID16;
                            cb_data.cb_content.discResultCB_Data.resultData.pCharUUID16DiscData = pElement16;
                            (*pfnClientAppCB)(AppProcessGeneralClientMsgID, &cb_data);
                        }

                        /* Pointer to the next element found. */
                        pElement16++;
                    }
                }
                else
                {
                    /* declaration handle, properties, value handle + 128 bit UUID */
                    PBlueAPI_GATTCharacteristicsElement128  pElement128;
                    TRegClientCBsInfo *pCBsInfo;

                    pElement128 = (PBlueAPI_GATTCharacteristicsElement128)(pGATTDiscoveryInd->list
                                                                            + pGATTDiscoveryInd->gap);
                    pCBsInfo = client_FindCBsInfoByID( disc_client_id );
                    for ( i=0; i<pGATTDiscoveryInd->elementCount; i++ )
                    {
                        /* XXXX some output may be lost due to limited buffer pool ... */
                        memcpy( dwUUID128, &pElement128->UUID128[0], sizeof(dwUUID128));
#ifdef PROFILE_DEBUG
                        DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO," decl hndl=0x%4.4x, prop=0x%2.2x, value hndl=0x%4.4x, UUID=<0x%8.8x%8.8x%8.8x%8.8x>",
                                    7,pElement128->declHandle,
                                       pElement128->properties,
                                       pElement128->valueHandle,
                                       dwUUID128[3], dwUUID128[2], dwUUID128[1], dwUUID128[0]);
#endif
                        wEndingHandle = pElement128->valueHandle;

                        /* Discover result can be sent to upper app to store. */
                        if( pCBsInfo && pCBsInfo->pCBs->pfnDiscoverResultCB )
                        {
                            resultType = DISC_RESULT_CHAR_UUID128;
                            resultData.pCharUUID128DiscData = pElement128;
                            (*(pCBsInfo->pCBs->pfnDiscoverResultCB))( resultType, resultData );
                        }
                        else if(pfnClientAppCB)
                        {
                            /* if specific client callback not found, send the data to app. */
                            TClientAppCB_Data cb_data;
                            cb_data.cb_type = CLIENT_APP_CB_TYPE_DISC_RESULT;
                            cb_data.cb_content.discResultCB_Data.resultType = DISC_RESULT_CHAR_UUID128;
                            cb_data.cb_content.discResultCB_Data.resultData.pCharUUID128DiscData = pElement128;
                            (*pfnClientAppCB)(AppProcessGeneralClientMsgID, &cb_data);
                        }

                        /* Pointer to the next element found. */
                        pElement128++;
                    }
                }
                break;

            case blueAPI_GATTDiscoveryCharacDescriptors:
                /* discovery of characteristic descriptors of a service */
                /* (or list of UUIDs ..)  */
                if ( pGATTDiscoveryInd->elementLength == sizeof(TBlueAPI_GATTCharacDescriptorsElement16) )
                {
                    /* handle, properties, 16 bit UUID */
                    PBlueAPI_GATTCharacDescriptorsElement16   pElement16;
                    TRegClientCBsInfo *pCBsInfo;

                    pElement16 = (PBlueAPI_GATTCharacDescriptorsElement16)(pGATTDiscoveryInd->list
                                                                            + pGATTDiscoveryInd->gap);
                    pCBsInfo = client_FindCBsInfoByID( disc_client_id );
                    for ( i=0; i<pGATTDiscoveryInd->elementCount; i++ )
                    {
                        /* XXXX some output may be lost due to limited buffer pool ... */
#ifdef PROFILE_DEBUG
                        DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO," handle=0x%4.4x, UUID=<0x%4.4x>",
                                    2,pElement16->handle,pElement16->UUID16);
#endif
                        wEndingHandle = pElement16->handle;

                        /* Discover result can be sent to upper app to store. */
                        if( pCBsInfo && pCBsInfo->pCBs->pfnDiscoverResultCB )
                        {
                            resultType = DISC_RESULT_CHAR_DESC_UUID16;
                            resultData.pCharDescUUID16DiscData = pElement16;
                            (*(pCBsInfo->pCBs->pfnDiscoverResultCB))( resultType, resultData );
                        }
                        else if(pfnClientAppCB)
                        {
                            /* if specific client callback not found, send the data to app. */
                            TClientAppCB_Data cb_data;
                            cb_data.cb_type = CLIENT_APP_CB_TYPE_DISC_RESULT;
                            cb_data.cb_content.discResultCB_Data.resultType = DISC_RESULT_CHAR_DESC_UUID16;
                            cb_data.cb_content.discResultCB_Data.resultData.pCharDescUUID16DiscData = pElement16;
                            (*pfnClientAppCB)(AppProcessGeneralClientMsgID, &cb_data);
                        }

                        /* Pointer to the next element found. */
                        pElement16++;
                    }
                }
                else
                {
                    /* handle, 128 bit UUID */
                    PBlueAPI_GATTCharacDescriptorsElement128  pElement128;
                    TRegClientCBsInfo *pCBsInfo;

                    pElement128 = (PBlueAPI_GATTCharacDescriptorsElement128)(pGATTDiscoveryInd->list
                                                                              + pGATTDiscoveryInd->gap);
                    pCBsInfo = client_FindCBsInfoByID( disc_client_id );
                    for ( i=0; i<pGATTDiscoveryInd->elementCount; i++ )
                    {
                        /* XXXX some output may be lost due to limited buffer pool ... */
                        memcpy( dwUUID128, &pElement128->UUID128[0], sizeof(dwUUID128));
#ifdef PROFILE_DEBUG
                        DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO," handle=0x%4.4x, UUID=<0x%8.8x%8.8x%8.8x%8.8x>",
                                    5,pElement128->handle,
                                      dwUUID128[3], dwUUID128[2], dwUUID128[1], dwUUID128[0]);
#endif
                        wEndingHandle = pElement128->handle;

                        /* Discover result can be sent to upper app to store. */
                        if( pCBsInfo && pCBsInfo->pCBs->pfnDiscoverResultCB )
                        {
                            resultType = DISC_RESULT_CHAR_DESC_UUID128;
                            resultData.pCharDescUUID128DiscData = pElement128;
                            (*(pCBsInfo->pCBs->pfnDiscoverResultCB))( resultType, resultData );
                        }
                        else if(pfnClientAppCB)
                        {
                            /* if specific client callback not found, send the data to app. */
                            TClientAppCB_Data cb_data;
                            cb_data.cb_type = CLIENT_APP_CB_TYPE_DISC_RESULT;
                            cb_data.cb_content.discResultCB_Data.resultType = DISC_RESULT_CHAR_DESC_UUID128;
                            cb_data.cb_content.discResultCB_Data.resultData.pCharDescUUID128DiscData = pElement128;
                            (*pfnClientAppCB)(AppProcessGeneralClientMsgID, &cb_data);
                        }

                        /* Pointer to the next element found. */
                        pElement128++;
                    }
                }
                break;
        }

        /* set next start handle, for continue discovery. */
        disc_start_handle = wEndingHandle + 1;

        if ( wEndingHandle >= disc_end_handle )
        {
            disc_start_handle = 0;
            disc_end_handle = 0;   /* discovery complete */
            discComplete = TRUE;
        }
    }
    else
    {
        /* no more results found */
        disc_start_handle = 0;
        disc_end_handle = 0;     /* discovery complete */
        discComplete = TRUE;
    }
    
    /* do continue discovery here. */
    if ( blueAPI_GATTDiscoveryConf(pGATTDiscoveryInd->local_MDL_ID,
                            pGATTDiscoveryInd->discoveryType,
                            disc_start_handle,  /* starting handle: continue search */
                            disc_end_handle
                           )
     )
    {
#ifdef PROFILE_DEBUG
        DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"GATTDiscoveryConf: start=0x%4.4x, end=0x%4.4x",
                    2,disc_start_handle,disc_end_handle);
#endif
    }

    /* if a discover procedure complete, switch disc state and notify the state to upper app. */
    if(discComplete)
    {
        TRegClientCBsInfo *pCBsInfo = client_FindCBsInfoByID( disc_client_id );
        switch(disc_state)
        {
            case DISC_STATE_SRV:
                disc_state = DISC_STATE_SRV_DONE;
                break;
            case DISC_STATE_RELATION:
                disc_state = DISC_STATE_RELATION_DONE;
                break;
            case DISC_STATE_CHAR:
                disc_state = DISC_STATE_CHAR_DONE;
                break;
            case DISC_STATE_CHAR_DESCRIPTOR:
                disc_state = DISC_STATE_CHAR_DESCRIPTOR_DONE;
                break;
            default:
                DBG_BUFFER(MODULE_PROFILE,LEVEL_ERROR,"Discovery State Error!",0);
                break;
        }
        /* send the disc_state to upper app. */
        if( pCBsInfo && pCBsInfo->pCBs->pfnDiscoverStateCB )
        {
            (*(pCBsInfo->pCBs->pfnDiscoverStateCB))( disc_state );
        }
        else if(pfnClientAppCB)
        {
            /* if specific client callback not found, send the data to app. */
            TClientAppCB_Data cb_data;
            cb_data.cb_type = CLIENT_APP_CB_TYPE_DISC_STATE;
            cb_data.cb_content.discStateCB_Data.discState = disc_state;
            (*pfnClientAppCB)(AppProcessGeneralClientMsgID, &cb_data);
        }
    }

    return( true );
}

/**
  * @brief  handle blueAPI_Event_GATTAttributeReadRsp.
  * @param  pGATTAttributeReadRsp: message from upper stack.
  * @retval TRUE: need to release buffer.
  *         FALSE: no need to release buffer.
  */
static bool clientHandle_GATTAttribReadRsp(PBlueAPI_GATTAttributeReadRsp pGATTAttributeReadRsp)
{
    uint16_t * pHandle = NULL;
    uint8_t  * pValue = NULL;
    int    iValueSize = 0;
    TClientRequestResult reqResult = CLIENT_REQ_FAIL;
    /* Find the specific client which send the read request. */
    TRegClientCBsInfo *pCBsInfo = client_FindCBsInfoByID( read_client_id );

#ifdef PROFILE_DEBUG
    DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"clientHandle_GATTAttribReadRsp: subCause=0x%x, length=%d",
            2,pGATTAttributeReadRsp->subCause,pGATTAttributeReadRsp->totalLength);
#endif

    if ( pGATTAttributeReadRsp->subCause == GATT_SUCCESS )
    {
        reqResult = CLIENT_REQ_SUCCESS;
        if ( pGATTAttributeReadRsp->totalLength > 0 )
        {
            int    i;
            int    iTruncated;   /* nbr. of bytes that have been truncated in last */
                               /* handle/value pair (ReadByTypeResp only ..)     */

            /* only OK if attribute values have same size. */
            iValueSize = pGATTAttributeReadRsp->attribLength;
            iTruncated = pGATTAttributeReadRsp->nbrOfHandles*(sizeof(uint16_t)+iValueSize) -
                                                           pGATTAttributeReadRsp->totalLength;
            pHandle    = (uint16_t *)(pGATTAttributeReadRsp->handlesData +   /* first handle */
                                        pGATTAttributeReadRsp->gap);
            pValue     = ((uint8_t *)pHandle) +                    /* first attrib. value */
                                        (pGATTAttributeReadRsp->nbrOfHandles*sizeof(uint16_t));
            
            for ( i=0; i<pGATTAttributeReadRsp->nbrOfHandles; i++ )
            {
                /* only Read Multiple may have not only one handle to process. */
                /* Ethan: to do, for multiple read we may have more consideration. */
                if ( (iTruncated != 0) && (i == (pGATTAttributeReadRsp->nbrOfHandles-1)) )
                    iValueSize -= iTruncated;

                /* Read result can be informed to upper app. */
                if( pCBsInfo && pCBsInfo->pCBs->pfnReadResultCB )
                {
                    (*(pCBsInfo->pCBs->pfnReadResultCB))( reqResult, *pHandle, iValueSize, pValue );
                }
                else if(pfnClientAppCB)
                {
                    /* if specific client callback not found, send the data to app. */
                    TClientAppCB_Data cb_data;
                    cb_data.cb_type = CLIENT_APP_CB_TYPE_READ_RESULT;
                    cb_data.cb_content.readResultCB_Data.result = reqResult;
                    cb_data.cb_content.readResultCB_Data.wHandle = *pHandle;
                    cb_data.cb_content.readResultCB_Data.iValueSize = iValueSize;
                    cb_data.cb_content.readResultCB_Data.pValue = pValue;
                    (*pfnClientAppCB)(AppProcessGeneralClientMsgID, &cb_data);
                }

                /* Pointer to the next handle value. */
                pHandle++;
                pValue += iValueSize;
            }

            /*Ethan: to do. End of for loop means this read request really completly processed. */
            
        }
        else
        {
            /* Read request success, but no data returned, this is unpossible, unless exceptions. */
        }
    }
    else
    {
        /* Read request, response data from peer device fail, inform upper app. */
        if(pCBsInfo && pCBsInfo->pCBs->pfnReadResultCB)
        {
            (*(pCBsInfo->pCBs->pfnReadResultCB))( reqResult, *pHandle, iValueSize, pValue );
        }
        else if(pfnClientAppCB)
        {
            /* if specific client callback not found, send the data to app. */
            TClientAppCB_Data cb_data;
            cb_data.cb_type = CLIENT_APP_CB_TYPE_READ_RESULT;
            cb_data.cb_content.readResultCB_Data.result = reqResult;
            cb_data.cb_content.readResultCB_Data.wHandle = *pHandle;
            cb_data.cb_content.readResultCB_Data.iValueSize = iValueSize;
            cb_data.cb_content.readResultCB_Data.pValue = pValue;
            (*pfnClientAppCB)(AppProcessGeneralClientMsgID, &cb_data);
        }
    }

    return( true );
}

/**
  * @brief  handle blueAPI_Event_GATTAttributeWriteRsp.
  * @param  pGATTAttributeWriteRsp: message from upper stack.
  * @retval TRUE: need to release buffer.
  *         FALSE: no need to release buffer.
  */
static bool clientHandle_GATTAttribWriteRsp(PBlueAPI_GATTAttributeWriteRsp pGATTAttributeWriteRsp)
{
    TClientRequestResult reqResult = CLIENT_REQ_FAIL;
    /* Find the specific client which send the write request. */
    TRegClientCBsInfo *pCBsInfo = client_FindCBsInfoByID( write_client_id );
    uint8_t wDsCredits;

    centralGetGapParameter(GAPPRRA_MAXTPDUDSCREDITS, &wDsCredits);
#ifdef PROFILE_DEBUG
    DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"clientHandle_GATTAttribWriteRsp: subCause=0x%x",1,pGATTAttributeWriteRsp->subCause);
#endif
    if ( pGATTAttributeWriteRsp->subCause == GATT_SUCCESS )
    {
        reqResult = CLIENT_REQ_SUCCESS;
    }

    /* Increase wDsCredits value when receive write command request's response. */
    if(pGATTAttributeWriteRsp->writeType == blueAPI_GATTWriteTypeCommand)
    {
        wDsCredits++;
        centralSetGapParameter(GAPPRRA_MAXTPDUDSCREDITS, sizeof(uint8_t), &wDsCredits);
    }

    /* Write result can be informed to upper app. */
    if(pCBsInfo && pCBsInfo->pCBs->pfnWriteResultCB)
    {
        (*(pCBsInfo->pCBs->pfnWriteResultCB))(reqResult);
    }
    else if(pfnClientAppCB)
    {
        /* if specific client callback not found, send the data to app directly. */
        TClientAppCB_Data cb_data;
        cb_data.cb_type = CLIENT_APP_CB_TYPE_WRITE_RESULT;
        cb_data.cb_content.writeResultCB_Data.result = reqResult;
        (*pfnClientAppCB)(AppProcessGeneralClientMsgID, &cb_data);
    }
    
    return( true );
}

/**
  * @brief  handle blueAPI_EventGATTAttributeInd and blueAPI_EventGATTAttributeNotificationInfo.
  * @param  Command: write flag, write request or write command.
  * @param  pGATTAttributeInd: message from upper stack.
  * @retval TRUE: need to release buffer.
  *         FALSE: no need to release buffer.
  */
static bool clientHandle_GATTAttribNotifInd(uint16_t Command,
                                      PBlueAPI_GATTAttributeInd pGATTAttributeInd)
{
    bool  Notify = (Command == blueAPI_EventGATTAttributeNotificationInfo);
    int   iValueSize = pGATTAttributeInd->attribLength;
    uint16_t *pHandle = &pGATTAttributeInd->attribHandle;
    uint8_t  *pValue = pGATTAttributeInd->data+pGATTAttributeInd->gap;
    TRegClientCBsInfo *pCBsInfo;
    TProfileResult wCause = ProfileResult_InvalidParameter;
    TAppResult appResult = AppResult_Success;

    pCBsInfo = client_FindCBsInfoByHandle( *pHandle );
#ifdef PROFILE_DEBUG
    DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"clientHandle_GATTAttribNotifInd: notif=%d, handle=0x%x",
            2,Notify,pGATTAttributeInd->attribHandle);
#endif

    /* Notify/Ind result can be sent to upper app to store. */
    if( pCBsInfo && pCBsInfo->pCBs->pfnNotifIndResultCB )
    {
        wCause = (*(pCBsInfo->pCBs->pfnNotifIndResultCB))( *pHandle, iValueSize, pValue );
    }
    else if(pfnClientAppCB)
    {
        /* if specific client callback not found, send the data to app. */
        TClientAppCB_Data cb_data;
        cb_data.cb_type = CLIENT_APP_CB_TYPE_NOTIF_IND_RESULT;
        cb_data.cb_content.notifIndResultCB_Data.wHandle = *pHandle;
        cb_data.cb_content.notifIndResultCB_Data.iValueSize = iValueSize;
        cb_data.cb_content.notifIndResultCB_Data.pValue = pValue;
        appResult = (*pfnClientAppCB)(AppProcessGeneralClientMsgID, &cb_data);
        wCause = clientAPI_GetProfileResult(appResult);
    }
    
    if ( (Notify == FALSE) && (wCause != ProfileResult_AppErr_Pending) )
    {
        /* send response/ack */
        if ( blueAPI_GATTAttributeConf(pGATTAttributeInd->local_MDL_ID) )
        {
#ifdef PROFILE_DEBUG
            DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"GATTAttributeConf",0);
#endif
        }
    }

    return( true );
}

/**
  * @brief  handle blueAPI messages.
  * @param  pMsg: message from upper stack.
  * @retval TRUE: need to release buffer.
  *         FALSE: no need to release buffer.
  */
bool client_HandleMessage( PBlueAPI_UsMessage pMsg )
{
    bool      ReleaseBuffer = false;

    switch( pMsg->Command )
    {
        default:
#ifdef PROFILE_DEBUG
            DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"clientHandle_BlueAPIMessage: unhandled command 0x%x",1,pMsg->Command);
#endif
            ReleaseBuffer = false;
            break;

        case blueAPI_EventDisconnectMDLInd:
            ReleaseBuffer = clientHandle_DisconnectMDLInd(&pMsg->p.DisconnectMDLInd );
    		break;

        case blueAPI_EventGATTDiscoveryRsp:
            ReleaseBuffer = clientHandle_GATTDiscoveryRsp( &pMsg->p.GATTDiscoveryRsp );
            break;

        case blueAPI_EventGATTDiscoveryInd:
            ReleaseBuffer = clientHandle_GATTDiscoveryInd( &pMsg->p.GATTDiscoveryInd );
            break;

        case blueAPI_EventGATTAttributeReadRsp:
            ReleaseBuffer = clientHandle_GATTAttribReadRsp( &pMsg->p.GATTAttributeReadRsp );
            break;

        case blueAPI_EventGATTAttributeWriteRsp:
            ReleaseBuffer = clientHandle_GATTAttribWriteRsp( &pMsg->p.GATTAttributeWriteRsp );
            break;

        case blueAPI_EventGATTAttributeInd:
        case blueAPI_EventGATTAttributeNotificationInfo:
            ReleaseBuffer = clientHandle_GATTAttribNotifInd(pMsg->Command,
                                           &pMsg->p.GATTAttributeInd );
            break;
    }

    if( ReleaseBuffer )
        blueAPI_BufferRelease( pMsg );

    return ReleaseBuffer;
}

