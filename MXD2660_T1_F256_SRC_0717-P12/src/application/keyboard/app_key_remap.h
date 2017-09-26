/**
 *****************************************************************************
 * @file     key_remap.h
 * @author   Richard
 * @version  V1.0.0
 * @date     09-Oct-2016
 * @brief    key function remap definition
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
 */

 /**
  * @addtogroup Application
  * @{
  * @defgroup key_remap key_remap
  * @{
  */
  
#ifndef __KEY_REMAP_H__
#define __KEY_REMAP_H__

#include "system_config.h"

/**
 * @brief  ble key val definition
 */ 
typedef enum
{
    KEY_REALEASE = 0,       /**<key press release*/
    KEY_BACK,               /**<return previous level*/
    KEY_RIGHT,               /**<right direction sel*/
    KEY_VOL_UP,             /**<vol++*/  
    KEY_VOL_DOWN,           /**<vol--*/
    KEY_UP,                 /**<up direction sel*/
    KEY_OK,                 /**<press OK*/
    KEY_DOWN,               /**<down direction sel*/
    KEY_HOME,               /**<home page*/
    KEY_POWER,              /**<power on/off*/ 
    KEY_LEFT,               /**<left direction sel*/
    KEY_MENU,               /**<main menu*/
    KEY_NUM0,               /**<number 0*/
    KEY_NUM1,               /**<number 1*/
    KEY_NUM2,               /**<number 2*/
    KEY_NUM3,               /**<number 3*/
    KEY_NUM4,               /**<number 4*/
    KEY_NUM5,               /**<number 5*/
    KEY_NUM6,               /**<number 6*/
    KEY_NUM7,               /**<number 7*/
    KEY_NUM8,               /**<number 8*/
    KEY_NUM9,               /**<number 9*/
    KEY_SIGNAL_SOURCE,      /**<signal source sel*/
    KEY_MORE,               /**<more info sel*/
    KEY_CH_UP,              /**<ch++*/
    KEY_CH_DOWN,            /**<ch--*/
    KEY_MIC,                /**<mic input*/
   
    KEY_PLAY_PAUSE,         /**<play/pause*/
    KEY_MUTE,               /**<mute*/   
    KEY_FF,                 /**<fast forward*/
    KEY_FB,                 /**<fast backward*/
    KEY_SET,                /**<setting*/
    KEY_REPEAT,
    
}typeKEYBOARD;

extern uint8 gkey_ir_remap[];

#endif

/** 
 * @} 
 * @} //end module
 */ 


