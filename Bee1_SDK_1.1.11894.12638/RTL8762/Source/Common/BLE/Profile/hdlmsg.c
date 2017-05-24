enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     hdlmsg.c
* @brief    handle GATT messages by profile.
* @details  handle GATT messages by profile.
* @author   
* @date     2015-5-10
* @version  v0.1
*********************************************************************************************************
*/
#include "profile.h"
#include <blueapi.h>
#include <string.h>
#include "trace.h"
#include "peripheral.h"
#include "profileConfig.h"

/**<  Number of all services used in application. */
extern uint8_t gServicesCount;
/**<  Global data struct of profile spesific data. */
extern TPROFILE_DATA gProfileData;
/**<  Array of all used services' informations. */
extern gattServiceInfos_t gattSrvInfo[PROFILE_SUPPORT_SERVICE_NUM];

/**<  find service state according to input service-handle. */
extern PGATTDService profileServiceFind(void * pServiceHandle );
/**<  Send service register complete event to application. */
extern void profileSrvStartCompleteEvt(uint8_t Status);
/**<  update CCCD bits from stack. */
extern void profileUpdateCCCDInfo(uint8_t ServiceId, uint16_t wAttribIndex, uint16_t wCCCBits);
/**<  send data-send complete event to application. */
extern void profileDataTxComplete(uint16_t wCredits);

/**
 * @brief handle blueAPI_Event_GATTServiceRegisterRsp.
 *
 * @param pGATTServiceRegisterRsp        upstream message from upper stack.
 * @return release buffer or not.
 * @retval TRUE 1
 * @retval FALSE 0
*/
bool profileHandle_GATTServiceRegisterRsp(
    PBlueAPI_GATTServiceRegisterRsp pGATTServiceRegisterRsp )
{
#ifdef PROFILE_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "profileHandle_GATTServiceRegisterRsp cause = 0x%x", 1,
               pGATTServiceRegisterRsp->cause);
#endif
    if ( pGATTServiceRegisterRsp->cause == blueAPI_CauseSuccess )
    {
        gattSrvInfo[gProfileData.iServiceCountIndex].Service.pServiceHandle =
            pGATTServiceRegisterRsp->serviceHandle;

        gProfileData.iServiceCountIndex++;
        if ( gProfileData.iServiceCountIndex == gServicesCount )
        {
            // service Register complete, send this to APP.lilly
            profileSrvStartCompleteEvt(SUCCESS);
            peripheral_Handle_ServiceRegisterCompleteEvt();
        }
        else
        {
            /* register next service */
            profileServiceRegister();
        }
    }
    else
    {
        gattSrvInfo[gProfileData.iServiceCountIndex].Service.Used           = false;
        gattSrvInfo[gProfileData.iServiceCountIndex].Service.pServiceHandle = NULL;
    }

    return ( true );
}

/**
 * @brief handle blueAPI_Event_GATTAttributeUpdateRsp.
 *
 * @param pGATTAttributeUpdateRsp        upstream message from upper stack.
 * @return release buffer or not.
 * @retval TRUE 1
 * @retval FALSE 0
*/
bool profileHandle_GATTAttributeUpdateRsp(
    PBlueAPI_GATTAttributeUpdateRsp pGATTAttributeUpdateRsp )
{
    bool  Error = false;
#ifdef PROFILE_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- GATTAttributeUpdateRsp cause = 0x%x subcause = 0x%x ", 2,
               pGATTAttributeUpdateRsp->cause, pGATTAttributeUpdateRsp->subCause);
#endif
    gProfileData.wDsCredits++ ;
    if ( pGATTAttributeUpdateRsp->cause == blueAPI_CauseSuccess )
    {
        profileDataTxComplete(gProfileData.wDsCredits);

    }
    else  if ( pGATTAttributeUpdateRsp->subCause == GATT_ERR_OUT_OF_RESOURCE )
    {
        Error = true;
    }
    else  if ( (pGATTAttributeUpdateRsp->subCause == GATT_ERR_NOTIF_IND_CFG) &&
               (pGATTAttributeUpdateRsp->count > 0)
             )
    {
        /* list of BDs that configured the CCCD for notifications/indications */
        /* is appended */
        PBlueAPI_GATTAttributeUpdateListElement  pElement;
        int   i;
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "GATTAttributeUpdateRsp GATT_ERR_NOTIF_IND_CFG.", 0);

        pElement = (PBlueAPI_GATTAttributeUpdateListElement)(pGATTAttributeUpdateRsp->list + pGATTAttributeUpdateRsp->gap);
        for ( i = 0; i < pGATTAttributeUpdateRsp->count ; i++ )
        {
            pElement++;
        }
    }
    if (pGATTAttributeUpdateRsp->subCause == GATT_ERR_NOTIF_IND_CFG)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "Check GATT_ERR_NOTIF_IND_CFG.AttribIndex", 1, pGATTAttributeUpdateRsp->attribIndex);
    }
    if ( Error )
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "GATTAttributeUpdateRsp Error %d.", 1, Error);
    }
    else
    {
        /* some services need to do some post update (sequence) processing (RACP ..): */
        /*
        if ( (pGATTData->iUpdateCnt == pGATTData->iUpdateConfirmed) ||
             (pGATTAttributeUpdateRsp->cause != blueAPI_CauseSuccess)
           )
        {
          gattdServiceUpdateCallback( pGATTData,
                pGATTAttributeUpdateRsp->subCause, pGATTAttributeUpdateRsp->attribIndex );
        }
        */
    }

    return ( true );
}

