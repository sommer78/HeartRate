/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    gap_common.c
  * @brief   This file provides the GAP role common functions, used by all GAP roles.
  * @details
  * @author  Ethan
  * @date    11-Aug-2015
  * @version v0.1
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
  ******************************************************************************
  */

/** Add Includes here **/
#include <string.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "blueapi.h"
#include "trace.h"
#include "gap.h"
#include "bee_message.h"

extern xQueueHandle hEventQueueHandle;
extern xQueueHandle hMessageQueueHandle;
extern xQueueHandle hIoQueueHandle;
extern TBlueAPIHandle   GAP_BlueAPIHandle;
TBlueAPIAppHandle tAppHandle;

#define CHAR2SHORT(p) (((*(p)) & 0xff) + ((*((p)+1)) << 8))
#define LE_WORD2EXTRN(p,w)                 \
    {*((unsigned char*)p)     = (unsigned char)((unsigned short)(w) & 0xff);      \
        *(((unsigned char*)p)+1) =  (unsigned char)(((unsigned short)(w)>>8) & 0xff);}

/** @cond private**/
/** @addtogroup Gap_Common
  * @{
  */

/** @defgroup Gap_Common_Private_Constants Gap Common Private Constants
  * @{
  */
/** Add all private defines here **/
#define BLUEAPI_MSG_EVENT        0x01
/** End of Gap_Common_Private_Constants
  * @}
  */

/** @defgroup Gap_Common_Private_Variables Gap Commnon Private Variables
  * @{
  */
/** Add all private variables here **/
uint8_t  gapPara_profileRole = GAP_PROFILE_PERIPHERAL;              /* GAP role: Peripheral, Central, Observer, Broadcaster. */
uint16_t gapPara_ConnHandle = 0;                                    /* GAP Connection Handle: after link connected, this handle will be set to local_MDL_ID. */
static uint8_t  gapPara_bdAddr[B_ADDR_LEN];                         /* Local device's BT address. */
static uint8_t  gapPara_bdAddrType = blueAPI_LocalBDTypeLEPublic;   /* Local device's BT address's type. */
static uint8_t  gapPara_DeviceName[GAP_DEVICE_NAME_LEN] = "GAP";    /* Local device's name, hold in GAP Service. */
static uint16_t gapPara_Appearance = GAP_GATT_APPEARANCE_UNKNOWN;   /* Local device's appearence. */
static int8_t   gapPara_btTxPower = 0;                              /* Local device's TX Power. */
int8_t   gapPara_btRSSI = 0;                                        /* Link RSSI value: only used by Peripheral and Central. */
#ifdef BT_GAP_LE_PRIVACY_RESOLVING
uint8_t  gapPara_PrivacyMode = GAP_PRIVACY_MODE_ENABLE;            /* Local device's privacy mode. */
#endif
/* Application callbacks, these callbacks will be used in each role to send msg to application. */
gapRolesParamUpdateCB_t pGapRoles_ParamUpdateCB = NULL;
gapDeviceStateChangeCb pGapDeviceStateChangeCb = NULL;
/** End of Gap_Common_Private_Variables
  * @}
  */

/** End of Gap_Common
  * @}
  */
/** @endcond **/

/**
  * @brief  Initialize parameters of gap GAP role. 
  * @param  gapRole: GAP role.
  * @retval None
  */
void GAP_ParaInit(uint8_t gapRole)
{
    gapPara_profileRole = gapRole;
    gapPara_bdAddrType = blueAPI_LocalBDTypeLEPublic;
    gapPara_Appearance = GAP_GATT_APPEARANCE_UNKNOWN;

#if defined (RTL8762AX_VB)
    memcpy(gapPara_bdAddr, (uint8_t *)0x20000076, 6);    
#endif
}

/**
  * @brief  Set a GAP Role parameter.
  *         NOTE: You can call this function with a GAP Peripheral Parameter ID and it will set the
  *         GAP Parameter.  GAP Peripheral Parameters are defined in (GAP.h).  Also,
  *         the "len" field must be set to the size of a "uint16_t" and the
  *         "pValue" field must point to a "uint16".
  * @param  param - Profile parameter ID: @ref GAP_PERIPHERAL_PARAMETERS
  * @param  len - length of data to write
  * @param  pValue - pointer to data to write.  This is dependent on
  *                  the parameter ID and WILL be cast to the appropriate
  *                  data type (example: data type of uint16 will be cast to
  *                  uint16 pointer).
  * @retval gapAPI_CauseSuccess or INVALIDPARAMETER (invalid paramID)
  */
