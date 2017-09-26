/**
****************************************************************************
* @file mxdTxRxService.h
* @brief
* @author maxscend\yanping.wang
* @version Initial Draft
* @date 2016/11/15
* @history
* @note
* detailed description

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/
/** @defgroup  profile  serverTxRx
* @{
*/
#ifndef __MXDTXRXSERVICE_H__
#define __MXDTXRXSERVICE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define TXRX_SVR_DEBUG_BASE LOG_PROFILE_DEBUG_BASE

extern uint16 txrx_server_receive(uint8 *data, uint16 length);
extern uint8 txrx_server_transfer(uint8 *data, uint16 length, uint8 source);;
extern uint16  trx_get_attrLenCB(uint16 attribute_handle);
extern uint16  trx_read_attrCB(uint16 attribute_handle, uint16 offset, uint8  * buffer, uint16  buffer_size);
extern uint32 trx_write_attrCB(uint16 attribute_handle, uint16  transaction_mode, uint16 offset, uint8  *buffer, uint16 buffer_size);
extern uint32 trx_init_service( void );
    
extern uint8 rxClientCharCfg;
extern uint16 txrx_server_hdl[3];

typedef enum TXRX_SRV_HANDLE
{
    TX_WRITE            = 0,
    RX_NOTIFY           = 1,
    RX_NOTIFY_CONFIG    = 2,
} TXRX_SRV_HANDLE_ENUM;



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __MXDTXRXSERVICE_H__ */
/** @} */ // 模块结尾,自行拷贝到末尾


