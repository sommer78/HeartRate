/**
****************************************************************************
* @file pts_service.h
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
/** @defgroup  profile  ptsservice
* @{
*/
#ifndef __PTSSERVICE_H__
#define __PTSSERVICE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

extern uint16  pts_read_attrCB(uint16 attribute_handle, uint16 offset, uint8  * buffer, uint16  buffer_size);
extern uint32 pts_write_attrCB(uint16 attribute_handle, uint16  transaction_mode, uint16 offset, uint8  *buffer, uint16 buffer_size);
extern uint32 pts_init_service( void );


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PTSSERVICE_H__ */
/** @} */ // 模块结尾,自行拷贝到末尾