TGAP_STATUS GAP_SetParameter( uint16_t param, uint8_t len, void *pValue )
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;

    /* Common param set should be added here. */
    switch ( param )
    {
        case GAPPARA_PROFILE_ROLE:
            if (len == sizeof ( uint8_t ))
            {
                gapPara_profileRole = *((uint8_t*)pValue);
            }
            else
            {
                ret = gapAPI_InvalidRange;
            }
            break;

        case GAPPARA_BD_ADDR_TYPE:
            if (len == sizeof( uint8_t ))
            {
                gapPara_bdAddrType = *((uint8_t*)pValue);
            }
            else
            {
                ret = gapAPI_InvalidRange;
            }
            break;

        case GAPPRRA_DEVICE_NAME:
            memset( gapPara_DeviceName, 0, GAP_DEVICE_NAME_LEN);
            memcpy( gapPara_DeviceName, pValue, len);
            break;

        case GAPPRRA_APPEARANCE:
            if ( len == sizeof ( uint16_t ) )
            {
                gapPara_Appearance = *((uint16_t*)pValue);
            }
            else
            {
                ret = gapAPI_InvalidRange;
            }
            break;
#ifdef BT_GAP_LE_PRIVACY_RESOLVING
        case GAPPARA_BLE_PRIVACY_MODE:
            if (len == sizeof( uint8_t ))
            {
                gapPara_PrivacyMode = *((uint8_t*)pValue);
            }
            else
            {
                ret = gapAPI_InvalidRange;
            }
            break;
#endif
        default:
            /* Param will be set in specific roles. */
            break;
    }

    return ( ret );
}

/**
  * @brief  Get a GAP Role parameter.
  *
  *         NOTE: You can call this function with a GAP Peripheral Parameter ID and it will get a
  *         GAP Peripheral Parameter.  GAP Peripheral Parameters are defined in (GAP.h).  Also, the
  *         "pValue" field must point to a "uint16".
  *
  * @param  param - Profile parameter ID: @ref GAP_PERIPHERAL_PARAMETERS
  * @param  pValue - pointer to location to get the value.  This is dependent on
  *                  the parameter ID and WILL be cast to the appropriate
  *                  data type (example: data type of uint16 will be cast to
  *                  uint16 pointer).
  *
  * @return gapAPI_CauseSuccess or INVALIDPARAMETER (invalid paramID)
  */
TGAP_STATUS GAP_GetParameter( uint16_t param, void *pValue )
{
    TGAP_STATUS ret = gapAPI_CauseSuccess;

    /* Common param set should be added here. */
    switch ( param )
    {
        case GAPPARA_PROFILE_ROLE:
            *((uint8_t*)pValue) = gapPara_profileRole;
            break;
        case GAPPARA_BD_ADDR:
            memcpy( pValue, gapPara_bdAddr, B_ADDR_LEN ) ;
            break;
        case GAPPARA_BD_ADDR_TYPE:
            *((uint8_t*)pValue) = gapPara_bdAddrType;
            break;
        case GAPPRRA_APPEARANCE:
            *((uint16_t*)pValue) = gapPara_Appearance;
            break;
        case GAPPARA_BLE_TX_POWER:
            *((int8_t*)pValue) = gapPara_btTxPower;
            break;
#ifdef BT_GAP_LE_PRIVACY_RESOLVING
        case GAPPARA_BLE_PRIVACY_MODE:
            *((uint8_t*)pValue) = gapPara_PrivacyMode;
            break;
#endif
        default:
            /* Param will be got in specific roles. */
            break;
    }

    return ( ret );
}

/**
  * @brief  Callback function should be register to upper stack to send message to application.
  * @param  pMsg - message sent from upper stack.
  * @retval none
  */
static void GAP_BlueAPIMessageCB(PBlueAPI_UsMessage pMsg)
{
    unsigned char Event = BLUEAPI_MSG_EVENT;

    if (xQueueSend(hMessageQueueHandle, &pMsg, 0) == errQUEUE_FULL)
    {
        blueAPI_BufferRelease(pMsg);
    }
    else if (xQueueSend(hEventQueueHandle, &Event, 0) == errQUEUE_FULL)
    {

    }
}