/**
 * @brief handle blueAPI_Event_GATTAttributeUpdateStatusInd.
 *
 * @param pGATTAttributeUpdateStatusInd      upstream message from upper stack.
 * @return release buffer or not.
 * @retval TRUE 1
 * @retval FALSE 0
*/
bool profileHandle_GATTAttributeUpdateStatusInd(
    PBlueAPI_GATTAttributeUpdateStatusInd pGATTAttributeUpdateStatusInd )
{
    PGATTDService  pService;
    
#ifdef PROFILE_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- profileHandle_GATTAttributeUpdateStatusInd subCause=0x%x",
               1, pGATTAttributeUpdateStatusInd->subCause);
#endif

    pService = profileServiceFind( pGATTAttributeUpdateStatusInd->serviceHandle );
    
    //notify service
    if(gattSrvInfo[pService->ServiceIDx].cbInfo.pfnUpdateStatusCB)
    {
        gattSrvInfo[pService->ServiceIDx].cbInfo.pfnUpdateStatusCB(
            pService->ServiceIDx, pGATTAttributeUpdateStatusInd->attribIndex);
    }
    
    if ( blueAPI_GATTAttributeUpdateStatusConf(
                pGATTAttributeUpdateStatusInd->serviceHandle,
                pGATTAttributeUpdateStatusInd->requestHandle,
                pGATTAttributeUpdateStatusInd->attribIndex
            )
       )
    {
#ifdef PROFILE_DEBUG    
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- --> GATTAttributeUpdateStatusConf", 0);
#endif
    }

    /* check if update sequence should be continued */
    //gProfileData.wDsCredits++ ;   /* Indication procedure do not use Credits. */
    if ( pGATTAttributeUpdateStatusInd->cause == blueAPI_CauseSuccess )
    {
        profileDataTxComplete(gProfileData.wDsCredits);
    }
    else
    {
        //gSrvSequnceCtrl.iUpdateCnt = 0;
    }

    /* Modified for RSC CSC Procedure Already In Progress */
    /* some services need to do some post update (sequence) processing (RSC, CSC): */
    /*  if ( gSrvSequnceCtrl.iUpdateCnt == 0 )
      {
          DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "--> gattdServiceUpdateCallback() executed.", 0);
      }
    */
    return ( true );
}

/**
 * @brief handle blueAPI_Event_GATTAttributeReadInd.
 *
 * @param pGATTAttributeReadInd      upstream message from upper stack.
 * @return release buffer or not.
 * @retval TRUE 1
 * @retval FALSE 0
*/
bool profileHandle_GATTAttributeReadInd(
    PBlueAPI_GATTAttributeReadInd pGATTAttributeReadInd )
{
    PGATTDService  pService;
    uint8_t *      pBuffer;
    uint16_t       wLength = 0;
    uint16_t       wOffset = gProfileData.wDsDataOffset;
    uint8_t *      pData   = NULL;
    TProfileResult wCause  = ProfileResult_InvalidParameter;

#ifdef PROFILE_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- profileHandle_GATTAttributeReadInd:idx=%d, offset=%d",
               2, pGATTAttributeReadInd->attribIndex, pGATTAttributeReadInd->readOffset);
#endif

    pService = profileServiceFind( pGATTAttributeReadInd->serviceHandle );
    if ( pService != (PGATTDService)NULL )
    {
        /* service/attribute specific read operation.                          */
        /* check for authorization etc. has been done by GATT layer already .. */

        wCause  = profileReadAttrib(pService->ServiceIDx, pGATTAttributeReadInd->attribIndex,
                                    pGATTAttributeReadInd->readOffset, &wLength, &pData );

    }

    if ( wCause == ProfileResult_AppErr_Pending)
    {
        return true;
    }


    /* send response */
    if ( wCause == ProfileResult_Success )
    {
        /* copy attribute value to buffer position that allows re-usage by stack */
        /* without copying ..                                                   */
        if ( blueAPI_BufferGet(
                    gProfileData.wDsPoolId,
                    wLength,
                    wOffset,
                    (void **)&pBuffer) == blueAPI_CauseSuccess )
        {
            memcpy( pBuffer + wOffset, pData, wLength );
        }
        else
        {
            wCause = ProfileResult_OutOfResource;
        }
    }
    if ( wCause != ProfileResult_Success )
    {
        pBuffer = NULL;
        wLength = 0;
    }

    if ( blueAPI_GATTAttributeReadConf(pBuffer,

                                       pGATTAttributeReadInd->local_MDL_ID,
                                       pService->pServiceHandle,
                                       (wCause != ProfileResult_Success) ?
                                       blueAPI_CauseLowerLayerError : blueAPI_CauseSuccess,
                                       (uint16_t)wCause,
                                       pGATTAttributeReadInd->attribIndex,
                                       wLength,
                                       wOffset
                                      )
       )
    {
#ifdef PROFILE_DEBUG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "--> GATTAttributeReadConf: wCause=%d, length=%d",
                   2, wCause, wLength);
