enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     profile.c
* @brief    the general implementation for profiles.
* @author   lilly_he
* @date     2014-05-21
* @version  v0.1
*********************************************************************************************************
*/
#include "blueapi.h"
#include "blueapi_types.h"
#include "stdint.h"
#include "stddef.h"
#include "gatt.h"
#include "trace.h"
#include "profile.h"
#include <string.h>
#include "profileApi.h"
#include "profileConfig.h"

/********************************************************************************************************
* global variables defined here.
********************************************************************************************************/
/**<  Number of all services used in application. */
uint8_t gServicesCount = 0;
/**<  Global data struct of profile spesific data. */
TPROFILE_DATA gProfileData = {0};
/**<  Array of all used services' informations. */
gattServiceInfos_t gattSrvInfo[PROFILE_SUPPORT_SERVICE_NUM] = {0};
/**<  Function pointer used to send event to application from profile. */
static pfnAPPHandleInfoCB_t pfnProfileCB = NULL;
/* Function pointer used to send handle prepare write & execute write. */
gattPreExeWrCB_t pfnPreExeWrCB = {NULL, NULL};
/**<  MTU size. */
uint16_t profileMtuSize = 0;

#ifdef ANCS_SUPPORT
pfnAPPHandleAncsCB_t pfnAncsCB = NULL;
#endif

/**
  * @brief  Convert app result to profile result.
  * @param  appResult: app result.
  * @return Profile result.
  */
TProfileResult  ProfileAPI_GetProfileResult(TAppResult appResult)
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
 * @brief read characteristic data from service.
 *
 * @param serviceIdx         ServiceID of service.
 * @param iAttribIndex          Attribute index of getting characteristic data.
 * @param iOffset                Used for Blob Read.
 * @param piLength            length of getting characteristic data.
 * @param ppValue            data got from service.
 * @return Profile procedure result
*/
TProfileResult  profileReadAttrib(uint8_t serviceIdx, uint16_t iAttribIndex,
                                  uint16_t iOffset, uint16_t * pwLength, uint8_t **ppValue )
{
    TProfileResult  wCause  = ProfileResult_InvalidParameter;

    *pwLength = 0;

    if (serviceIdx >= PROFILE_SUPPORT_SERVICE_NUM)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "profileReadAttrib ServiceIDx(%d) is not in gattSrvInfo \n",
                   1, serviceIdx);
        return wCause;
    }

    if (gattSrvInfo[serviceIdx].cbInfo.pfnReadAttrCB != NULL)
    {
        /* Call read callback function to get data from services. */
        wCause = gattSrvInfo[serviceIdx].cbInfo.pfnReadAttrCB(serviceIdx, iAttribIndex, iOffset, pwLength, ppValue);
        if(wCause == ProfileResult_Success)            
        {
            if (*pwLength > (profileMtuSize - 1) + iOffset)
            {
                *pwLength = (profileMtuSize - 1) + iOffset;
            }

            if ((*pwLength - iOffset) >= 0 )
            {
                *pwLength -= iOffset;
                *ppValue  += iOffset;
                wCause   = ProfileResult_Success;
            }
            else
            {
                wCause = ProfileResult_InvalidOffset;
            }
        }
    }
    
    return ( wCause );
}

/**
  * @brief  Confirm from application when receive read request from client.
  * @param  ServiceIdx: service ID index.
  * @param  attribIndex: Attribute index of attribute to read confirm from application.
  * @param  pData: Point to the data which client reads.
  * @param  wLength: The length of the data
  * @retval TRUE Confirm from app OK.
  * @retval FALSE Confirm from app failed.
  */
bool ProfileAPI_AttribValueReadConf(uint8_t ServiceIdx,
                         uint16_t wAttribIndex,
                         uint8_t* pData,
                         uint16_t     wLength)
{
    PGATTDService pService;
    uint8_t *      pBuffer;
    uint16_t       wOffset = gProfileData.wDsDataOffset;
    TProfileResult wCause  = ProfileResult_Success;
    
    pService = &gattSrvInfo[ServiceIdx].Service;

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
                                       gProfileData.local_MDL_ID,
                                       pService->pServiceHandle,
                                       (wCause != ProfileResult_Success) ?
                                       blueAPI_CauseLowerLayerError : blueAPI_CauseSuccess,
                                       (uint16_t)wCause,
                                       wAttribIndex,
                                       wLength,
                                       wOffset
                                      )
       )
    {
#ifdef PROFILE_DEBUG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "--> ProfileAPI_AttribValueReadConf: wCause=%d, length=%d",
                   2, wCause, wLength);