#ifdef BT_GAP_VENDOR_CMD_ADD
/******************************************************************
 * @fn          GAP_SendBtMsgToApp
 * @brief      send BEE_IO_MSG to app task.            
 * @param    pBeeMsgBlk  - pointer to BEE_IO_MSG message
 *
 * @return     void
 */
static void GAP_SendBtMsgToApp(BEE_IO_MSG *pBeeMsgBlk)
{
    portBASE_TYPE SendQueueResult;
    uint8_t Event = 0;

    SendQueueResult = xQueueSend(hIoQueueHandle, pBeeMsgBlk, 0xFFFF);
    if (SendQueueResult != pdPASS)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "GAP_SendBtMsgToApp fail2", 1, SendQueueResult);
    }

    Event = EVENT_NEWIODRIVER_TO_APP;
    SendQueueResult = xQueueSend(hEventQueueHandle, &Event, 0xFFFF);
    if (SendQueueResult != pdPASS)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "GAP_SendBtMsgToApp fail", 1, SendQueueResult);
    }
}
#endif

/**
  * @brief  Make upper stack ready to use, by sending the register request.
  * @retval TRUE - start successful.
  *         FALSE - start failed.
  */
bool GAP_StartBtStack()
{
    bool Result = true;
    if (
        blueAPI_RegisterReq( (void *)&tAppHandle,
                             (void *)GAP_BlueAPIMessageCB)
    )
    {
#ifdef GAP_DEBUG_FLAG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GAP_StartBtStack", 0);
#endif
    }
    else
    {
        Result = false;
    }

    return( Result );
}

/**
  * @brief  process blueAPI_EventRegisterRsp message from bt stack.
  * @param  pRegisterRsp - message sent from upper stack.
  * @retval TRUE - need to release buffer by application.
  *         FALSE - no need to release buffer by application.
  */
static bool GAP_Handle_RegisterRsp(PBlueAPI_RegisterRsp pRegisterRsp )
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GAP_Handle_RegisterRsp: cause=%d", 1, pRegisterRsp->cause);
#endif
    if ( pRegisterRsp->cause == blueAPI_CauseSuccess )
    {
        /* Upper Stack register success */
    }
    else
    {
        /* Upper Stack register fail */
    }

    return( true );
}

/**
  * @brief  process blueAPI_EventActInfo message from bt stack.
  * @param  pActInfo - message sent from upper stack.
  * @retval TRUE - need to release buffer by application.
  *         FALSE - no need to release buffer by application.
  */
static bool GAP_Handle_ActInfo(PBlueAPI_ActInfo pActInfo )
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GAP_Handle_ActInfo: cause=%d", 1, pActInfo->cause);
#endif
    if (pActInfo->cause == blueAPI_CauseSuccess)
    {
        //gapRole_state = GAPSTATE_STACK_READY;
        memcpy(gapPara_bdAddr, pActInfo->local_BD, B_ADDR_LEN );
        DBG_BUFFER(MODULE_PROFILE,LEVEL_INFO,"    My Local BD-- 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x",6,
                    pActInfo->local_BD[5],
                    pActInfo->local_BD[4],
                    pActInfo->local_BD[3],
                    pActInfo->local_BD[2],
                    pActInfo->local_BD[1],
                    pActInfo->local_BD[0]);
        /* first config device name. */
        blueAPI_DeviceConfigDeviceNameSetReq(gapPara_DeviceName);
    }
    return ( true );
}

/**
  * @brief  process blueAPI_EventSetBleTxPowerRsp message from bt stack.
  * @param  pActInfo - message sent from upper stack.
  * @retval TRUE - need to release buffer by application.
  *         FALSE - no need to release buffer by application.
  */
static void GAP_Handle_SetBleTxPowerRsp(PBlueAPI_SetBleTxPowerRsp pSetBleTxPowerRsp)
{
    BOOL respResult = TRUE;
    BEE_IO_MSG bee_io_msg;
    BT_STACK_MSG btGapMsg;
    
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GAP_Handle_SetBleTxPowerRsp: cause=%d tx_power_index = %d subCause=0x%x\r\n",3,
                              pSetBleTxPowerRsp->cause,
                              pSetBleTxPowerRsp->tx_power_index,
                              pSetBleTxPowerRsp->subCause
                              );