#endif

    }
    else
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "!!! illegal parameter (e.g. offset too small)", 0);
        if ( pBuffer != NULL )
            blueAPI_BufferRelease(pBuffer);
    }

    return ( true );
}

/**
 * @brief handle blueAPI_Event_GATTAttributeWriteInd (triggered by ATT_WRITE_REQUEST)
 * and blueAPI_Event_GATTAttributeWriteCommandIngo (triggered by
 * ATT_WRITE_COMMAND). both have the same parameters.
 *
 * @param Command        type of write command.
 * @param pGATTAttributeWriteInd         upstream message from upper stack.
 * @return release buffer or not.
 * @retval TRUE 1
 * @retval FALSE 0
*/
bool profileHandle_GATTAttributeWriteIndCommandInfo(
    TBlueAPI_Command               Command,
    PBlueAPI_GATTAttributeWriteInd pGATTAttributeWriteInd )
{
    PGATTDService           pService;
    TGATTDWriteIndPostProc  WriteIndPostProc = NULL;
    TProfileResult          wCause  = ProfileResult_InvalidParameter;
#if 0

    static const char writeIndTxt[]         = "GATTAttributeWriteInd";
    static const char writeCommandInfoTxt[] = "GATTAttributeWriteCommandInfo";
    char * pTxt;

    if ( Command == blueAPI_EventGATTAttributeWriteCommandInfo )
        pTxt = (char *)writeCommandInfoTxt;
    else
        pTxt = (char *)writeIndTxt;
#endif

#ifdef PROFILE_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- :profileHandle_GATTAttributeWriteIndCommandInfo idx=%d, length=%d data = 0x%x gap=%x data[0]%x",
               5, pGATTAttributeWriteInd->attribIndex, pGATTAttributeWriteInd->attribLength,
               *(pGATTAttributeWriteInd->data + pGATTAttributeWriteInd->gap),
               pGATTAttributeWriteInd->gap,
               pGATTAttributeWriteInd->data[0]);
#endif

    pService = profileServiceFind(pGATTAttributeWriteInd->serviceHandle );
    if ( pService != (PGATTDService)NULL )
    {
        /* service/attribute specific write operation.                         */
        /* check for authorization etc. has been done by GATT layer already .. */

        wCause  = profileWriteAttrib(
                      pService->ServiceIDx, pGATTAttributeWriteInd->attribIndex,
                      pGATTAttributeWriteInd->attribLength,
                      pGATTAttributeWriteInd->data + pGATTAttributeWriteInd->gap,
                      &WriteIndPostProc
                  );
    }

    /* send response */
    if ( (Command == blueAPI_EventGATTAttributeWriteInd) && (wCause != ProfileResult_AppErr_Pending) )
    {
        if ( blueAPI_GATTAttributeWriteConf(pGATTAttributeWriteInd->local_MDL_ID,
                                            pService->pServiceHandle,
                                            (wCause != ProfileResult_Success) ?
                                            blueAPI_CauseLowerLayerError : blueAPI_CauseSuccess,
                                            (uint16_t)wCause,
                                            pGATTAttributeWriteInd->attribIndex
                                           )
           )
        {
#ifdef PROFILE_DEBUG
            DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "--> GATTAttributeWriteConf subCause=0x%x",
                       1, wCause);
#endif
        }
    }

    /* for services with RACP some post processing may be needed: */
    if ( WriteIndPostProc != 0 )
    {
        (*WriteIndPostProc)(
            pService->ServiceIDx,
            pGATTAttributeWriteInd->attribLength,
            pGATTAttributeWriteInd->data + pGATTAttributeWriteInd->gap );
    }

    return ( true );
}

/**
 * @brief handle blueAPI_Event_GATTCCCDInfo.
 *
 * @param pGATTCCCDInfo      upstream message from upper stack.
 * @return release buffer or not.
 * @retval TRUE 1
 * @retval FALSE 0
*/
bool profileHandle_GATTCCCDInfo(PBlueAPI_GATTCCCDInfo pGATTCCCDInfo )
{
    PGATTDService pService;

    pService = profileServiceFind(pGATTCCCDInfo->serviceHandle );

#ifdef PROFILE_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- profileHandle_GATTCCCDInfo:  count=%d ServiceID=%d",
               2, pGATTCCCDInfo->count, pService->ServiceIDx);