#endif

    }
    else
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "ProfileAPI_AttribValueReadConf illegal parameter (e.g. offset too small)", 0);
        if ( pBuffer != NULL )
            blueAPI_BufferRelease(pBuffer);
    }

    return ( true );
}



/**
 * @brief write characteristic data from service.
 *
 * @param serviceIdx          ServiceID to be written.
 * @param iAttribIndex          Attribute index of characteristic.
 * @param wLength            length of value to be written.
 * @param pValue            value to be written.
 * @param pWriteIndPostProc pointer of a function to handle control point write.
 * @return Profile procedure result
*/
TProfileResult  profileWriteAttrib(uint8_t serviceIdx, uint16_t iAttribIndex,
                                   uint16_t wLength, uint8_t * pValue, TGATTDWriteIndPostProc * pWriteIndPostProc)
{
    TProfileResult  wCause  = ProfileResult_InvalidParameter;
    *pWriteIndPostProc = NULL;

    if (serviceIdx >= PROFILE_SUPPORT_SERVICE_NUM)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "profileWriteAttrib serviceIdx(%d) is not in gattSrvInfo\n",
                   1, serviceIdx);
        wCause = ProfileResult_AttrNotFound;
        return wCause;
    }

    if (gattSrvInfo[serviceIdx].cbInfo.pfnWriteAttrCB != NULL)
    {
        /* Call write callback function writing data to services. */
        wCause = gattSrvInfo[serviceIdx].cbInfo.pfnWriteAttrCB(serviceIdx, iAttribIndex, wLength, pValue, pWriteIndPostProc);
        return ( wCause );
    }
    else
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "ServiceID(%d) pfnWriteAttrCB = NULL\n",
                   1, serviceIdx);
    }

    return ( wCause );
}

/**
 * @brief write characteristic data from service.
 *
 * @param serviceIdx          ServiceID to be written.
 * @param iAttribIndex          Attribute index of characteristic.
 * @param wLength            length of value to be written.
 * @param pValue            value to be written.
 * @param pWriteIndPostProc pointer of a function to handle control point write.
 * @return Profile procedure result
*/
TProfileResult  profilePreWriteAttrib(uint8_t serviceIdx, uint16_t handle, uint16_t iAttribIndex,
                                   uint16_t wLength, uint8_t * pValue, uint16_t offset,
                                   TGATTDWriteIndPostProc * pWriteIndPostProc)
{
    TProfileResult  wCause  = ProfileResult_InvalidParameter;
    TAppResult appResult = AppResult_AplicationErr;
    
    *pWriteIndPostProc = NULL;

    if (serviceIdx >= PROFILE_SUPPORT_SERVICE_NUM)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "profilePreWriteAttrib serviceIdx(%d) is not in gattSrvInfo\n",
                   1, serviceIdx);
        return wCause;
    }
    
    if(pfnPreExeWrCB.pfnPreWriteAttrCB)
    {
        appResult = pfnPreExeWrCB.pfnPreWriteAttrCB(serviceIdx, handle, iAttribIndex, wLength, pValue, offset);
    }
    else
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "-->profilePreWriteAttrib:pfnPreWriteAttrCB = NULL\n", 0);
    }

    wCause = (TProfileResult)appResult;

    return wCause;
}


/**
 * @brief handle execute write.
 *
 * @param mdlId         local MDL ID.
 * @param flag          1--write, 0--no write.
 * @return Profile procedure result
*/
TProfileResult profileExeWriteAttrib(uint8_t mdlId, uint8_t flag, uint16_t* handle)
{
    TProfileResult  wCause  = ProfileResult_InvalidParameter;
    TAppResult appResult = AppResult_AplicationErr;
    
    if(pfnPreExeWrCB.pfnExeWriteAttrCB)
    {
        appResult = pfnPreExeWrCB.pfnExeWriteAttrCB(flag, handle);
    }
    else
    {
         DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "-->profileExeWriteAttrib:pfnExeWriteAttrCB = NULL\n", 0);
    }
    
    wCause = (TProfileResult)appResult;
    
    return wCause;
}
/**
  * @brief  Confirm from application when receive execute write request from client.
  * @retval TRUE Confirm from app OK.
  * @retval FALSE Confirm from app failed.
  */