#endif

    if(pSetBleTxPowerRsp->cause == blueAPI_CauseSuccess)
    {
        switch(pSetBleTxPowerRsp->tx_power_index)
        {
            case TX_POWER_MINUS_16_DBM:
                gapPara_btTxPower = -16;
                break;
            case TX_POWER_0_DBM:
                gapPara_btTxPower = 0;
                break;
            case TX_POWER_3_DBM:
                gapPara_btTxPower = 3;
                break;
            default:
                respResult = FALSE;
                DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "GAP_Handle_SetBleTxPowerRsp: tx_power_index error!\r\n",0);
                break;
        }
    }
    else
    {
        respResult = FALSE;
    }

    bee_io_msg.IoType = BT_STATUS_UPDATE;
    bee_io_msg.subType = BT_MSG_TYPE_PARAM_SET_RESULT;
    btGapMsg.msgData.gapBTParamSetResult.bleParamType = BLE_PARAM_TYPE_TX_POWER;
    btGapMsg.msgData.gapBTParamSetResult.result = respResult;

    memcpy(&bee_io_msg.parm, &btGapMsg, sizeof(bee_io_msg.parm));

    GAP_SendBtMsgToApp(&bee_io_msg);

}

#ifdef BT_GAP_VENDOR_CMD_ADD
/**
  * @brief  process blueAPI_EventVendorCmdRsp message from bt stack.
  * @param  pActInfo - message sent from upper stack.
  * @retval TRUE - need to release buffer by application.
  *         FALSE - no need to release buffer by application.
  */
static void GAP_Handle_VendorCmdRsp(PBlueAPI_UsCommandData pVendorCmdRsp)
{
    BOOL respResult = TRUE;
    BEE_IO_MSG bee_io_msg;
    BT_STACK_MSG btGapMsg;
    uint8_t vendorCmdData[40] = {0};
    uint16_t vendorCmdOpCode;
    uint8_t vendorCmdStatus;
    //uint16_t connHandle;

    /* Set the IO msg type for BT events. */
    bee_io_msg.IoType = BT_STATUS_UPDATE;
    bee_io_msg.subType = BT_MSG_TYPE_PARAM_SET_RESULT;

    memcpy(vendorCmdData, pVendorCmdRsp, 40);
    vendorCmdOpCode = CHAR2SHORT(vendorCmdData + 1);
    vendorCmdStatus = vendorCmdData[3];
    
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GAP_Handle_VendorCmdRsp: OpCode=%d status = %d\r\n",2,
                              vendorCmdOpCode,
                              vendorCmdStatus );
#endif

    
    switch(vendorCmdOpCode)
    {
        case HCI_READ_RSSI:
            btGapMsg.msgData.gapBTParamSetResult.bleParamType = BLE_PARAM_TYPE_RSSI;
            //connHandle = CHAR2SHORT(vendorCmdData + 4);
            /* Ethan: to do. */
            //if(connHandle == gapPara_ConnHandle)
            {
                gapPara_btRSSI = (int8_t)vendorCmdData[6];
            }
            //else
            {
            //    respResult = FALSE;
            //    DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "GAP_Handle_VendorCmdRsp: local mdl id error!\r\n",0);
            }
            break;
        case HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION_OPCODE:
            btGapMsg.msgData.gapBTParamSetResult.bleParamType = BLE_PARAM_TYPE_SET_CHANNEL_CLASS;
            break;
        case HCI_VENDOR_ONOFF_SLAVE_LATENCY:
            btGapMsg.msgData.gapBTParamSetResult.bleParamType = BLE_PARAM_TYPE_ONOFF_SLAVE_LATENCY;
            break;
        default:
            respResult = FALSE;
            DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "GAP_Handle_VendorCmdRsp: opcode error!\r\n",0);
            break;
    }

    if(vendorCmdStatus != SUCCESS)
    {
        respResult = FALSE;
    }

    btGapMsg.msgData.gapBTParamSetResult.result = respResult;

    /* Send msg to App queue. */
    memcpy(&bee_io_msg.parm, &btGapMsg, sizeof(bee_io_msg.parm));
    GAP_SendBtMsgToApp(&bee_io_msg);

}
#endif