#endif

    if ( (pService != (PGATTDService)NULL))
    {
        int        i;
        uint16_t   wAttribIndex, wCCCBits;
        uint16_t * pWord;

        pWord = (uint16_t *)&pGATTCCCDInfo->data[pGATTCCCDInfo->gap];
        for ( i = 0; i < pGATTCCCDInfo->count; i++ )
        {
            wAttribIndex = *(pWord++);
            wCCCBits     = *(pWord++);
#ifdef PROFILE_DEBUG
            DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "attribIndex=%d, cccBits=0x%04x",
                       2, wAttribIndex, wCCCBits);
#endif
            profileUpdateCCCDInfo((uint8_t)pService->ServiceIDx, wAttribIndex, wCCCBits);

        }
    }

    return ( true );
}

/**
 * @brief handle blueAPI_Event_DisconnectMDLInd.
 *
 * @param pDisconnectMDLInd      upstream message from upper stack.
 * @return release buffer or not.
 * @retval TRUE 1
 * @retval FALSE 0
*/
static bool profileHandle_DisconnectMDLInd(
    PBlueAPI_DisconnectMDLInd pDisconnectMDLInd )
{
#ifdef PROFILE_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,
               "<-- hid profileHandle_DisconnectMDLInd: cause=%d, local_MDL_ID=%d\n",
               2, pDisconnectMDLInd->cause, pDisconnectMDLInd->local_MDL_ID
              );
#endif
    gProfileData.wUpdReqHandle = 0;
    gProfileData.wDsCredits = 0;
    return ( false );
}


static bool profileHandle_GATTAttributePrepareWriteInd(
                        TBlueAPI_Command                 Command,
                        PBlueAPI_GATTAttributeWriteInd pGATTAttributeWriteInd  )
{
    uint16_t       wCause  = GATT_SUCCESS;
    uint8_t *      pBuffer;
    uint16_t       wLength = 0;
    uint16_t       wOffset = gProfileData.wDsDataOffset;
    uint8_t *      pData   = NULL;
    PGATTDService           pService;
    TGATTDWriteIndPostProc  WriteIndPostProc = NULL;
#ifdef PROFILE_DEBUG
     DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,
                     "<--GATTAttributePrepareWriteInd :idx=%d, length=%d valueOffset=%d",3,\
                     pGATTAttributeWriteInd->attribIndex,
                     pGATTAttributeWriteInd->attribLength,
                     pGATTAttributeWriteInd->writeOffset
                     );
#endif
        wLength = pGATTAttributeWriteInd->attribLength;
        pData = pGATTAttributeWriteInd->data + pGATTAttributeWriteInd->gap;

        pService = profileServiceFind(pGATTAttributeWriteInd->serviceHandle );
        if ( pService != (PGATTDService)NULL )
        {
            wCause  = profilePreWriteAttrib(
              pService->ServiceIDx,
              pGATTAttributeWriteInd->handle,   //for execute write response
              pGATTAttributeWriteInd->attribIndex,
              pGATTAttributeWriteInd->attribLength,
              pGATTAttributeWriteInd->data + pGATTAttributeWriteInd->gap,
              pGATTAttributeWriteInd->writeOffset,
              &WriteIndPostProc
          );
        }

    if ( wCause == ProfileResult_Success )
    {
        if ( blueAPI_BufferGet(
                              gProfileData.wDsPoolId,
                              wLength,
                              wOffset,
                              (void **)&pBuffer) == blueAPI_CauseSuccess )
        {
            memcpy( pBuffer+wOffset, pData, wLength );
        }
        else
        {
            wCause = GATT_ERR_OUT_OF_RESOURCE;
        }
    }
    
    if ( wCause != GATT_SUCCESS )
    {
        pBuffer = NULL;
        wLength = 0;
    }   
    
    /* send response */
    if ( blueAPI_GATTAttributePrepareWriteConf(pBuffer,
                                       pGATTAttributeWriteInd->local_MDL_ID,
                                       pGATTAttributeWriteInd->serviceHandle,
                                       (wCause != GATT_SUCCESS) ?
                                           blueAPI_CauseLowerLayerError : blueAPI_CauseSuccess,
                                       wCause,
                                       pGATTAttributeWriteInd->attribIndex,
                                       wLength,
                                       wOffset
                                      )
           )
    {

    }
    else
    {
         DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "!!! illegal parameter (e.g. offset too small)\r\n",
                       1, wCause);

        if ( pBuffer != NULL )
            blueAPI_BufferRelease(pBuffer);
    }
        
    return( true );
}


