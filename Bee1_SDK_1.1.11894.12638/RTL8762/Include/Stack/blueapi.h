/**
************************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     blueapi.h
* @brief    BlueAPI separate messages generator functions.
* @details  Supplies a separate message generator function for each request (Req) and confirmation (Conf) message.
*               This message generator functions are always named "blueAPI_" followed by the name of the message to be sent.
* @author   ranhui & jane
* @date     2015-10-29
* @version  v1.2
*************************************************************************************************************
*/

#ifndef __BLUEAPI_H
#define __BLUEAPI_H

#ifndef __BLUEAPI_TYPES_H
#include <blueapi_types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/** @addtogroup RTK_BlueAPI RTK BlueAPI
  * @{
  */

/** @defgroup Downstream_API_Functions Downstream API Functions
  * @brief APP->BlueAPI.      An application sends commands to the BlueAPI driver by issuing an API call for that specific message.
  * @{
  */

/** @defgroup Buffer_Management_API Buffer Management
  * @brief Buffer Management API
  * @{
  */

/**
 * @brief  Allocate a memory buffer.
 *
 *  The BlueAPI API library supplies this function to allocate a memory buffer
 *  that can be used to send data via the BlueAPI method(e.g. blueAPI_GATTAttributeReadConf)  to a remote device.
 *
 *  When BlueAPI functions have a parameter "pBuffer", buffers are allocated with blueAPI_BufferGet.
 *
 * @param dsPoolID  Downstream pool to allocate the buffer from.
 *                                This parameter is part of the MDLConnectInfo message
 *                                that is received by the APP for signaling MDL establishment.
 * @param dsTPDUSize  Length of payload to be sent.
 * @param dsDataOffset  Offset of payload relative to start of buffer.
 *                      The minimum value for this parameter is part of the MDLConnectInfo message that is received by the APP for signaling MDL establishment.
 * @param ppBuffer  Pointer to memory where the buffer start address is stored on return. In case of error NULL is returned.
 * @return The result of buffer get.
 * @retval blueAPI_CauseSuccess successfully
 * @retval other failed
*/
TBlueAPI_Cause  blueAPI_BufferGet(
    uint16_t        dsPoolID,
    uint16_t        dsTPDUSize,
    uint16_t        dsDataOffset,
    void          **ppBuffer
);

/**
 * @brief  Release buffer.
 *
 * @param pBuffer  Pointer to memory that need to release (address of begin of message header).
 * @return The result of buffer release.
 * @retval blueAPI_CauseSuccess successfully
 * @retval other failed
*/
TBlueAPI_Cause  blueAPI_BufferRelease(
    void           *pBuffer
);

/** @} End of Buffer_Management_API */

/** @defgroup Application_Management_API Application Management
  * @brief Application Management API
  * @{
  */

/**
 * @brief  Register application.
 *
 *  To have any communication with BlueAPI an application must register at BlueAPI first.
 *
 * @param appHandle  Application provided handle that will be stored by the BT Stack
 * @param MDHmsgHandlerCallback Callback function provided by the APP to handle messages sent from the BT Stack.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_RegisterReq(TBlueAPIAppHandle  appHandle,
                         void *             MDHmsgHandlerCallback
                        );

///@cond
bool blueAPI_ReleaseReq(void);
///@endcond

/** @} End of Application_Management_API */

/** @defgroup Connection_Management_API Connection Management
  * @brief Connection Management API
  * @{
  */

/**
 * @brief  Initiate a GATT connection.
 *
 *  This command is used by the APP to initiate a GATT connection.
 *
 * @param remote_BD  Bluetooth device address of remote device to connect to.
 *                                   Set to all zero for LE whitelist connect.
 * @param remote_BD_Type  Type of remote BD address.
 * @param local_BD_Type  Type of local BD address.
 * @param scanInterval  Value range: 0x0004 - 0x4000 (2.5ms - 10240ms, 0.625ms/step).
 * @param scanWindow  Value range: 0x0004 - 0x4000 (2.5ms - 10240ms, 0.625ms/step).
 * @param scanTimeout  Value range: 0x0001 - 0xFFFF (10ms - 655.56s, 10ms/step).
 * @param connIntervalMin  Value range: 0x0006 - 0x0C80 (7.5ms - 4000ms, 1.25ms/step).
 * @param connIntervalMax  Value range: 0x0006 - 0x0C80 (7.5ms - 4000ms, 1.25ms/step).
 * @param connLatency  Value range: 0x0000 - 0x01F3.
 * @param supervisionTimeout  Value range: 0x000A - 0x0C80 (100ms - 32000ms, 10ms/step).
 * @param CE_Length  Value range: 0x0000 ¨C 0xFFFF (0.625 ms/step).
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_ConnectGATTMDLReq(uint8_t              * remote_BD,
                               TBlueAPI_RemoteBDType  remote_BD_Type,
                               TBlueAPI_LocalBDType local_BD_Type,
                               uint16_t    scanInterval,
                               uint16_t    scanWindow,
                               uint16_t    scanTimeout,
                               uint16_t    connIntervalMin,
                               uint16_t    connIntervalMax,
                               uint16_t    connLatency,
                               uint16_t    supervisionTimeout,
                               uint16_t    CE_Length
                              );




/**
 * @brief  Respond to a blueAPI_EventCreateMDLInd.
 *
 * This command will be used by the APP to respond to a CreateMDLInd from the BT Stack.
 *
 * @param local_MDL_ID  Local link ID to be created.
 * @param maxTPDUusCredits Reserved for further use.
 * @param cause  Indicates the result of the createMDL transaction.
 *                           blueAPI_CauseAccept -- Accept MDL,
 *                           blueAPI_CauseReject -- Reject MDL
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_CreateMDLConf(uint16_t        local_MDL_ID,
                           uint8_t         maxTPDUusCredits,
                           TBlueAPI_Cause  cause
                          );

/**
 * @brief  Disconnect MDL.
 *
 *  This command shall be used by the APP to request a disconnection of
 *  an established MDL.
 *
 * @param local_MDL_ID  Local link ID to be disconnected.
 * @param cause  The cause for the disconnect.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_DisconnectMDLReq(uint16_t    local_MDL_ID,
                              TBlueAPI_Cause cause
                             );

/**
 * @brief  Respond to a blueAPI_EventDisconnectMDLInd.
 *
 * This command will be used by the APP to respond
 *  a DisconnectMDLInd of the BT Stack.
 *
 * @param local_MDL_ID  Local link ID to be disconnected.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_DisconnectMDLConf(uint16_t        local_MDL_ID
                              );
/** @} End of Connection_Management_API */

/** @defgroup GATT_Server_API GATT Server
  * @brief GATT Server API
  * @{
  */
/**
 * @brief  Register GATT service.
 *
 *  The purpose of this command is to register one
 *  or more GATT based services to the BT Stack.
 *
 * @param nbrOfAttrib  Number of attributes in service structure.
 * @param pService  Pointer to service structure to register, may point to a RAM or ROM area. Service structure: @ref Attributes_Table
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_GATTServiceRegisterReq(uint16_t   nbrOfAttrib,
                                    void *     pService
                                   );


///@cond

bool blueAPI_GATTServiceReleaseReq(void *      serviceHandle
                                  );
///@endcond

/**
 * @brief  GATT attribute update.
 *
 *  The APP uses this command to inform the BT Stack
 *  that the value of an attribute has changed.
 *
 * @param pBuffer  Point to the buffer allocated by APP.
 *                              The buffer stores the attribute value.
 * @param serviceHandle  Handle of service that contains the attribute to update.
 * @param requestHandle  Request handle that can be freely assigned by
 *                                      the APP to track progress of each specific attribute update.
 * @param attribIndex  Index of attribute in service descriptor array.
 * @param attribLength  Attribute value/data length.
 *                                    For attributes with Flags bit VALUE_APPL=0
 *                                    this parameter and pBuffer are ignored.
 * @param offset  Offset of attribute value in pBuffer.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_GATTAttributeUpdateReq(void *     pBuffer,
                                    void *     serviceHandle,
                                    void *     requestHandle,
                                    uint16_t   attribIndex,
                                    uint16_t   attribLength,
                                    uint16_t   offset
                                   );

/**
 * @brief  Respond to a blueAPI_EventGATTAttributeUpdateStatusInd.
 *
 *  The APP uses this command to respond to a
 *  GATTAttributeUpdateStatusInd received from the BT Stack.
 *
 * @param serviceHandle  Handle of service that contains the attribute to update.
 * @param requestHandle  Request handle as defined in attribute update request.
 * @param attribIndex  Index of attribute in service descriptor array.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_GATTAttributeUpdateStatusConf(void *     serviceHandle,
        void *     requestHandle,
        uint16_t   attribIndex
                                          );

/**
 * @brief  Respond to a blueAPI_EventGATTAttributeReadInd.
 *
 *  The APP uses this command to respond to a
 *  GATTAttributeReadInd received from the BT Stack.
 *
 * @param pBuffer  Point to the buffer allocated by APP.
 *                              The buffer stores the attribute value.
 * @param local_MDL_ID  Local link ID for this link
 * @param serviceHandle  Handle of service that contains the attribute to read.
 * @param cause  Indicates the result of the transaction.
 * @param subCause  More detailed result information for lower protocol layers.
 * @param attribIndex  Index of attribute in service descriptor array.
 * @param attribLength  Attribute value/data length.
 * @param offset  Offset of attribute value in pBuffer.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_GATTAttributeReadConf(void *          pBuffer,
                                   uint16_t        local_MDL_ID,
                                   void *          serviceHandle,
                                   TBlueAPI_Cause cause,
                                   uint16_t        subCause,
                                   uint16_t        attribIndex,
                                   uint16_t        attribLength,
                                   uint16_t        offset
                                  );
/**
 * @brief  Respond to a blueAPI_EventGATTAttributeWriteInd.
 *
 * The APP uses this command to respond to a
 *  GATTAttributeWriteInd received from the BT Stack.
 *
 * @param local_MDL_ID  Local link ID for this link.
 * @param serviceHandle  Handle of service that contains the attribute to write.
 * @param cause  Indicates the result of the transaction. .
 * @param subCause  More detailed result information for lower protocol layers.
 * @param attribIndex  Index of attribute in service descriptor array.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_GATTAttributeWriteConf(uint16_t        local_MDL_ID,
                                    void *         serviceHandle,
                                    TBlueAPI_Cause cause,
                                    uint16_t        subCause,
                                    uint16_t        attribIndex
                                   );
/**
 * @brief  Respond to a blueAPI_EventGATTAttributePrepareWriteInd.
 *
 * The APP uses this command to respond to a
 *  GATTAttributePrepareWriteInd received from the BT Stack.
 *
 * @param pBuffer  Point to the buffer allocated by APP.
 *                              The buffer stores the part attribute value.
 * @param local_MDL_ID  Local link ID for this link.
 * @param serviceHandle  Handle of service that contains the attribute to write.
 * @param cause  Indicates the result of the transaction. .
 * @param subCause  More detailed result information for lower protocol layers.
 * @param attribIndex  Index of attribute in service descriptor array.
 *                     The attribIndex shall be set to the same value as in the corresponding blueAPI_EventGATTAttributePrepareWriteInd
 * @param attribLength  Part Attribute value/data length.
 *                      The attribLength shall be set to the same value as in the corresponding blueAPI_EventGATTAttributePrepareWriteInd
 * @param offset  Offset of attribute value in pBuffer.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_GATTAttributePrepareWriteConf(void *  pBuffer,
        uint16_t        local_MDL_ID,
        void *          serviceHandle,
        TBlueAPI_Cause cause,
        uint16_t        subCause,
        uint16_t        attribIndex,
        uint16_t        attribLength,
        uint16_t        offset
                                          );
/**
 * @brief  Respond to a blueAPI_EventGATTAttributeExecuteWriteInd.
 *
 * The APP uses this command to respond to a
 *  GATTAttributeExecuteWriteInd received from the BT Stack.
 *
 * @param local_MDL_ID  Local link ID for this link.
 * @param cause  Indicates the result of the transaction. .
 * @param subCause  More detailed result information for lower protocol layers.
 * @param handle   The attribute handle of the attribute from the prepare queue that
 *                             caused this application error. if cause is success ,handle set to zero.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_GATTAttributeExecuteWriteConf(uint16_t        local_MDL_ID,
        TBlueAPI_Cause  cause,
        uint16_t        subCause,
        uint16_t        handle
                                          );
/** @} End of GATT_Server_API */

/** @defgroup GATT_Client_API GATT Client
  * @brief GATT Client API
  * @{
  */
/**
 * @brief  Perform discovery procedures.
 *
 *  The APP uses this command to perform the various discovery procedures
 *
 *    Discover All Primary Services
 *
 *    Discover Primary Service by Service UUID
 *
 *    Find Included Services
 *
 *    Discover All Characteristics of a Service
 *
 *    Discover All Characteristic Descriptors
 *
 * @param local_MDL_ID  Local link ID for this link.
 * @param discoveryType  Type of discovery.
 * @param startHandle  Start handle of range to be searched.
 * @param endHandle  End handle of range to be searched.
 * @param UUID16  UUID to search for.
 *                             discoveryType = blueAPI_GATTDiscoveryServiceByUUID:
 *                             If set to zero, UUID128 is searched for instead.
 * @param pUUID128  128 bit UUID to search for if UUID16 is set to zero.
 *                                 discoveryType = blueAPI_GATTDiscoveryServiceByUUID:
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
 * @note  The "Discover Characteristic by UUID" functionality is provided
 *            through a GATTAttributeReadReq with readType = blueAPI_GATTReadTypeByUUID.
*/
bool blueAPI_GATTDiscoveryReq(uint16_t        local_MDL_ID,
                              TBlueAPI_GATTDiscoveryType   discoveryType,
                              uint16_t        startHandle,
                              uint16_t        endHandle,
                              uint16_t        UUID16,
                              uint8_t  *      pUUID128
                             );


/**
 * @brief  Respond to a blueAPI_EventGATTDiscoveryInd
 *
 * The APP uses this command to continue or terminate the discovery procedure.
 * If not terminated the continuation is based on the largest attribute handle value
 * received in the elemenList parameter of the GATTDiscoveryInd + 1.
 *
 * @param local_MDL_ID  Local link ID for this link.
 * @param discoveryType  Type of discovery.
 * @param startHandle If startHandle=0 and endHandle<>0 the search continues
 *                                  with the largest handle value received in the elemenList parameter of the GATTDiscoveryInd + 1,
 *                                  otherwise the search continues with the startHandle value.
 * @param endHandle  End handle of range to be searched.
 *                                  startHandle=0 and endHandle=0 the search is terminated.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_GATTDiscoveryConf(uint16_t        local_MDL_ID,
                               TBlueAPI_GATTDiscoveryType   discoveryType,
                               uint16_t        startHandle,
                               uint16_t        endHandle
                              );

/**
 * @brief  Perform read procedure.
 *
 *  The APP uses this command to perform the following read procedures
 *
 *  Read Characteristic Value
 *
 *  Read Using Characteristic UUID
 *
 *  Read Long Characteristic Value
 *
 * @param local_MDL_ID  Local link ID for this link.
 * @param readType  Type of read.
 * @param readOffset  The first readOffset bytes of the attribute value are skipped before reading starts.
 *                                 readType = blueAPI_GATTReadTypeBasic
 * @param startHandle  readType = blueAPI_GATTReadTypeBasic: Handle of attribute to be read.
 *                                   readType = blueAPI_GATTReadTypeByUUID: Start of handle range to be searched for matching UUID.
 * @param endHandle  readType = blueAPI_GATTReadTypeByUUID:
 *                                  End of handle range to be searched for matching UUID.
 * @param UUID16  readType = blueAPI_GATTReadTypeByUUID:
 *                              UUID to search for. If set to zero, UUID128 is searched for instead.
 * @param pUUID128  readType = blueAPI_GATTReadTypeByUUID:
 *                                  128 bit UUID to search for if UUID16 is set to zero.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_GATTAttributeReadReq(uint16_t        local_MDL_ID,
                                  TBlueAPI_GATTReadType   readType,
                                  uint16_t        readOffset,
                                  uint16_t        startHandle,
                                  uint16_t        endHandle,
                                  uint16_t        UUID16,
                                  uint8_t  *      pUUID128
                                 );

/**
 * @brief  Perform read multiple procedure.
 *
 *  The APP uses this command to read two or more values of a set of attributes.
 *
 * @param local_MDL_ID  Local link ID for this link.
 * @param nbrOfHandles  The number of handles. range: 2-11
 * @param pHandles  The attribute handles to read.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_GATTAttributeReadMultipleReq(uint16_t        local_MDL_ID,
        uint16_t        nbrOfHandles,
        uint16_t *      pHandles
                                         );

/**
 * @brief  Perform write procedure.
 *
 *  The APP uses this command to perform the following write procedures
 *
 *    Write Without Response
 *
 *    Write Characteristic Value
 *
 * @param pBuffer  Point to the buffer allocated by APP.
 *                              The buffer stores the attribute value.
 * @param local_MDL_ID  Local link ID for this link.
 * @param writeType  Type of write.
 * @param attribHandle  Attribute handle.
 * @param attribLength  Length of data to be written.
 * @param offset  Offset of first attribute data byte in pBuffer.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_GATTAttributeWriteReq(void *          pBuffer,
                                   uint16_t        local_MDL_ID,
                                   TBlueAPI_GATTWriteType   writeType,
                                   uint16_t        attribHandle,
                                   uint16_t        attribLength,
                                   uint16_t        offset
                                  );

/**
 * @brief  Perform prepare write procedure.
 *
 *  The APP uses this command to request the server to prepare to write the value of an attribute.
 *
 * @param pBuffer  Point to the buffer allocated by APP.
 *                              The buffer stores the part attribute value.
 * @param local_MDL_ID  Local link ID for this link.
 * @param attribHandle  Attribute handle.
 * @param attribLength  Length of data to be written.
 * @param writeOffset  The offset of the first octet to be written.
 * @param offset  Offset of first attribute data byte in pBuffer.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_GATTAttributePrepareWriteReq(void *          pBuffer,
        uint16_t        local_MDL_ID,
        uint16_t        attribHandle,
        uint16_t        attribLength,
        uint16_t        writeOffset,
        uint16_t        offset
                                         );

/**
 * @brief  Perform execute write procedure.
 *
 *  The APP uses this command to to request the server to write or cancel the
 *  write of all the prepared values currently held in the prepare queue from this client.
 *
 * @param local_MDL_ID  Local link ID for this link.
 * @param flags  0x00 Cancel all prepared writes
 *                       0x01 Immediately write all pending prepared values
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_GATTAttributeExecuteWriteReq(
    uint16_t        local_MDL_ID,
    uint8_t         flags
);

/**
 * @brief  Respond to a blueAPI_EventGATTAttributeInd.
 *
 *  The APP uses this command to respond to a GATTAttributeInd.
 *
 * @param local_MDL_ID  Local link ID for this link.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_GATTAttributeConf(uint16_t        local_MDL_ID
                              );
/** @} End of GATT_Client_API */

/** @defgroup Configuration_API System Activation, Configuration and Status
  * @brief System Activation, Configuration and Status API
  * @{
  */

/**
 * @brief  Configure Device Name characterstic of GAP service.
 *
 * This command can be used by the APP to configure Device Name characterstic of GAP service.
 *
 * @param deviceName  UTF-8 formatted zero terminated device name of the local device.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_DeviceConfigDeviceNameSetReq(uint8_t *               deviceName
                                         );

/**
 * @brief  Get the value of Device Name characterstic of GAP service.
 *
 * This command can be used by the APP to get the value of Device Name characterstic of GAP service.
 *
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_DeviceConfigDeviceNameGetReq(void);


/**
 * @brief  Configure Appearance characterstic of GAP service.
 *
 *  This command can be used by the APP to configure Appearance characterstic of GAP service.
 *
 * @param appearance  The External appearance of this device.@ref GATT_appearance_definitions
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_DeviceConfigAppearanceSetReq(uint16_t                appearance
                                         );

/**
 * @brief  Get the value of Appearance characterstic of GAP service.
 *
 * This command can be used by the APP to get the value of Appearance characterstic of GAP service.
 *
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_DeviceConfigAppearanceGetReq(void);


/**
 * @brief  Configure Peripheral Preferred Connection Parameters characterstic.
 *
 *  This command can be used by the APP to configure
 *  Peripheral Preferred Connection Parameters characterstic of GAP service.
 *
 * @param connIntervalMin  Minimum value for the connection event interval.
 *                           Value range: 0x0006 - 0x0C80 (7.5ms - 4000ms, 1.25ms/step).
 * @param connIntervalMax  Maximum value for the connection event interval.
 *                           Value range: 0x0006 - 0x0C80 (7.5ms - 4000ms, 1.25ms/step).
 * @param slaveLatency  Slave latency for the connection in number of connection events.
 *                                      Range: 0x0000 to 0x01F3
 * @param supervisionTimeout  Supervision timeout for the LE Link.
 *                           Value range: 0x000A - 0x0C80 (100ms - 32000ms, 10ms/step)
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_DeviceConfigPerPrefConnParamSetReq(uint16_t               connIntervalMin,
        uint16_t               connIntervalMax,
        uint16_t               slaveLatency,
        uint16_t               supervisionTimeout
                                               );
/**
 * @brief  Get the value of Peripheral Preferred Connection Parameters characterstic of GAP service.
 *
 * This command can be used by the APP to get the value of Peripheral Preferred Connection Parameters characterstic of GAP service.
 *
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_DeviceConfigPerPrefConnParamGetReq(void);

/**
 * @brief  Configure fix pass entry key.
 *
 *  This command can be used by the APP to configure fix pass entry key of the local device.
 *
 * @param leFixedDisplayValue  Use this value when acting as SMP Display on a BLE link.
 *        Valid range: 0 - 999999, default: 0x00
 *        The leFixedDisplayValue must be ORed with the constant BLUE_API_USE_LE_FIXED_DISPLAY_VALUE to take effect.
 *        The value 0x00 disables the feature and a random generated number will be used.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_DeviceConfigSecuritySetReq(uint32_t                leFixedDisplayValue
                                       );

/**
 * @brief  Configure the bond store of the local device.
 *
 *  This command can be used by the APP to configure the bond store of the local device.
 *  The default mode is blueAPI_StoreBondModeExtStore.
 *  If user wants to change the store mode, user needs to change efuse setting to open stack storage at first.
 *
 *
 * @param storeBondMode  Method chosen for storing bonding information.
 * @param storeBondSize  Number of authentication results that shall be stored BlueAPI internally.
 *                                    The maximum value that is allowed to be used for this parameter is implementation specific.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_DeviceConfigStoreSetReq(TBlueAPI_StoreBondModes storeBondMode,
                                     uint8_t                 storeBondSize
                                    );
/** @} End of Configuration_API */



/****************************************************************************
 ******************   Start of Secutity Management part *********************
 ***************************************************************************/
/** @defgroup Security_Management_API Security Management
  * @brief Security Management API
  * @{
  */

/**
 * @brief   Initiate a authentication.
 *
 *  This command is used by the APP to initiate an authentication on a given GATT connection.
 *
 * @param local_MDL_ID  Local link ID for this link.
 * @param requirements  Logically OR-ed requirements to the security.
 *                                  0x00--requires nothing
 *                                  0x01--requires authenticated (mitm secure) linkkey
 *                                  0x10--do not use stored key
 * @param minKeySize  Minimum required key size for this connection.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_GATTSecurityReq(uint16_t        local_MDL_ID,
                             uint16_t        requirements,
                             uint8_t         minKeySize
                            );

/** @} End of Security_Management_API */

/** @addtogroup GATT_Server_API
  * @{
  */
/**
 * @brief  Respond to a blueAPI_EventGATTServerStoreInd.
 *
 *  The APP uses this command to respond to a
 *  GATTServerStoreInd received from the BT Stack.
 *
 * @param opCode  Requested operation.
 * @param remote_BD  Bluetooth device address of remote device.
 * @param remote_BD_Type  Type of remote BD address.
 * @param restartHandle  Used to request multiple data chunks.
 * @param dataLength  Length of supplied data.
 * @param data  Up to 32 bytes of data.
 * @param cause  Indicates the result of the transaction.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_GATTServerStoreConf(TBlueAPI_GATTStoreOpCode opCode,
                                 uint8_t *                remote_BD,
                                 TBlueAPI_RemoteBDType    remote_BD_Type,
                                 uint16_t                 restartHandle,
                                 uint8_t                  dataLength,
                                 uint8_t *                data,
                                 TBlueAPI_Cause           cause
                                );
/** @} End of GATT_Server_API */

/** @addtogroup Security_Management_API
  * @{
  */

/**
 * @brief  Set the pairable mode of the local device.
 *
 *  This command shall be used by the APP to set the pairable mode of the local device.
 *
 * @param enablePairableMode  If this parameter is set to TRUE, pairable mode is enabled,
 *                                                otherwise pairable mode is disabled.
 * @param AuthRequirements  This parameter defines the authentication requirements
 *                                              for authentication while the local device is in pairable mode.
 * @param IOCapabilities  Defines the input/output capabilities that can be used for authentication.
 * @param remoteOOBDataPresent  If this parameter is set to TRUE,
 *                                                     remote OOB data is present and can be used for authentication,
 *                                                     otherwise OOB functionality is disabled.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_PairableModeSetReq(bool            enablePairableMode,
                                TBlueAPI_AuthRequirements  AuthRequirements,
                                TBlueAPI_IOCapabilities    IOCapabilities,
                                bool            remoteOOBDataPresent
                               );

/**
 * @brief  Respond to blueAPI_EventAuthResultRequestInd.
 *
 *  This command shall be used by the APP to respond to an AuthResultRequestInd of the BT Stack.
 *  In case the requested bonding information can be provided by the APP it shall be returned with this message.
 *
 * @param remote_BD  Bluetooth device address of remote device.
 * @param remote_BD_Type  Type of remote BD address.
 * @param linkKeyLength  Link key length.
 * @param linkKey  Link key, up to 28 bytes.
 * @param keyType  Key type of link key.
 * @param restartHandle Value != 0x0000 when returning multiple keys.
 * @param cause  Indicates the result of the transaction.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_AuthResultRequestConf(uint8_t  *            remote_BD,
                                   TBlueAPI_RemoteBDType remote_BD_Type,
                                   uint8_t               linkKeyLength,
                                   uint8_t  *            linkKey,
                                   TBlueAPI_LinkKeyType  keyType,
                                   uint16_t              restartHandle,
                                   TBlueAPI_Cause        cause
                                  );


/**
 * @brief  Respond to blueAPI_EventUserPasskeyReqInd.
 *
 *  With this message the APP shall respond to a SMP driven
 *  UserPasskeyReqInd as soon as User interaction is performed.
 *
 * @param remote_BD  Bluetooth device address of remote device.
 * @param cause  Indicates the result of the operation..
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_UserPasskeyReqConf( uint8_t  *       remote_BD,
                                 TBlueAPI_Cause   cause
                               );

/**
 * @brief  Transfer the result of the Keyboard input.
 *
 *  If a SMP procedure is completed that requires Keyboard functionality from the local,
 *  and display functionality from the remote device the APP shall send this message to the BT Stack
 *  to transfer the result of the Keyboard input and request completion of the SMP procedures.
 *
 * @param remote_BD  Bluetooth device address of remote device.
 * @param passKey  Result of Keyboard input.
 * @param cause  Indicates the result of the operation.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_UserPasskeyReqReplyReq(uint8_t  *       remote_BD,
                                    uint32_t         passKey,
                                    TBlueAPI_Cause   cause
                                   );


/**
 * @brief  Respond to blueAPI_EventRemoteOOBDataReqInd.
 *
 *  With this message the APP shall respond to a SMP driven RemoteOOBDataReqInd message of the BT Stack.
 *
 * @param remote_BD  Bluetooth device address of remote device.
 * @param pC  Hash C value.
 * @param cause  Indicates the result of the operation.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_RemoteOOBDataReqConf(uint8_t  *      remote_BD,
                                  uint8_t  *      pC,
                                  TBlueAPI_Cause  cause
                                 );



/**
 * @brief  Respond to an blueAPI_EventAuthResultInd.
 *
 *  This command is used by the APP to respond to an AuthResultInd of the BT Stack.
 *
 * @param remote_BD  Bluetooth device address of remote device.
 * @param remote_BD_Type  Type of remote BD address.
 * @param cause  Indicates the result of the transaction.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_AuthResultConf(uint8_t  *            remote_BD,
                            TBlueAPI_RemoteBDType remote_BD_Type,
                            TBlueAPI_Cause        cause
                           );
/** @} End of Security_Management_API */


/****************************************************************************
 ******************   End of Secutity Management part ***********************
 ***************************************************************************/

/** @defgroup Bluetooth_LE_generic_API Bluetooth LE generic
  * @brief Bluetooth LE generic API
  * @{
  */

/**
 * @brief  Control LE Advertising.
 *
 *  This command is used by the APP to control Bluetooth Low Energy (LE) Advertising.
 *
 * @param advMode  Advertising mode.
 *                            blueAPI_LEAdvModeDirectedHighDuty is used to enable high duty cycle directed advertising.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_LEAdvertiseReq(TBlueAPI_LEAdvMode      advMode);

/**
 * @brief  Set parameters for LE Advertising.
 *
 *  This command is used by the APP to set parameters for Bluetooth Low Energy Advertising.
 *
 * @param advType  Advertising Type.
 * @param filterScanReq  Use whitelist to filter scan requests.
 * @param filterConnectReq  Use whitelist to filter connect requests.
 * @param minAdvInterval  Minimum advertising interval for undirected and low duty cycle directed advertising.
 *                                         Value range: 0x0020 - 0x4000 (20ms - 10240ms, 0.625ms/step)
 * @param maxAdvInterval  Maximum advertising interval for undirected and low duty cycle directed advertising.
 *                                         Value range: 0x0020 - 0x4000 (20ms - 10240ms 0.625ms/step)
 * @param local_BD_type  Local device address type .
 * @param remote_BD  Bluetooth device address (for low duty cycle directed advertising and high duty cycle directed advertising only).
 * @param remote_BD_type  Bluetooth device address type (for low duty cycle directed advertising and high duty cycle directed advertising only).
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_LEAdvertiseParameterSetReq(TBlueAPI_LEAdvType      advType,
                                        TBlueAPI_LEFilterPolicy filterScanReq,
                                        TBlueAPI_LEFilterPolicy filterConnectReq,
                                        uint16_t                minAdvInterval,
                                        uint16_t                maxAdvInterval,
                                        TBlueAPI_LocalBDType    local_BD_type,
                                        uint8_t *               remote_BD,
                                        TBlueAPI_RemoteBDType   remote_BD_type);

/**
 * @brief  Set advertising data or scan response data.
 *
 *  This command is used by the APP to set advertising data
 *  or scan response data for Bluetooth Low Energy Advertising.
 *
 * @param dataType  Type of data to be set.
 * @param dataLength  Value range: 0x00 - 0x1F.
 * @param data  Point to Advertising Data / Scan Response Data.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_LEAdvertiseDataSetReq(TBlueAPI_LEDataType  dataType,
                                   uint8_t              dataLength,
                                   uint8_t  *           data);

/**
 * @brief  Control Bluetooth Low Energy Scan.
 *
 *  This command is used by the APP to enable Bluetooth Low Energy Scan.
 *
 * @param scanMode  Scan Mode (disabled/passive/active).
 * @param scanInterval  Value range: 0x0004 - 0x4000 (2.5ms - 10240ms, 0.625ms/step).
 * @param scanWindow  Value range: 0x0004 - 0x4000 (2.5ms - 10240ms, 0.625ms/step).
 * @param filterPolicy  Use whitelist to filter advertising events.
 * @param local_BD_type  Local device address type .
 * @param filterDuplicates  Filter duplicated advertising events.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_LEScanReq(TBlueAPI_LEScanMode      scanMode,
                       uint16_t                 scanInterval,
                       uint16_t                 scanWindow,
                       TBlueAPI_LEFilterPolicy  filterPolicy,
                       TBlueAPI_LocalBDType     local_BD_Type,
                       uint8_t                  filterDuplicates);

/**
 * @brief  Modify the BD address whitelis.
 *
 *  This command is used by the APP to modify the BD address whitelist.
 *
 * @param operation  Whitelist operation requested (clear/add/remove).
 * @param remote_BD  Bluetooth device address of remote device.
 * @param remote_BD_type  Bluetooth device address type.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_LEModifyWhitelistReq(TBlueAPI_LEWhitelistOp  operation,
                                  uint8_t  *              remote_BD,
                                  TBlueAPI_RemoteBDType   remote_BD_type);

/**
 * @brief  Initiate a Connection Parameter Update.
 *
 *  This command is used by the APP to initiate a Connection Parameter Update on an established LE link.
 *
 * @param local_MDL_ID  Local link ID for this link.
 * @param connIntervalMin  Value range: 0x0006 - 0x0C80 (7.5ms - 4000ms, 1.25ms/step).
 * @param connIntervalMax  Value range: 0x0006 - 0x0C80 (7.5ms - 4000ms, 1.25ms/step).
 * @param connLatency  Value range: 0x0000 - 0x01F3.
 * @param supervisionTimeout  Value range: 0x000A - 0x0C80 (100ms - 32000ms, 10ms/step).
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_LEConnectionUpdateReq(uint16_t        local_MDL_ID,
                                   uint16_t        connIntervalMin,
                                   uint16_t        connIntervalMax,
                                   uint16_t        connLatency,
                                   uint16_t        supervisionTimeout);

/**
 * @brief  Respond to an blueAPI_EventLEConnectionUpdateInd.
 *
 *  This command is used by the APP to respond to an LEConnectionUpdateInd of the BT Stack.
 *
 * @param local_MDL_ID  Local link ID for this link.
 * @param cause  Indicates the result of the transaction.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_LEConnectionUpdateConf(uint16_t        local_MDL_ID,
                                    TBlueAPI_Cause  cause);

/**
 * @brief  Set random address.
 *
 *  This command is used by the APP to set  random device address used by local device.
 *
 * @param random_BD  Random device address used by local device .
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_SetRandomAddressReq(uint8_t *   random_BD);

/**
 * @brief  Set Ble TX Power
 *
 *  This command can be used by the APP to the radio's transmit power used by local device.
 *
 * @param tx_power_index  Tx power used by local device.Range: 0,6,7.  1-5 is reserved.
 *                                       @arg 0 -> -16dBm
 *                                       @arg 6 ->   0dBm
 *                                       @arg 7 ->   3dBm
 *
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/

bool blueAPI_SetBleTxPowerReq(uint8_t tx_power_index);

/** @} End of Bluetooth_LE_generic_API */

/** @addtogroup Buffer_Management_API
  * @{
  */

/**
 * @brief  Initialize the data ram pool.
 *
 * @param pBufferDataOff  Pointer to the memory allocated by APP to use as data ram pool.
 * @param size  The size of the memory.
 * @return The results of initialize the data ram pool.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_DataRamPoolInit(
                                uint8_t* pBufferDataOff,
                                uint16_t size
                                );

/**
 * @brief  Create the data ram pool.
 *
 * @param poolElementSize  Pool element size.
 * @param poolElementCount  Pool element count.
 * @return PoolId that use to get buffer from this pool.
 * @retval  PoolId = 0xFFFF It means that pool creates failed.
*/
uint16_t blueAPI_DataRamPoolCreate(uint16_t poolElementSize,
                               uint16_t poolElementCount
                              );

/**
 * @brief  Create extended data ram pool.
 *
 * @param poolID  The return value of the function" blueAPI_DataRamPoolCreate".
 * @param poolElementSize  Extended pool element size.
 * @param poolElementCount  Extended pool element count.
 * @return The results of creating the extended pool.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_DataRamPoolExtend(uint16_t poolID,
                           uint16_t poolElementSize,
                           uint16_t poolElementCount
                          );

/**
 * @brief  Get the buffer from the data ram pool.
 *
 * @param AppPoolID  poolID to allocate the buffer from.The return value of the function" blueAPI_DataRamPoolCreate".
 * @param len  The buffer length to allocate.
 * @return Pointer to memory allocated.
 *              In case of error NULL is returned
*/
void * blueAPI_DataRamPoolBufferGet(uint16_t AppPoolID,
                              uint16_t len
                             );

/**
 * @brief  Release a memory buffer to data ram pool.
 *
 * @param AppPoolID  poolID which the memory allocated from.The return value of the function" blueAPI_DataRamPoolCreate".
 * @param pBuffer  Pointer to memory that need to release (address of begin of message header).
 * @return none
*/
void   blueAPI_DataRamPoolBufferRelease(uint16_t AppPoolID,
                                                            void * pBuffer
                                                            );

/**
 * @brief  Create the buffer ram pool.
 *
 * Note: Used by ADC only, other applications can't use this API.
 *
 * @param poolElementSize  Pool element size.
 * @param poolElementCount  Pool element count.
 * @return PoolId that use to get buffer from this pool.
 * @retval  PoolId = 0xFFFF It means that pool creates failed.
*/
uint16_t blueAPI_BufRamPoolCreate(
                    uint16_t poolElementSize, 
                    uint16_t poolElementCount
                    );
///@cond
void blueAPI_BufRamPoolReset(uint16_t AppPoolID);

///@endcond
/**
 * @brief  Get the buffer from the buffer ram pool.
 *
 * Note: Used by ADC only, other applications can't use this API.
 *
 * @param AppPoolID  poolID to allocate the buffer from. The return value of the function" blueAPI_BufRamPoolCreate".
 * @param len  The buffer length to allocate.
 * @return Pointer to memory allocated.
 *              In case of error NULL is returned
*/
void * blueAPI_BufRamPoolBufferGet(uint16_t AppPoolID, uint16_t len);

/**
 * @brief  Release a memory buffer to buffer ram pool.
 *
 * Note: Used by ADC only, other applications can't use this API.
 *
 * @param AppPoolID  poolID which the memory allocated from. The return value of the function" blueAPI_BufRamPoolCreate".
 * @param pBuffer  Pointer to memory that need to release (address of begin of message header).
 * @return none
*/
void blueAPI_BufRamPoolBufferRelease(uint16_t AppPoolID, void * pBuffer);

/** @} End of Buffer_Management_API */

///@cond
bool blueAPI_VendorSetVoiceParaReq(
    uint16_t        local_MDL_ID,
    bool bEnable,
    uint16_t length,
    void* pServiceHandle,
    uint16_t attributeIndex,
    uint8_t Report_Id,
    uint8_t extLen,
    uint8_t *pExtData);
///@endcond

/****************************************************************************
 ******************   Start of BT4.1 Credit based channel part *********************
 ***************************************************************************/
/** @defgroup BT41_API LE credit based flow control mode
 * @brief LE credit based flow control mode API
 * @{
 */

/**
 * @brief  Initiate a LE credit based connection.
 *
 *  This command is used by the APP to create a LE credit based connection.
 *
 * @param local_MDL_ID  Local link ID for this link.
 * @param le_psm  LE Protocol/Service Multiplexers.
 * @param mtu  Maximum Transmission Unit. range:>=23
 * @param mps  Maximum PDU Size. Value range: 23 - 65533.
 * @param initialCredits  Initial Credits, Value range: 0 - 65535.
 * @param creditsIncrease  Value range: 0 - initialCredits.
 *                                      0- stack will not send LE Flow Control Credit packet automatical,app shall use blueAPI_SendLEFlowControlCreditReq to send.
 *                                      >0 - stack will send LE Flow Control Credit packet automatical when credit reduce to creditsIncrease.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_CreateLEDataChannelReq(uint16_t     local_MDL_ID,
                                    uint16_t    le_psm,
                                    uint16_t    mtu,
                                    uint16_t    mps,
                                    uint16_t    initialCredits,
                                    uint16_t    creditsIncrease);

/**
 * @brief  Respond to a blueAPI_EventCreateLEDataChannelInd.
 *
 * This command will be used by the APP respond to a CreateLEDataChannelInd.
 *
 * @param local_MDL_ID  Local link ID for this link.
 * @param channel   channel id to be created.
 * @param mtu  Maximum Transmission Unit. range:>=23
 * @param mps  Maximum PDU Size. Value range: 23 - 65533.
 * @param initialCredits  Initial Credits, Value range: 0 - 65535.
 * @param creditsIncrease  Value range: 0 - initialCredits.
 *                                      0- stack will not send LE Flow Control Credit packet automatical,app shall use blueAPI_SendLEFlowControlCreditReq to send.
 *                                      >0 - stack will send LE Flow Control Credit packet automatical when credit reduce to creditsIncrease.
 * @param cause  Indicates the result of the CreateLEDataChannel transaction.
 *                           blueAPI_CauseAccept -- Accept LE credit based connection
 *                           blueAPI_CauseReject -- Reject LE credit based connection
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_CreateLEDataChannelConf(uint16_t     local_MDL_ID,
                                     uint16_t    channel,
                                     uint16_t    mtu,
                                     uint16_t    mps,
                                     uint16_t    initialCredits,
                                     uint16_t    creditsIncrease,
                                     TBlueAPI_Cause  cause);

/**
 * @brief  Disconnect LE credit based connection.
 *
 *  This command shall be used by the APP to request a disconnection of
 *  an established LE credit based connection.
 *
 * @param local_MDL_ID  Local link ID for this link.
 * @param channel  Channel ID to be disconnected.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_DisconnectLEDataChannelReq(uint16_t     local_MDL_ID,
                                        uint16_t           channel);

/**
 * @brief  Respond to a blueAPI_EventDisconnectLEDataChannelInd.
 *
 * This command will be used by the APP to respond
 *  a DisconnectLEDataChannelInd of the BT Stack.
 *
 * @param local_MDL_ID  Local link ID for this link.
 * @param channel  Channel ID to be disconnected.
 * @param cause  Indicates the result of disconnection LE data channel transaction.
 *                           blueAPI_CauseAccept -- Accept
 *                           blueAPI_CauseReject -- Reject
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_DisconnectLEDataChannelConf(uint16_t     local_MDL_ID,
        uint16_t        channel,
        TBlueAPI_Cause  cause);

/**
 * @brief  Send a LE Flow Control Credit packet .
 *
 * This command will be send a LE Flow Control Credit packet when it is capable of
 * receive additional LE-frames
 *
 * @param local_MDL_ID  Local link ID for this link.
 * @param channel  Channel ID.
 * @param credits  The credit value field represents number of credits the receiving device can increment.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_SendLEFlowControlCreditReq(uint16_t     local_MDL_ID,
                                        uint16_t    channel,
                                        uint16_t    credits);

/**
 * @brief  Send data via LE credit based connection.
 *
 * This command will be send data via LE credit based connection.
 *
 * @param pBuffer  Point to the buffer allocated by APP. The buffer stores the value.
 * @param local_MDL_ID  Local link ID for this link.
 * @param channel  Channel ID to be sent data.
 * @param valueLength  data length.
 * @param offset  Offset of value in pBuffer.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_LEDataReq(void *          pBuffer,
                       uint16_t     local_MDL_ID,
                       uint16_t     channel,
                       uint16_t     valueLength,
                       uint16_t     offset);

/**
 * @brief  Respond to an blueAPI_EventLEDataInd.
 *
 *  This command is used by the APP to respond to an LEDataInd of the BT Stack.
 *
 * @param local_MDL_ID  Local link ID for this link.
 * @param channel  Channel ID.
 * @param cause  Indicates the result of the transaction.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_LEDataConf(uint16_t     local_MDL_ID,
                        uint16_t        channel,
                        TBlueAPI_Cause  cause);
/**
 * @brief  Set LE PSM's security level.
 *
 *  This command is used to set LE PSM's security level.
 *
 * @param le_psm  LE Protocol/Service Multiplexer.
 * @param active  FALSE - delete TRUE- add.
 * @param secMode  Security level.
 * @param keySize  Minimum required key size.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_LEPsmSecuritySetReq(uint16_t   le_psm,
                                 bool       active,
                                 TBlueAPI_LESecurityMode      secMode,
                                 uint8_t  keySize
                                );
/** @} End of BT41_API */

/****************************************************************************
 ******************   End of BT4.1 Credit based channel part ***********************
 ***************************************************************************/
/** @addtogroup Bluetooth_LE_generic_API
  * @{
  */
/**
 * @brief  Set data length.
 *
 *  This command can be used by the APP to suggest maximum transmission packet size
 *  and maximum packet transmission time to be used for a given connection.
 *  Note: This is BT4.2 feature. This feature is default closed. If APP wants to use this feature,
 *        User needs to use MP tool modify Bee efuse setting to open this feature.
 *
 * @param local_MDL_ID  Local link ID for this link.
 * @param txOctets  Preferred maximum number of payload octets that the local Controller
 *                  should include in a single Link Layer Data Channel PDU. Range: 0x001B-0x00FB
 * @param txTime  Preferred maximum number of microseconds that the local Controller
 *                should use to transmit a single Link Layer Data Channel PDU. Range: 0x0148-0x0848
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
bool blueAPI_SetDataLengthReq(uint16_t    local_MDL_ID,
                                uint16_t    txOctets,
                                uint16_t    txTime
                                );

///@cond
bool blueAPI_UserDefined(void *          pBuffer               
                      );
///@endcond

/**
 * @brief  Set LE Advertise channel map.
 *
 *  This command can be used by the APP to the set LE Advertise channel map used by local device.
 *  Note: This parameter is not come into effect immediately.
 *        When APP uses commond blueAPI_LEAdvertiseParameterSetReq to set advertise parameter, BT stack will use the parameter to set advertising channel map.
 *
 * @param AdvertisingChannelMap  Indicates the advertising channels that shall be used when transmitting advertising packets.
 * @return Send result.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
void blueAPI_SetLEAdvertiseChannelMap(uint8_t AdvertisingChannelMap);
/** @} End of Bluetooth_LE_generic_API */

/** @} End of Downstream_API_Functions */

/** @} End of RTK_BlueAPI */
#ifdef __cplusplus
}
#endif

#endif