/**
  * @brief  process TBlueAPI_UsMessage message from bt stack, only gap related messages are handled here.
  * @param  pMsg - message sent from upper stack.
  * @retval TRUE - need to release buffer by application.
  *         FALSE - no need to release buffer by application.
  */
bool GAP_ProcessGapEvent( PBlueAPI_UsMessage pMsg )
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GAPRole_ProcessEvent: Command(0x%02x)", 1, pMsg->Command);
#endif

    switch ( pMsg->Command )
    {
    default:
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "GAPRole_Not_ProcessEvent: Command(0x%02x)", 1, pMsg->Command);
#endif
        break;

    case blueAPI_EventRegisterRsp:
        GAP_Handle_RegisterRsp( &pMsg->p.RegisterRsp );
        break;

    case blueAPI_EventActInfo:
        GAP_Handle_ActInfo( &pMsg->p.ActInfo );
        break;

    case blueAPI_EventSetBleTxPowerRsp:
        GAP_Handle_SetBleTxPowerRsp( &pMsg->p.SetBleTxPowerRsp );
        break;

#ifdef BT_GAP_VENDOR_CMD_ADD
    case blueAPI_EventIdle + 1:
        GAP_Handle_VendorCmdRsp( &pMsg->p );
        break;
#endif

    }
    return true;
}

/**
  * @brief  Get L2CAP connection handle from upper local_MDL_ID.
  * @param[in]  local_MDL_ID - when GATT connection created, the ID will be informed to application.
  * @retval LE_conn_handle - L2CAP connection handle.
  */
uint16_t blueAPI_GetLEConnHandle(uint16_t local_MDL_ID)
{
    uint16_t LE_conn_handle;

    TApiBufUserDefined ApiBufUserDefined;
    ApiBufUserDefined.Type = API_TYPE_GET_LE_CON_HDL_OF_MDL;
    ApiBufUserDefined.p.pLEConnHdl = &local_MDL_ID;
    if(!blueAPI_UserDefined(&ApiBufUserDefined))
    {
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "blueAPI_GetLEConnHandle: get handle failed!!", 0);
        return 0;
    }
    LE_conn_handle = local_MDL_ID;
    
    return LE_conn_handle;
}

/**
  * @brief  Send HCI Vendor command to lower stack from application.
  * @param[in]  opCode - HCI opcode.
  * @param[in]  len - length of HCI command parameters.
  * @param[in]  para - HCI command parameters.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_GattSendVendorHciCmd(uint16_t opCode, uint8_t len, uint8_t *para)
{
    TApiBufUserDefined ApiBufUserDefined;
    ApiBufUserDefined.Type = API_TYPE_GATT_SEND_VENDOR_HCI_CMD;
    ApiBufUserDefined.p.ApiBufVendorCmd.opCode = opCode;
    ApiBufUserDefined.p.ApiBufVendorCmd.len = len;
    ApiBufUserDefined.p.ApiBufVendorCmd.para = para;
    return blueAPI_UserDefined(&ApiBufUserDefined); 
}
/**
  * @brief  On/off slave latency.
  * @param[in]  latency_onoff  0- Close latency 1- Open latency.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_OnOffLatency(uint8_t latency_onoff)
{   
    uint8_t len;
    uint8_t hci_para[1] = {0};
    if (latency_onoff)
    {
        latency_onoff = 0;
    }
    else
    {
        latency_onoff = 1;
    }

    hci_para[0] = latency_onoff;
    len = sizeof(hci_para);

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "blueAPI_OnOffLatency: latency_onoff = %d, len = %d",
                    2, latency_onoff, len);
    
    return blueAPI_GattSendVendorHciCmd(HCI_VENDOR_ONOFF_SLAVE_LATENCY, len, hci_para);
}

/**
  * @brief  Read the peer device's RSSI value of the connection.
  * @param[in]  local_MDL_ID - connection handle.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_ReadRSSI(uint16_t local_MDL_ID)
{
    uint16_t LE_conn_hdl;
    uint8_t len;
    /* Read RSSI HCI command has only 2 bytes' parameter. */
    uint8_t hci_para[2] = {0};

    len = sizeof(hci_para);
        
    /* Get LE connection handle param from the given upper local MDL ID. */
    LE_conn_hdl = blueAPI_GetLEConnHandle(local_MDL_ID);
    if(LE_conn_hdl == 0)
    {
        return FALSE;
    }
    LE_WORD2EXTRN(hci_para, LE_conn_hdl);
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "blueAPI_ReadRSSI: local_MDL_ID = %d, LE_conn_hdl = %d, len = %d",
                    3, local_MDL_ID, LE_conn_hdl, len);
    
    return blueAPI_GattSendVendorHciCmd(HCI_READ_RSSI, len, hci_para);
}