static bool profileHandle_GATTAttributeExecuteWriteInd(TBlueAPI_Command               Command,
                                          PBlueAPI_GATTAttributeExecuteWriteInd pExecuteWriteInd)
{
    uint16_t  wCause = blueAPI_CauseSuccess;
    uint16_t        handle = 0;
#ifdef PROFILE_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- ExecuteWriteInd: MDL=0x%04x flags=%d", 2,\
                            pExecuteWriteInd->local_MDL_ID,
                            pExecuteWriteInd->flags
                            );
#endif    
    wCause = profileExeWriteAttrib(pExecuteWriteInd->local_MDL_ID, pExecuteWriteInd->flags, &handle);
    
    if(wCause == ProfileResult_AppErr_Have_Confirm)
    {
#ifdef PROFILE_DEBUG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "--> ProfileAPI_AttribExecuteWriteConf: App have confirmed:0x%x, handle:0x%x", 2, \
                    wCause, handle);
#endif
        return true;
    }
    else
    {
        blueAPI_GATTAttributeExecuteWriteConf(pExecuteWriteInd->local_MDL_ID,
                                        (wCause != GATT_SUCCESS) ?
                                        blueAPI_CauseLowerLayerError : blueAPI_CauseSuccess,
                                        wCause,
                                        handle);
#ifdef PROFILE_DEBUG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "--> ProfileAPI_AttribExecuteWriteConf OK, 0x%x, handle:0x%x", 2\
                    , wCause, handle);
#endif
    }
    
    return true;
}



#ifdef ANCS_SUPPORT

static uint16_t    disc_start_handle = 0;
static uint16_t    disc_end_handle = 0;


uint16_t    ancs_cp_value_handle = 0;
uint16_t    ancs_ns_value_handle = 0;
uint16_t    ancs_ds_value_handle = 0;
uint16_t    ancs_write_handle = 0;
bool        ancs_write_notify_value = 0;
extern pfnAPPHandleAncsCB_t pfnAncsCB;

bool ancs_enableNotification(uint16_t cccHandle, uint16_t cccValue)
{
    void       * pBuffer;        
    uint16_t wLength = sizeof(cccValue);
    uint16_t     wOffset = gProfileData.wDsDataOffset + 3;

    if(ancs_write_handle != 0)
    {
        return false;
    }
    
    if ( blueAPI_BufferGet(gProfileData.wDsPoolId, wLength,wOffset,(void **)&pBuffer) == blueAPI_CauseSuccess )
    {
        TBlueAPI_GATTWriteType   writeType = blueAPI_GATTWriteTypeRequest;
        
        memcpy( ((uint8_t *)pBuffer) + wOffset, &cccValue, wLength );

        if (blueAPI_GATTAttributeWriteReq(
                                                                    pBuffer,
                                                                    gProfileData.local_MDL_ID,
                                                                    writeType,
                                                                    cccHandle,
                                                                    wLength,
                                                                    wOffset)
                                                                    )
        {
            ancs_write_handle = cccHandle;
            ancs_write_notify_value = cccValue;
            return true;
        }
        else
        {
            blueAPI_BufferRelease(pBuffer);
        }
    }

    return false;
}



/**
  * @brief  handle blueAPI_Event_GATTDiscoveryRsp.
  * @param  pGATTDiscoveryRsp: message from upper stack.
  * @retval TRUE: need to release buffer.
  *         FALSE: no need to release buffer.
  */
static bool profileHandle_GATTDiscoveryRsp(PBlueAPI_GATTDiscoveryRsp pGATTDiscoveryRsp)
{
#ifdef ANCS_DEBUG
    DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"profileHandle_GATTDiscoveryRsp: subCause=0x%x",1,pGATTDiscoveryRsp->subCause);
#endif
    return( true );
}

/**
  * @brief  handle blueAPI_Event_GATTDiscoveryInd.
  * @param  pGATTDiscoveryInd: message from upper stack.
  * @retval TRUE: need to release buffer.
  *         FALSE: no need to release buffer.
  */
static bool profileHandle_GATTDiscoveryInd(PBlueAPI_GATTDiscoveryInd pGATTDiscoveryInd)
{
    int        i;
    uint16_t   wEndingHandle = 0;   /* used for continuation search */
    bool dis_complete = false;
 

#ifdef ANCS_DEBUG
    DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"profileHandle_GATTDiscoveryInd: Cause=0x%x, subCause=0x%x, type=%d, count=%d",
            4,pGATTDiscoveryInd->cause, pGATTDiscoveryInd->subCause,pGATTDiscoveryInd->discoveryType,pGATTDiscoveryInd->elementCount);