bool ProfileAPI_ExeWriteConf(void)
{
    if(blueAPI_GATTAttributeExecuteWriteConf(gProfileData.local_MDL_ID,
                                    blueAPI_CauseSuccess,
                                    GATT_SUCCESS,
                                    0))
    {
#ifdef PROFILE_DEBUG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "--> App execute write confirm", 0);
#endif
    }
   return true;
}

/**
 * @brief find service state according to input service-handle.
 *
 * @param pServiceHandle         service-handle.
 * @return service state found.
*/
PGATTDService  profileServiceFind(void * pServiceHandle )
{
    int  i;

    for ( i = 0; i < gServicesCount; i++ )
    {
        if ( gattSrvInfo[i].Service.Used &&
                (gattSrvInfo[i].Service.pServiceHandle == pServiceHandle)
           )
        {
            return ( &gattSrvInfo[i].Service );
        }
    }

    return ( NULL );
}

/**
 * @brief Send service register complete event to application.
 *
 * @param Status         status of service register result.
 * @return none.
*/
void profileSrvStartCompleteEvt(uint8_t Status)
{
    TEventInfoCBs_t data = {0};

    data.eventId = PROFILE_EVT_SRV_REG_COMPLETE;
    data.sParas[0] = Status;
#ifdef PROFILE_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "profileSrvStartCompleteEvt Status = %d", 1, Status);
#endif
    /* Notify Application. */
    if (pfnProfileCB)
    {
        pfnProfileCB(ProfileAPI_ServiceUndefined, (void*)&data);
    }
}

/**
 * @brief send data-send complete event to application.
 *
 * @param wCredits         for flow-control.
 * @return none.
*/
void profileDataTxComplete(uint16_t wCredits)
{
    TEventInfoCBs_t data = {0};
    data.eventId = PROFILE_EVT_SEND_DATA_COMPLETE;
    data.sParas[0] = wCredits;

    /* Notify Application. */
    if (pfnProfileCB)
    {
        pfnProfileCB(ProfileAPI_ServiceUndefined, (void*)&data);
    }

}

/**
 * @brief update CCCD bits from stack.
 *
 * @param ServiceIdx          Service ID.
 * @param wAttribIndex          Attribute index of characteristic data.
 * @param wCCCBits         CCCD bits from stack.
 * @return None
*/
void profileUpdateCCCDInfo(uint8_t ServiceIdx, uint16_t wAttribIndex, uint16_t wCCCBits)
{
#ifdef PROFILE_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "profileUpdateCCCDInfo ServiceId %d", 1, ServiceIdx);
#endif
    if (ServiceIdx >= PROFILE_SUPPORT_SERVICE_NUM)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "profileUpdateCCCDInfo ServiceID(%d) is not in gattSrvInfo\n",
                   1, ServiceIdx);
        return;
    }

    if (gattSrvInfo[ServiceIdx].cbInfo.pfnUpdateCccdCB != NULL)
    {
        /* Call CCCD update callback function. */
        gattSrvInfo[ServiceIdx].cbInfo.pfnUpdateCccdCB(ServiceIdx, wAttribIndex, wCCCBits);
    }
}

/**
 * @brief register services to GATT from profile.
 *
 * @param none
 * @return register result.
 * @retval TRUE 1.
 * @retval FALSE 0.
*/
bool profileServiceRegister(void)
{
    PGATTDService   pService;
    bool bResult = FALSE;
    uint16_t iProfileSize = 0;
    uint8_t* pProfileDatabase = NULL;

    /* get service database to be registerd. */
    pProfileDatabase = gattSrvInfo[gProfileData.iServiceCountIndex].pAttrTbl;
    iProfileSize = gattSrvInfo[gProfileData.iServiceCountIndex].Length;
 #ifdef PROFILE_DEBUG   
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "gattdServiceRegister index %d", 1, gProfileData.iServiceCountIndex);
 #endif
    /* check if service database is valid. */
    if (!pProfileDatabase)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "gattdServiceRegister pProfileDatabase= NULL!~~~serviceId = 0x%x", 1,
                   gProfileData.iServiceCountIndex);
        return ( bResult );
    }
    /* check if service index is invalid or service has been registerd before. */
    if ( (gProfileData.iServiceCountIndex >= PROFILE_SUPPORT_SERVICE_NUM) ||
            gattSrvInfo[gProfileData.iServiceCountIndex].Service.Used )
    {
        return ( bResult );
    }
    pService       = &gattSrvInfo[gProfileData.iServiceCountIndex].Service;
    pService->ServiceIDx = gattSrvInfo[gProfileData.iServiceCountIndex].Service.ServiceIDx;
    pService->Used = true;
    /* send register request to upper stack. */
    if ( blueAPI_GATTServiceRegisterReq(
                iProfileSize / sizeof(TAttribAppl),
                pProfileDatabase
            )
       )
    {
#ifdef PROFILE_DEBUG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "--> GATTServiceRegisterReq: pService=0x%x, nbrOfAttrib=%d, serviceId %d",
                   3,
                   pProfileDatabase,
                   iProfileSize / sizeof(TAttribAppl),
                   pService->ServiceIDx);
