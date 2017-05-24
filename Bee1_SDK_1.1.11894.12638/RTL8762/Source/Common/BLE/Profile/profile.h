/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     profile.h
  * @brief    Head file for profile structure.
  * @details  Common data struct definition.
  * @author   Lilly_he
  * @date     2014-06-26
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef __PROFILE_H
#define __PROFILE_H
	
#ifdef  __cplusplus
	extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "rtl_types.h"
#include "endialig.h"
#include <blueapi_types.h>
#include <bterrcod.h>
#include <gatt.h>
#include "profileApi.h"

///@cond
/** @addtogroup RTK_Profile_Module RTK Profile Module
  * @{
  */

/** @addtogroup GATT_Service_Common GATT Service Common
  * @brief Common Part of Service
  * @{
  */

/** @defgroup Service_Common_Exported_Types Service Common Exported Types
  * @brief  types that other.c files may use all defined here
  * @{
  */
  
/* Add all public types here */

/** @brief Profile related data struct */
typedef struct _PROFILE_DATA  /* main structure */
{
    /*-- BlueAPI/Blueface API data --*/
    uint16_t    iServiceCountIndex;
    uint16_t    wUpdReqHandle;   /**< attribute update request handle */
    /* (NOT attribute handle !!!) */
    uint16_t    wMTUSize;        /**< ATT layer MTU size */

    /* connection related data */
    uint16_t    wDsPoolId;       /**< buffer pool id for connection related  */
    /* downstream data  */
    uint16_t    wDsDataOffset;   /**< data offset in downstream data buffer  */
    uint16_t    wDsCredits;      /**< downstream (tx) credits for WriteCommand */
    /* and Notifications */

    uint16_t    local_MDL_ID;
    /* attribute handle to (16 bit) UUID mapping */
    int32_t     iHandleCnt;
} TPROFILE_DATA, * PTPROFILE_DATA;

/** @brief tx buffer state change */
typedef struct
{
    uint8_t bTxBufferAvaiable;
} TPROFILE_TX_BUF_STATE_CHANGE;

/** @brief TBT_PROFILE_MSG_DATA */
typedef union
{
    TPROFILE_TX_BUF_STATE_CHANGE       profileTxBufferStateChange;
} TBT_PROFILE_MSG_DATA;

typedef struct _BT_PROFILE_MSG
{
    TBT_PROFILE_MSG_DATA msgData;
} BT_PROFILE_MSG;
/** @} End of Service_Common_Exported_Types */


extern TPROFILE_DATA gProfileData;


/** @defgroup Service_Common_Exported_Functions Service Common Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */
/* Update MTU size for LE Devices */
void profileUpdateMtu(uint16_t mtu_size);
/* update connection information */
void profileUpdateConnInfo(PBlueAPI_ConnectMDLInfo pConnectMDLInfo);
/* register services to GATT from profile */
bool profileServiceRegister(void);
/* read characteristic data from service */
TProfileResult  profileReadAttrib(uint8_t serviceIdx, uint16_t iAttribIndex,
                            uint16_t iOffset, uint16_t * pwLength, uint8_t **ppValue );
/* write characteristic data from service */
TProfileResult  profileWriteAttrib(uint8_t serviceIdx, uint16_t iAttribIndex,
                             uint16_t wLength, uint8_t * pValue, TGATTDWriteIndPostProc * pWriteIndPostProc);
//prepare write
TProfileResult  profilePreWriteAttrib(uint8_t serviceIdx, uint16_t handle, uint16_t iAttribIndex,
                                   uint16_t wLength, uint8_t * pValue, uint16_t offset, TGATTDWriteIndPostProc * pWriteIndPostProc);
//execute write
TProfileResult profileExeWriteAttrib(uint8_t mdlId, uint8_t flag, uint16_t* handle);                                   

/* handle message from upstream of upper stack */
bool profile_HandleMessage(PBlueAPI_UsMessage pMsg );
/** @} End of Service_Common_Exported_Functions */

/** @} End of GATT_Service_Common */

/** @} End of RTK_Profile_Module */
///@endcond

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif /* !defined (__PROFILE_H) */