#endif

    if ( pGATTDiscoveryInd->subCause == GATT_SUCCESS )
    {
        uint32_t dwUUID128[4];

        switch( pGATTDiscoveryInd->discoveryType )
        {
            case blueAPI_GATTDiscoveryServiceByUUID:
                /* discover primary services by UUID */
                {
                    /* 2 handles */
                    PBlueAPI_GATTServiceByUUIDElement  pElementUUID;


                    pElementUUID = (PBlueAPI_GATTServiceByUUIDElement)(pGATTDiscoveryInd->list
                                                                          + pGATTDiscoveryInd->gap);

                    for ( i=0; i<pGATTDiscoveryInd->elementCount; i++ )
                    {
                        /* XXXX some output may be lost due to limited buffer pool ... */
#ifdef ANCS_DEBUG
                        DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO," range: 0x%4.4x-0x%4.4x",
                                    2,pElementUUID->attHandle,pElementUUID->endGroupHandle);
#endif
                        blueAPI_GATTDiscoveryReq(pGATTDiscoveryInd->local_MDL_ID, blueAPI_GATTDiscoveryCharacteristics, pElementUUID->attHandle, pElementUUID->endGroupHandle, 0, NULL);

                        wEndingHandle = pElementUUID->endGroupHandle;
                        pElementUUID++;

 
                    }
                }
                break;

            case blueAPI_GATTDiscoveryCharacteristics:
                /* discover all characteristics of a service */
                if ( pGATTDiscoveryInd->elementLength == sizeof(TBlueAPI_GATTCharacteristicsElement16) )
                {
                    /* declaration handle, properties, value handle + 16 bit UUID */
                    PBlueAPI_GATTCharacteristicsElement16   pElement16;


                    pElement16 = (PBlueAPI_GATTCharacteristicsElement16)(pGATTDiscoveryInd->list
                                                                          + pGATTDiscoveryInd->gap);
                    
                    for ( i=0; i<pGATTDiscoveryInd->elementCount; i++ )
                    {
                        /* XXXX some output may be lost due to limited buffer pool ... */
#ifdef ANCS_DEBUG
                        DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO," decl hndl=0x%4.4x, prop=0x%2.2x, value hndl=0x%4.4x, UUID=<0x%4.4x>",
                                    4,pElement16->declHandle,
                                      pElement16->properties,
                                      pElement16->valueHandle,
                                      pElement16->UUID16);
#endif
                        wEndingHandle = pElement16->valueHandle;
                    

                        /* Pointer to the next element found. */
                        pElement16++;
                    }
                }
                else
                {
                    /* declaration handle, properties, value handle + 128 bit UUID */
                    PBlueAPI_GATTCharacteristicsElement128  pElement128;

                    uint8_t uuid128_ControlPoint[16] =           {0xd9, 0xd9, 0xaa, 0xfd, 0xbd, 0x9b, 0x21, 0x98, 0xa8, 0x49, 0xe1, 0x45, 0xf3, 0xd8, 0xd1, 0x69};
                    uint8_t uuid128_NotificationSource[16] = {0xBD, 0x1D, 0xA2, 0x99, 0xE6, 0x25, 0x58, 0x8C, 0xD9, 0x42, 0x01, 0x63, 0x0D, 0x12, 0xBF, 0x9F};
                    uint8_t uuid128_DataSource[16] =            {0xFB, 0x7B, 0x7C, 0xCE, 0x6A, 0xB3, 0x44, 0xBE, 0xB5, 0x4B, 0xD6, 0x24, 0xE9, 0xC6, 0xEA, 0x22};              

                    pElement128 = (PBlueAPI_GATTCharacteristicsElement128)(pGATTDiscoveryInd->list
                                                                            + pGATTDiscoveryInd->gap);
                    for ( i=0; i<pGATTDiscoveryInd->elementCount; i++ )
                    {
                        /* XXXX some output may be lost due to limited buffer pool ... */
                        memcpy( dwUUID128, &pElement128->UUID128[0], sizeof(dwUUID128));
#ifdef ANCS_DEBUG
                        DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO," decl hndl=0x%4.4x, prop=0x%2.2x, value hndl=0x%4.4x, UUID=<0x%8.8x%8.8x%8.8x%8.8x>",
                                    7,pElement128->declHandle,
                                       pElement128->properties,
                                       pElement128->valueHandle,
                                       dwUUID128[3], dwUUID128[2], dwUUID128[1], dwUUID128[0]);
#endif          
                        wEndingHandle = pElement128->valueHandle;
                        if(0 == memcmp(uuid128_ControlPoint, pElement128->UUID128, 16))
                        {
                            ancs_cp_value_handle = pElement128->valueHandle;
                        }
                        else
                        if(0 == memcmp(uuid128_NotificationSource, pElement128->UUID128, 16))
                        {
                            ancs_ns_value_handle = pElement128->valueHandle;
                        }
                        else
                        if(0 == memcmp(uuid128_DataSource, pElement128->UUID128, 16))
                        {
                            ancs_ds_value_handle = pElement128->valueHandle;
                        }                        
                        /* Pointer to the next element found. */
                        pElement128++;
                    }
                    
#ifdef ANCS_DEBUG
                        DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO," ancs_cp_value_handle =0x%4.4x, ancs_ns_value_handle =0x%4.4x, ancs_ds_value_handle =0x%4.4x",
                                    3,ancs_cp_value_handle,
                                       ancs_ns_value_handle,
                                       ancs_ds_value_handle);