/**
  * @brief  Read the patch version.
  * @param[out]  PatchVersion - output patch version in this pointer.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_ReadPatchVersion(uint16_t * PatchVersion)
{
    TApiBufUserDefined ApiBufUserDefined;
    ApiBufUserDefined.Type = API_TYPE_READ_PATCH_VERSION;
    ApiBufUserDefined.p.pPatchVersion = PatchVersion;
    return blueAPI_UserDefined(&ApiBufUserDefined);     
}

/**
  * @brief  HCI command, set host channel classification.
  * @param[in]  pChannelMap - channel map to set.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_LESetHostChanClass(uint8_t *pChannelMap)
{
    /* LE Channel Map: 37 data channels, 5 bytes. */
    uint8_t len = 5;
            
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "blueAPI_LESetHostChanClass: pChannelMap (%p)",
                    1, pChannelMap);
    
    return blueAPI_GattSendVendorHciCmd(HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION_OPCODE, len, pChannelMap);
}

/**
  * @brief  HCI vendor command, set local sca 32k external.
  * @param[in]  sca_index - sca index value.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_SetLocalSca32kExt(TSca32kExt sca_index)
{
    uint8_t len;
    uint8_t hci_para[2] = {0};

    hci_para[0] = 7;
    hci_para[1] = (uint8_t)sca_index;

    len = sizeof(hci_para);
            
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "blueAPI_SetLocalSca32kExt: sca_index (%d)",
                    1, sca_index);
    
    return blueAPI_GattSendVendorHciCmd(HCI_VENDOR_LPS_SETTING, len, hci_para);
}

/**
  * @brief  Enable/Disable advertising using io control interface.
  * @param  bEnable - enable or disable advertising.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_IOCTL_AdvertisingEnable(uint8_t bEnable)
{
    TApiBufUserDefined ApiBufUserDefined;
    ApiBufUserDefined.Type = API_TYPE_FMC_IOCTL;
    ApiBufUserDefined.p.fmcIoctl.cmd = 0x200A;
    ApiBufUserDefined.p.fmcIoctl.p1 = 1;
    ApiBufUserDefined.p.fmcIoctl.p2 = (uint32_t)&bEnable;
    return blueAPI_UserDefined(&ApiBufUserDefined);     
}

/**
  * @brief  Set advertising data using io control interface.
  * @param  pData - advertising data to set.
  * @param  len - length of advertising data to set.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_IOCTL_SetAdvertisingData(uint8_t *pData, uint8_t len)
{
    TApiBufUserDefined ApiBufUserDefined;
    ApiBufUserDefined.Type = API_TYPE_FMC_IOCTL;
    ApiBufUserDefined.p.fmcIoctl.cmd = 0x2008;
    ApiBufUserDefined.p.fmcIoctl.p1 = len;
    ApiBufUserDefined.p.fmcIoctl.p2 = (uint32_t)pData;
    return blueAPI_UserDefined(&ApiBufUserDefined);
}

/**
  * @brief  Set Adv fast disable.
  * @param[in] enable 0x01 (enable adv fast disable) / 0x00 (disable adv fast disable)
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_SetAdvFastDisable(uint8_t enable)
{
    uint8_t len;
    uint8_t hci_para[2] = {0};

    hci_para[0] = 1;
    hci_para[1] = enable;
    len = sizeof(hci_para);
            
    return blueAPI_GattSendVendorHciCmd(HCI_VENDOR_SET_ADV_FAST_DISABLE, len, hci_para);    
}

/**
  * @brief  Hand Shake with windows audio tool.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_HandShakeInit(void)
{
    TApiBufUserDefined ApiBufUserDefined;
    ApiBufUserDefined.Type = API_TYPE_INIT_SEC_KEY;
    return blueAPI_UserDefined(&ApiBufUserDefined);
}

/**
  * @brief  Get response value when hand shake with windows audio tool.
  * @param  pData - response value.
  * @param  len - length of response value.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_HandShakeGetResponse(uint8_t *pData, uint8_t len)
{
    TApiBufUserDefined ApiBufUserDefined;
    ApiBufUserDefined.Type = API_TYPE_ENCRYPT_DATA;
    ApiBufUserDefined.p.pEncryptData = pData;
    return blueAPI_UserDefined(&ApiBufUserDefined);
}

/**
  * @brief  Send vendor ACL data from upper application.
  * @param[in]  local_MDL_ID - connection handle of upper app.
  * @param[in]  pData - data to be sent.
  * @param[in]  len - length of data.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_SendVendorAclData(uint16_t local_MDL_ID, uint8_t *pData, uint8_t len)
{
    TApiBufUserDefined ApiBufUserDefined;
    ApiBufUserDefined.Type = API_TYPE_SEND_HCI_VENDOR_ACL_DATA;
    ApiBufUserDefined.p.AclVendorData.local_MDL_ID = local_MDL_ID;
    ApiBufUserDefined.p.AclVendorData.len = len;
    ApiBufUserDefined.p.AclVendorData.pData = pData;
    return blueAPI_UserDefined(&ApiBufUserDefined);
}

#ifdef BT_GAP_VENDOR_AES_API_SUPPORT
/**
  * @brief  Set AES key from upper application.
  * @param[in]  ctx - AES context, must allocated in upper application.
  * @param[in]  key - AES key will be used in AES algorithm.
  * @param[in]  nbits - key bits, 128/192/256.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_AES_SetKey(aes_context *ctx, uint8_t *key, int nbits)
{
    TApiBufUserDefined ApiBufUserDefined;
    ApiBufUserDefined.Type = API_TYPE_AES_SET_KEY;
    ApiBufUserDefined.p.aesSetKeyParam.ctx = ctx;
    ApiBufUserDefined.p.aesSetKeyParam.key = key;
    ApiBufUserDefined.p.aesSetKeyParam.nbits = nbits;
    return blueAPI_UserDefined(&ApiBufUserDefined);
}

/**
  * @brief  Encrypt your data use AES algorithm from upper application.
  * @param[in]  ctx - AES context, must initiated in upper application.
  * @param[in]  input - data to be encrypted.
  * @param[out]  output - data output after encryption.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_AES_Encrypt(aes_context *ctx, uint8_t input[16], uint8_t output[16])
{
    TApiBufUserDefined ApiBufUserDefined;
    ApiBufUserDefined.Type = API_TYPE_AES_ENCRYPT;
    ApiBufUserDefined.p.aesEncryptParam.ctx = ctx;
    ApiBufUserDefined.p.aesEncryptParam.input = input;
    ApiBufUserDefined.p.aesEncryptParam.output = output;
    return blueAPI_UserDefined(&ApiBufUserDefined);
}

/**
  * @brief  Decrypt your data use AES algorithm from upper application.
  * @param[in]  ctx - AES context, must initiated in upper application.
  * @param[in]  input - data to be decrypted.
  * @param[out]  output - data output after decryption.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_AES_Decrypt(aes_context *ctx, uint8_t input[16], uint8_t output[16])
{
    TApiBufUserDefined ApiBufUserDefined;
    ApiBufUserDefined.Type = API_TYPE_AES_DECRYPT;
    ApiBufUserDefined.p.aesDecryptParam.ctx = ctx;
    ApiBufUserDefined.p.aesDecryptParam.input = input;
    ApiBufUserDefined.p.aesDecryptParam.output = output;
    return blueAPI_UserDefined(&ApiBufUserDefined);
}
#endif

#ifdef BT_GAP_BOND_KEY_DISTRIBUTION
/**
  * @brief  Set Key distribution mode to upper stack.
  * @param  mode privacy mode
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_SetSMPKeyDistribution(uint8_t mode)
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "blueAPI_SetSMPKeyDistribution:%d", 1, mode);
#endif
    TApiBufUserDefined ApiBufUserDefined;
    ApiBufUserDefined.Type = API_TYPE_SMP_KEY_DISTRIBUTION;
    ApiBufUserDefined.p.key_mode = mode;
    return blueAPI_UserDefined(&ApiBufUserDefined);
}

/**
  * @brief  Set Local IRK to upper stack.
  * @param  local_irk - Local IRK to set.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_SetSMPLocalIRK(uint8_t local_irk[16])
{
#ifdef GAP_DEBUG_FLAG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "blueAPI_SetSMPLocalIRK", 0);
#endif
    TApiBufUserDefined ApiBufUserDefined;
    ApiBufUserDefined.Type = API_TYPE_SMP_LOCAL_IRK;
    ApiBufUserDefined.p.pLocalIRK = local_irk;
    return blueAPI_UserDefined(&ApiBufUserDefined);
}
#endif

/**
  * @brief  Set the delay time before enter DLPS for the first time.
  * @param  time -- delay time before enter DLPS, 50ms per step.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool RapidEnterDLPS(uint16_t time)
{
    uint8_t len;
    uint8_t hci_para[3] = {0};

    hci_para[0] = 5;
    hci_para[1] = (uint8_t)(time & 0x00ff);
    hci_para[2] = (uint8_t)(time >> 8);

    len = sizeof(hci_para);
            
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RapidEnterDLPS: time (%d)",
                    1, time);
    
    return blueAPI_GattSendVendorHciCmd(HCI_VENDOR_LPS_SETTING, len, hci_para);
}

/**
  * @brief  Set the threshold time of enter DLPS.
  * @param  time -- DLPS threshold time, 0.625ms per step.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool SetDLPSThreshold(uint16_t time)
{
    uint8_t len;
    uint8_t hci_para[3] = {0};

    hci_para[0] = 8;
    hci_para[1] = (uint8_t)(time & 0x00ff);
    hci_para[2] = (uint8_t)(time >> 8);

    len = sizeof(hci_para);
            
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SetDLPSThreshold: time (%d)",
                    1, time);
    
    return blueAPI_GattSendVendorHciCmd(HCI_VENDOR_LPS_SETTING, len, hci_para);
}

/**
  * @brief  Start LE TX Test.
  * @param  tx_channel - Range 0-39.
  * @param  length_of_test_data - Range 0-0xFF.
  * @param  packet_payload - LL_TEST_PKT_TYPE_SET.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_LETXTestStart(uint8_t tx_channel, uint8_t length_of_test_data, LL_TEST_PKT_TYPE_SET packet_payload)
{
    uint8_t len;
    uint8_t hci_para[3] = {0};
    
    hci_para[0] = tx_channel;
    hci_para[1] = length_of_test_data;
    hci_para[2] = (uint8_t)packet_payload;

    len = sizeof(hci_para);
            
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "blueAPI_LETXTestCmdStart: chan: %d, data length: %d, payload: %d",
                    3, tx_channel, length_of_test_data, packet_payload);
    
    return blueAPI_GattSendVendorHciCmd(HCI_LE_TRANSMITTER_TEST_OPCODE, len, hci_para);
}

/**
  * @brief  Stop LE TX Test.
  * @param  none.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_LETXTestStop(void)
{
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "blueAPI_LETXTestEnd", 0);
    
    return blueAPI_GattSendVendorHciCmd(HCI_LE_TEST_END_OPCODE, 0, NULL);
}

/**
  * @brief  Set RX sensitivity, could only be used in LE idle state.
  * @param  bEnable -- 0x0 (disable): set 0x18 and 0x13 to default value; 0x1 (enable): set 0x18 and 0x13 to 0x5D83 and 0x76BE.
  * @retval TRUE/FALSE - call this api success or not.
  */
bool blueAPI_SetRXSensitivityReg(uint8_t bEnable)
{
    uint8_t len;
    uint8_t hci_para[1] = {0};
    hci_para[0] = bEnable;
		
    len = sizeof(hci_para);
            
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "blueAPI_SetRXSensitivityReg: bEnable (%d)",
                    1, bEnable);
    
    return blueAPI_GattSendVendorHciCmd(HCI_VENDOR_SET_RX_SENSITIVITY_REG, len, hci_para);
}