#endif
        bResult = TRUE;
    }

    return ( bResult );
}

/**
 * @brief Send characteristic value to peer device.
 *
 * @param serviceIdx          ServiceID.
 * @param iAttribIndex          Attribute index of characteristic.
 * @param wLength            length of value to be sent.
 * @param pData             data to be sent.
 * @return data sent result
 * @retval TRUE Success.
 * @retval FALSE Failed.
*/
bool ProfileAPI_SendData(uint8_t ServiceIdx,
                         uint16_t wAttribIndex,
                         uint8_t* pData,
                         uint16_t     wLength)
{
    bool Result = true;
    PGATTDService pService;
    uint16_t ReportMTU = gProfileData.wMTUSize - 3;

    /* check if data is too long to sent. */
    if (wLength > ReportMTU)
    {
        Result = false;
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "ProfileAPI_SendData wLength(=%d)>(MTU-3)=%d\n",
                   2, wLength, ReportMTU);
        return Result;
    }
    /* check for flow control. */
    if (gProfileData.wDsCredits == 0)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "ProfileAPI_SendData Error Credits=%d\n",
                   1, gProfileData.wDsCredits);
        Result = false;
        return Result;
    }

    if (ServiceIdx >= PROFILE_SUPPORT_SERVICE_NUM)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "ProfileAPI_SendData ServiceID(%d) is not in gattSrvInfo\n",
                   1, ServiceIdx);
        Result = false;
        return Result;
    }

    pService = &gattSrvInfo[ServiceIdx].Service;

    if ( wLength != 0 && pData != NULL)
    {
        union
        {
            uint32_t d;
            void    *p;
        } requestHandle;
        
        uint8_t   * pBuffer = NULL;

        uint16_t    wOffset = gProfileData.wDsDataOffset + 3;

        if ( pData == NULL )
        {
            /* value is directly accessible by BT stack */
            wLength = 0;
        }
        else
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
                return ( false );
            }
        }

        requestHandle.d = gProfileData.wUpdReqHandle;
        if ( blueAPI_GATTAttributeUpdateReq(pBuffer,

                                            pService->pServiceHandle,
                                            requestHandle.p,
                                            wAttribIndex,
                                            wLength,
                                            wOffset
                                           )
           )
        {
#ifdef PROFILE_DEBUG        
            DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "--> GATT_Notification: ServiceID=0x%x, attribIndex=%d",
                       2,
                       pService->ServiceIDx,
                       wAttribIndex);
#endif

            gProfileData.wDsCredits--;

            gProfileData.wUpdReqHandle++;
            // gSrvSequnceCtrl.iUpdateSent++;
            Result = true;
        }
        else
        {
            DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "GATT_Notification!!! illegal parameter (e.g. offset too small)", 0);
            Result = false;
            if ( pBuffer != NULL )
                blueAPI_BufferRelease(pBuffer);
        }
    }

    return Result;
}

/**
 * @brief Register callback function to send events to application.
 *
 * @param pFunc          callback function.
 * @return none
*/
void ProfileAPI_RegisterCB(pfnAPPHandleInfoCB_t pFunc)
{
    pfnProfileCB = pFunc;
}

/**
 * @brief Register callback function to handle prepare write & execute write.
 *
 * @param pFunc          callback function.
 * @return none
*/
void ProfileAPI_RegisterPreExeWrCB(gattPreExeWrCB_t pFunc)
{
    pfnPreExeWrCB = pFunc;
    
    return;
}