#endif              
                    dis_complete = true;
                }
                break;

            case blueAPI_GATTDiscoveryCharacDescriptors:
                /* discovery of characteristic descriptors of a service */
                /* (or list of UUIDs ..)  */
                if ( pGATTDiscoveryInd->elementLength == sizeof(TBlueAPI_GATTCharacDescriptorsElement16) )
                {
                    /* handle, properties, 16 bit UUID */
                    PBlueAPI_GATTCharacDescriptorsElement16   pElement16;


                    pElement16 = (PBlueAPI_GATTCharacDescriptorsElement16)(pGATTDiscoveryInd->list
                                                                            + pGATTDiscoveryInd->gap);

                    for ( i=0; i<pGATTDiscoveryInd->elementCount; i++ )
                    {
                        /* XXXX some output may be lost due to limited buffer pool ... */
#ifdef ANCS_DEBUG
                        DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO," handle=0x%4.4x, UUID=<0x%4.4x>",
                                    2,pElement16->handle,pElement16->UUID16);
#endif
                        wEndingHandle = pElement16->handle;

                     
                        /* Pointer to the next element found. */
                        pElement16++;
                    }
                }
                else
                {
                    /* handle, 128 bit UUID */
                    PBlueAPI_GATTCharacDescriptorsElement128  pElement128;


                    pElement128 = (PBlueAPI_GATTCharacDescriptorsElement128)(pGATTDiscoveryInd->list
                                                                              + pGATTDiscoveryInd->gap);

                    for ( i=0; i<pGATTDiscoveryInd->elementCount; i++ )
                    {
                        /* XXXX some output may be lost due to limited buffer pool ... */
                        memcpy( dwUUID128, &pElement128->UUID128[0], sizeof(dwUUID128));
#ifdef ANCS_DEBUG
                        DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO," handle=0x%4.4x, UUID=<0x%8.8x%8.8x%8.8x%8.8x>",
                                    5,pElement128->handle,
                                      dwUUID128[3], dwUUID128[2], dwUUID128[1], dwUUID128[0]);
#endif
                        wEndingHandle = pElement128->handle;

                    

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

        }
    }
    else
    {
        /* no more results found */
        disc_start_handle = 0;
        disc_end_handle = 0;     /* discovery complete */

    }

    #if 1
    /* do continue discovery here. */
    if ( blueAPI_GATTDiscoveryConf(pGATTDiscoveryInd->local_MDL_ID,
                            pGATTDiscoveryInd->discoveryType,
                            disc_start_handle,  /* starting handle: continue search */
                            disc_end_handle
                           )
     )
    {
#ifdef ANCS_DEBUG
        DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"GATTDiscoveryConf: start=0x%4.4x, end=0x%4.4x",
                    2,disc_start_handle,disc_end_handle);
#endif
    }
    #endif
    if(dis_complete)
    {        
        pfnAncsCB(ANCS_DISCOVERY_COMPLETE, NULL, 0);
    }
    /* if a discover procedure complete, switch disc state and notify the state to upper app. */
  

    return( true );
}


/**
  * @brief  handle blueAPI_Event_GATTAttributeWriteRsp.
  * @param  pGATTAttributeWriteRsp: message from upper stack.
  * @retval TRUE: need to release buffer.
  *         FALSE: no need to release buffer.
  */
static bool profileHandle_GATTAttribWriteRsp(PBlueAPI_GATTAttributeWriteRsp pGATTAttributeWriteRsp)
{
#ifdef ANCS_DEBUG
    DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"profileHandle_GATTAttribWriteRsp: subCause=0x%x",1,pGATTAttributeWriteRsp->subCause);
#endif
    if(pGATTAttributeWriteRsp->cause == blueAPI_CauseSuccess)
    {
        if(ancs_write_handle == ancs_ns_value_handle+1)
        {
            ancs_write_handle = 0;
            if(ancs_write_notify_value == 0)
            {
                pfnAncsCB(ANCS_NOTIFICATION_SOURCE_NOTIFY_DISABLE, NULL, 0);
            }
            else
            {
                pfnAncsCB(ANCS_NOTIFICATION_SOURCE_NOTIFY_ENABLE, NULL, 0);
            }
            return( true );
        }
        else if(ancs_write_handle == ancs_ds_value_handle+1)
        {
            ancs_write_handle = 0;
            if(ancs_write_notify_value == 0)
            {
                pfnAncsCB(ANCS_DATA_SOURCE_NOTIFY_DISABLE, NULL, 0);
            }
            else
            {
                pfnAncsCB(ANCS_DATA_SOURCE_NOTIFY_ENABLE, NULL, 0);
            }
            return( true );
        }
    }
    ancs_write_handle = 0;
   
    return( true );
}

