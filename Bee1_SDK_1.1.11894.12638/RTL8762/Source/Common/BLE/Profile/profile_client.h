/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     profile_client.h
  * @brief    Head file for profile client structure.
  * @details  Common data struct definition.
  * @author   ethan_su
  * @date     2015-07-15
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _PROFILE_CLIENT_H_
#define _PROFILE_CLIENT_H_

#ifdef  __cplusplus
	extern "C" {
#endif      /* __cplusplus */

/** Add Includes here **/
#include "blueapi_types.h"
#include "profileApi.h"

/** @addtogroup RTK_Profile_Module RTK Profile Module
  * @{
  */

/** @defgroup GATT_Client_Common GATT Client Common
  * @brief Common Part of Client
  * @{
  */

/** @defgroup Client_Common_Exported_Constants Client Common Exported Constants
  * @brief macros that other .c files may use all defined here
  * @{
  */

/** @defgroup Client_ID_App Client ID App
  * @brief	Client ID used by application, when directly calls the APIs of profile client layer.
  * @{
  */
#define AppProcessGeneralClientMsgID   0x00        //!< Client ID used by application, distinguish with other specific client module.
/** End of Client_ID_App * @} */

/** End of Client_Common_Exported_Constants
* @}
*/

/** @defgroup Client_Common_Exported_Types Client Common Exported Types
  * @{
  */

/** @defgroup TClientID TClientID
  * @{ 
  */
typedef uint8_t TClientID;
/** End of TClientID * @} */

/** @defgroup TUuidType TUuidType
  * @{ 
  */
typedef enum
{
    UUID_TYPE_16,
    UUID_TYPE_128,
    UUID_TYPE_UNUSED
}TUuidType;
/** End of TUuidType * @} */

/** @defgroup TUuidData TUuidData
  * @{ 
  */
typedef union
{
    uint16_t    UUID16;
    uint8_t     UUID128[16]; 
}TUuidData;
/** End of TUuidData
  * @}
  */

/** @defgroup TUuid TUuid
  * @{ 
  */
typedef struct
{
    TUuidType   UUIDType;
    TUuidData   UUIDData;
}TUuid;
/** End of TUuid * @} */

/** @defgroup TSrvDiscReq TSrvDiscReq
  * @{ Request Data of discovering service by UUID 
  */
typedef struct
{
    TUuid  uuidValue;                    /**< UUID of the service to be discovered */
} TSrvDiscReq, * PSrvDiscReq;
/** End of TSrvDiscReq * @} */

/** @defgroup TRelationDiscReq TRelationDiscReq
  * @{ Request Data of discovering included service
  */
typedef struct
{
    uint16_t  wStartHandle;
    uint16_t  wEndHandle;
} TRelationDiscReq, * PRelationDiscReq;
/** End of TRelationDiscReq * @} */

/** @defgroup TCharDiscReq TCharDiscReq
  * @{ Request Data of discovering characteristic
  */
typedef struct
{
    uint16_t  wStartHandle;
    uint16_t  wEndHandle;
} TCharDiscReq, * PCharDiscReq;
/** End of TCharDiscReq * @} */

/** @defgroup TCharDescriptorDiscReq TCharDescriptorDiscReq
  * @{ Request Data of discovering characteristic descriptor
  */
typedef struct
{
    uint16_t  wStartHandle;
    uint16_t  wEndHandle;
} TCharDescriptorDiscReq, * PCharDescriptorDiscReq;
/** End of TCharDescriptorDiscReq * @} */

/* Client Read procedure related typedefs. */

/** @defgroup TReadHandleReq TReadHandleReq
  * @{ Read attribute by using handle
  */
typedef struct
{
    uint16_t  wHandle;                  /**< Handle of the attribute to be read */
} TReadHandleReq, * PReadHandleReq;
/** End of TReadHandleReq * @} */

/** @defgroup TReadUUIDReq TReadUUIDReq
  * @{ Read attribute by using 16 bit UUID
  */
typedef struct
{
    uint16_t  UUID16;               /**< UUID of the attribute to be read */
    uint16_t  wStartHandle;         /**< starting handle of service the attribute belongs to */
    uint16_t  wEndHandle;           /**< ending handle of service the attribute belongs to */
} TReadUUIDReq, * PReadUUIDReq;
/** End of TReadUUIDReq * @} */

/** @defgroup TReadUUID128Req TReadUUID128Req
  * @{ Read attribute by using 128 bit UUID
  */
typedef struct
{
    uint8_t  UUID128[16];           /**< UUID of the attribute to be read */
    uint16_t  wStartHandle;         /**< starting handle of service the attribute belongs to */
    uint16_t  wEndHandle;           /**< ending handle of service the attribute belongs to */
} TReadUUID128Req, * PReadUUID128Req;
/** End of TReadUUID128Req * @} */

/** @defgroup TWriteReq TWriteReq
  * @{ Write attribute request data
  */
typedef struct
{
    uint16_t  wHandle;                  /**< Handle of the attribute to be written */
    TBlueAPI_GATTWriteType  wType;      /**< Write type, command or request */
    uint16_t  wLength;                  /**< Length of data to be written */
    uint8_t  *pData;                    /**< Data to be written */
} TWriteReq, * PWriteReq;
/** End of TWriteReq * @} */

/** @defgroup TClientRequestResult TClientRequestResult
  * @{ Discovery/Write/Read request result, success or fail
  */
typedef enum
{
    CLIENT_REQ_SUCCESS,
    CLIENT_REQ_FAIL
}TClientRequestResult, *PClientRequestResult;
/** End of TClientRequestResult * @} */

/* callback related data to inform application. */

/** @defgroup TDiscoveryState TDiscoveryState
  * @{ Discovery state during discovery procedure
  */
typedef enum
{
    DISC_STATE_SRV,
    DISC_STATE_SRV_DONE,
    DISC_STATE_RELATION,
    DISC_STATE_RELATION_DONE,
    DISC_STATE_CHAR,
    DISC_STATE_CHAR_DONE,
    DISC_STATE_CHAR_DESCRIPTOR,
    DISC_STATE_CHAR_DESCRIPTOR_DONE,
    DISC_STATE_IDLE
} TDiscoveryState, * PDiscoveryState;
/** End of TDiscoveryState * @} */

/** @defgroup TDiscStateCB_Data TDiscStateCB_Data
  * @{ Discovery procedure related data to inform application
  */
typedef struct
{
    TDiscoveryState discState;
}TDiscStateCB_Data, *PDiscStateCB_Data;
/** End of TDiscStateCB_Data * @} */

/** @defgroup TDiscoverResultType TDiscoverResultType
  * @{ Discovery result type
  */
typedef enum
{
    DISC_RESULT_ALL_SRV_UUID16,
    DISC_RESULT_ALL_SRV_UUID128,
    DISC_RESULT_SRV_DATA,
    DISC_RESULT_CHAR_UUID16,
    DISC_RESULT_CHAR_UUID128,
    DISC_RESULT_CHAR_DESC_UUID16,
    DISC_RESULT_CHAR_DESC_UUID128,
    DISC_RESULT_RELATION_UUID16,
    DISC_RESULT_RELATION_UUID128
} TDiscoverResultType, * PDiscoverResultType;
/** End of TDiscoverResultType * @} */

/** @defgroup TDiscoverResultData TDiscoverResultData
  * @{ Discovery result data
  */
typedef union
{
    PBlueAPI_GATTServicesElement16            pSrvUUID16DiscData;
    PBlueAPI_GATTServicesElement128           pSrvUUID128DiscData;
    PBlueAPI_GATTServiceByUUIDElement         pSrvDiscData;
    PBlueAPI_GATTCharacteristicsElement16     pCharUUID16DiscData;
    PBlueAPI_GATTCharacteristicsElement128    pCharUUID128DiscData;
    PBlueAPI_GATTCharacDescriptorsElement16   pCharDescUUID16DiscData;
    PBlueAPI_GATTCharacDescriptorsElement128  pCharDescUUID128DiscData;
    PBlueAPI_GATTRelationshipElement16        pRelationUUID16DiscData;
    PBlueAPI_GATTRelationshipElement128       pRelationUUID128DiscData;
} TDiscoverResultData, * PDiscoverResultData;
/** End of TDiscoverResultData * @} */

/** @defgroup TDiscResultCB_Data TDiscResultCB_Data
  * @{ Discovery result data will be sent to upper through the callback
  */
typedef struct
{
    TDiscoverResultType resultType;
    TDiscoverResultData resultData;
}TDiscResultCB_Data, *PDiscResultCB_Data;
/** End of TDiscResultCB_Data * @} */

/** @defgroup TReadResultCB_Data TReadResultCB_Data
  * @{ Read result data will be sent to upper through the callback
  */
typedef struct
{
    TClientRequestResult    result;
    uint16_t    wHandle;
    int         iValueSize;
    uint8_t     *pValue;
}TReadResultCB_Data, *PReadResultCB_Data;
/** End of TReadResultCB_Data * @} */

/** @defgroup TWriteResultCB_Data TWriteResultCB_Data
  * @{ Write result will be sent to upper through the callback
  */
typedef struct
{
    TClientRequestResult    result;
}TWriteResultCB_Data, *PWriteResultCB_Data;
/** End of TWriteResultCB_Data * @} */

/** @defgroup TNotifIndResultCB_Data TNotifIndResultCB_Data
  * @{ Notification/Indication data received will be sent to upper through the callback
  */
typedef struct
{
    uint16_t    wHandle;
    int         iValueSize;
    uint8_t     *pValue;
}TNotifIndResultCB_Data, *PNotifIndResultCB_Data;
/** End of TNotifIndResultCB_Data * @} */

/** @defgroup TClientAppCB_Type TClientAppCB_Type
  * @{ Message data type, when data sent to app directly
  */
typedef enum
{
    CLIENT_APP_CB_TYPE_DISC_STATE,
    CLIENT_APP_CB_TYPE_DISC_RESULT,
    CLIENT_APP_CB_TYPE_READ_RESULT,
    CLIENT_APP_CB_TYPE_WRITE_RESULT,
    CLIENT_APP_CB_TYPE_NOTIF_IND_RESULT
}TClientAppCB_Type, *PClientAppCB_Type;
/** End of TClientAppCB_Type * @} */

/** @defgroup TClientAppCB_Content TClientAppCB_Content
  * @{ Client received data from server, when no specific client registered, will be sent to app directly
  */
typedef union
{
    TDiscStateCB_Data   discStateCB_Data;
    TDiscResultCB_Data  discResultCB_Data;
    TReadResultCB_Data  readResultCB_Data;
    TWriteResultCB_Data  writeResultCB_Data;
    TNotifIndResultCB_Data  notifIndResultCB_Data;
}TClientAppCB_Content, * PClientAppCB_Content;
/** End of TClientAppCB_Content * @} */

/** @defgroup TClientAppCB_Data TClientAppCB_Data
  * @{ Callback data sent to application directly from client, include type and content
  */
typedef struct
{
    TClientAppCB_Type     cb_type;
    TClientAppCB_Content    cb_content;
} TClientAppCB_Data;
/** End of TClientAppCB_Data
  * @}
  */

/** @defgroup pfnSpecificClientAppCB_t pfnSpecificClientAppCB_t
  * @{ function ponter used in each specific profile, to send events to application
  */
typedef TAppResult (*pfnSpecificClientAppCB_t)(TClientID clientID, void *pData);
/** End of pfnSpecificClientAppCB_t
  * @}
  */

///@cond
/** @brief Specific client related callback function type definition. */
typedef TProfileResult (*pfnDiscoverStateCB_t)( TDiscoveryState discoveryState );
typedef TProfileResult (*pfnDiscoverResultCB_t)( TDiscoverResultType resultType, TDiscoverResultData resultData );
typedef TProfileResult (*pfnReadResultCB_t)( TClientRequestResult reqResult, uint16_t wHandle, int iValueSize, uint8_t *pValue );
typedef TProfileResult (*pfnWriteResultCB_t)( TClientRequestResult reqResult );
typedef TProfileResult (*pfnNotifIndResultCB_t)( uint16_t wHandle, int iValueSize, uint8_t *pValue );
typedef TProfileResult (*pfnDisconnectCB_t)( void );
///@endcond

/** @defgroup TGATTClientCBs TGATTClientCBs
  * @{ Client related callback functions struct
  */
typedef struct
{
    pfnDiscoverStateCB_t pfnDiscoverStateCB;        //!< Discovery state callback function pointer
    pfnDiscoverResultCB_t pfnDiscoverResultCB;      //!< Discovery reault callback function pointer
    pfnReadResultCB_t pfnReadResultCB;              //!< Read response callback function pointer
    pfnWriteResultCB_t pfnWriteResultCB;            //!< Write result callback function pointer
    pfnNotifIndResultCB_t pfnNotifIndResultCB; 		//!< Notify Indication callback function pointer
    pfnDisconnectCB_t pfnDisconnectCB;              //!< Disconnection callback function pointer
} TGATTClientCBs;
/** End of TGATTClientCBs * @} */

/** End of Client_Common_Exported_Types
* @}
*/

/** @defgroup Client_Common_Exported_Functions Client Common Exported Functions
  * @{
  */
extern void clientAPI_RegisterAppCB( pfnSpecificClientAppCB_t pFunc );
extern bool clientAPI_RegisterSpecClientCB( TClientID *outClientId, uint16_t *pSrvHandle, CONST TGATTClientCBs *pClientCBs );
extern TProfileResult clientAPI_AllPrimarySrvDiscovery( void );
extern TProfileResult clientAPI_ByUuidSrvDiscovery( TClientID clientID, uint16_t wUUID16 );
extern TProfileResult clientAPI_ByUuid128SrvDiscovery( TClientID clientID, uint8_t *pUUID128 );
extern TProfileResult clientAPI_RelationshipDiscovery( TClientID clientID, TRelationDiscReq relationReq );
extern TProfileResult clientAPI_AllCharDiscovery( TClientID clientID, TCharDiscReq charReq );
extern TProfileResult clientAPI_AllCharDescriptorDiscovery( TClientID clientID, TCharDescriptorDiscReq charDescriptorReq );
extern TProfileResult clientAPI_AttribRead( TClientID clientID, TReadHandleReq readHandle );
extern TProfileResult clientAPI_AttribReadUsingUUID( TClientID clientID, TReadUUIDReq readUUID );
extern TProfileResult clientAPI_AttribReadUsingUUID128( TClientID clientID, TReadUUID128Req readUUID128Req );
extern TProfileResult clientAPI_AttribWrite( TClientID clientID, PWriteReq pWriteReq );
extern bool clientAPI_AttribIndConf( uint16_t local_MDL_id );
extern TProfileResult  clientAPI_GetProfileResult(TAppResult appResult);
/** End of Client_Common_Exported_Functions
* @}
*/

/** End of GATT_Client_Common
* @}
*/

/** End of RTK_Profile_Module
* @}
*/

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif /* !defined (_PROFILE_CLIENT_H_) */