/**
  * @brief  Confirm from application when receive Write Request from client.
  * @param  local_MDL_id: local MDL id indicate which link is.
  * @param  ServiceIdx: service ID index.
  * @param  attribIndex: Attribute index of attribute to write confirm from application.
  * @param  wCause: write request app handle result, ProfileResult_Success or other.
  * @retval TRUE Confirm from app OK.
  * @retval FALSE Confirm from app failed.
  */
bool ProfileAPI_AttributeWriteConf( uint16_t local_MDL_id,
                                    uint8_t ServiceIdx,
                                    uint16_t attribIndex,
                                    TProfileResult wCause )
{
    bool result = FALSE;
    PGATTDService pService;

    pService = &gattSrvInfo[ServiceIdx].Service;
    
    /* send response/ack */
    if ( blueAPI_GATTAttributeWriteConf( local_MDL_id,
                                         pService->pServiceHandle,
                                         (wCause != ProfileResult_Success) ?
                                         blueAPI_CauseLowerLayerError : blueAPI_CauseSuccess,
                                         (uint16_t)wCause,
                                         attribIndex ) )
    {
        result = TRUE;
#ifdef PROFILE_DEBUG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "ProfileAPI_AttributeWriteConf subCause=0x%x",
                   1, wCause);
#endif
    }
    return result;
}

#ifdef ANCS_SUPPORT
void ProfileAPI_RegisterAncsNotification(pfnAPPHandleAncsCB_t pFunc)
{
    pfnAncsCB = pFunc;
}
#endif
/**
 * @brief Add specific service infomation to gattSrvInfo struct, will be registered to GATT later.
 *
 * @param outServiceId              Service ID of specific service.
 * @param pDB                   Database pointer of specific service.
 * @param Length                Length of Database of specific service.
 * @param cCBs                      Service callback functions of specific service.
 * @retval TRUE success
 * @retval FALSE failed
*/
bool ProfileAPI_AddService(uint8_t* outServiceId, uint8_t* pDB, uint16_t Length, CONST gattServiceCBs_t cCBs)
{
    bool bRetStatus = TRUE;
    *outServiceId = gServicesCount;

    if (gServicesCount == 0xff)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "ProfileAPI_AddService ServiceId %d already exits", 1, *outServiceId);
        bRetStatus = FALSE;
        return bRetStatus;
    }
    if(gServicesCount >= PROFILE_SUPPORT_SERVICE_NUM)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "ProfileAPI_AddService gServicesCount %d More than SERVICE_NUM%d ",
            1, gServicesCount,PROFILE_SUPPORT_SERVICE_NUM);
        bRetStatus = FALSE;
        return bRetStatus;
    }

    gattSrvInfo[gServicesCount].Service.ServiceIDx = *outServiceId;
    gattSrvInfo[gServicesCount].pAttrTbl = pDB;
    gattSrvInfo[gServicesCount].Length = Length;
    gattSrvInfo[gServicesCount].cbInfo = cCBs;

    gServicesCount += 1;

    return bRetStatus;
}

/**
 * @brief maintain some connection information after connected.
 *
 * @param pConnectMDLInfo          upstream data from upper stack.
 * @return none
*/
void profileUpdateConnInfo(PBlueAPI_ConnectMDLInfo pConnectMDLInfo)
{
#ifdef PROFILE_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "profileSetConnInfo\n", 0);
#endif
    gProfileData.wDsPoolId     = pConnectMDLInfo->dsPoolID;
    gProfileData.wDsDataOffset = pConnectMDLInfo->dsDataOffset;
    gProfileData.wDsCredits    = pConnectMDLInfo->maxTPDUdsCredits;
    gProfileData.wMTUSize      = pConnectMDLInfo->maxTPDUSize;
    gProfileData.local_MDL_ID  = pConnectMDLInfo->local_MDL_ID;
    gProfileData.wUpdReqHandle = 0;
    profileMtuSize = pConnectMDLInfo->maxTPDUSize;
    
    return;
}

/**
 * @brief update MTU size.
 *
 * @param mtu_size          MTU size to be updated.
 * @return none
*/
void profileUpdateMtu(uint16_t mtu_size)
{
#ifdef PROFILE_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "profileUpdateMtu mtu_size = %d\n", 1, mtu_size);
#endif
    gProfileData.wMTUSize = mtu_size;
    profileMtuSize = mtu_size;

    return;
}