/**
  * @brief  handle blueAPI_EventGATTAttributeInd and blueAPI_EventGATTAttributeNotificationInfo.
  * @param  Command: write flag, write request or write command.
  * @param  pGATTAttributeInd: message from upper stack.
  * @retval TRUE: need to release buffer.
  *         FALSE: no need to release buffer.
  */
static bool profileHandle_GATTAttribNotifInd(uint16_t Command,
                                      PBlueAPI_GATTAttributeInd pGATTAttributeInd)
{
#ifdef ANCS_DEBUG
    DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"profileHandle_GATTAttribNotifInd: handle=0x%x",
            1,pGATTAttributeInd->attribHandle);
#endif
    if(pGATTAttributeInd->attribHandle == ancs_ns_value_handle)
    {
         pfnAncsCB(ANCS_FROM_NOTIFICATION_SOURCE, pGATTAttributeInd->data + pGATTAttributeInd->gap, pGATTAttributeInd->attribLength);
    }
    else
    if(pGATTAttributeInd->attribHandle == ancs_ds_value_handle)        
    {
         pfnAncsCB(ANCS_FROM_DATA_SOURCE, pGATTAttributeInd->data + pGATTAttributeInd->gap, pGATTAttributeInd->attribLength);
    }

    return( true );
}
#endif


/**
 * @brief handle messages from upper stack.
 *
 * @param pMsg       upstream message from upper stack.
 * @return release buffer or not.
 * @retval TRUE 1
 * @retval FALSE 0
*/
bool profile_HandleMessage(PBlueAPI_UsMessage pMsg )
{
    bool      ReleaseBuffer = false;
    switch ( pMsg->Command )
    {
    default:
#ifdef PROFILE_DEBUG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "profile_HandleMessage: unhandled 0x%x",
                   1,
                   pMsg->Command);
#endif
        ReleaseBuffer = false;
        break;
    case blueAPI_EventDisconnectMDLInd:
        profileHandle_DisconnectMDLInd(&pMsg->p.DisconnectMDLInd );
        ReleaseBuffer = false;
        break;
    case blueAPI_EventGATTServiceRegisterRsp:
        ReleaseBuffer = profileHandle_GATTServiceRegisterRsp(
                            &pMsg->p.GATTServiceRegisterRsp );
        break;

    case blueAPI_EventGATTAttributeUpdateRsp:
        ReleaseBuffer = profileHandle_GATTAttributeUpdateRsp(
                            &pMsg->p.GATTAttributeUpdateRsp );
        break;

    case blueAPI_EventGATTAttributeUpdateStatusInd:
        ReleaseBuffer = profileHandle_GATTAttributeUpdateStatusInd(
                            &pMsg->p.GATTAttributeUpdateStatusInd );
        break;

    case blueAPI_EventGATTAttributeReadInd:
        ReleaseBuffer = profileHandle_GATTAttributeReadInd(
                            &pMsg->p.GATTAttributeReadInd );
        break;

    case blueAPI_EventGATTAttributeWriteInd:
    case blueAPI_EventGATTAttributeWriteCommandInfo:
        ReleaseBuffer = profileHandle_GATTAttributeWriteIndCommandInfo(
                            (TBlueAPI_Command)pMsg->Command,
                            &pMsg->p.GATTAttributeWriteInd );
        break;

    case blueAPI_EventGATTCCCDInfo:
        ReleaseBuffer = profileHandle_GATTCCCDInfo(
                            &pMsg->p.GATTCCCDInfo );
        break;

    case blueAPI_EventGATTAttributePrepareWriteInd:
        profileHandle_GATTAttributePrepareWriteInd(
                                         (TBlueAPI_Command)pMsg->Command,
                                         &pMsg->p.GATTAttributeWriteInd );
        break;
    case blueAPI_EventGATTAttributeExecuteWriteInd:
        profileHandle_GATTAttributeExecuteWriteInd((TBlueAPI_Command)pMsg->Command,&pMsg->p.GATTAttributeExecuteWriteInd);
    break;


#ifdef ANCS_SUPPORT
        case blueAPI_EventGATTDiscoveryRsp:
            ReleaseBuffer = profileHandle_GATTDiscoveryRsp( &pMsg->p.GATTDiscoveryRsp );
            break;

        case blueAPI_EventGATTDiscoveryInd:
            ReleaseBuffer = profileHandle_GATTDiscoveryInd( &pMsg->p.GATTDiscoveryInd );
            break;

        case blueAPI_EventGATTAttributeWriteRsp:
            ReleaseBuffer = profileHandle_GATTAttribWriteRsp( &pMsg->p.GATTAttributeWriteRsp );
            break;

 
        case blueAPI_EventGATTAttributeNotificationInfo:
            ReleaseBuffer = profileHandle_GATTAttribNotifInd(pMsg->Command,
                                           &pMsg->p.GATTAttributeInd );
            break;
#endif
		}
		
			
    if (ReleaseBuffer)
        blueAPI_BufferRelease(pMsg);

    return ReleaseBuffer;
}

