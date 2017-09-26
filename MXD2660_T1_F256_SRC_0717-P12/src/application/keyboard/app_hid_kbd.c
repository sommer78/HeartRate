/**
 *****************************************************************************
 * @file     kbd_app.c
 * @author   Richard
 * @version  V1.0.0
 * @date     31-Oct-2016
 * @brief    kbd application
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
 */
#include "string.h"
#include "cmsis_os.h"
#include "system_config.h"
#include "app_task.h"
#include "app_message.h"
#include "app_hid_kbd.h"
#include "app_key_remap.h"
#include "app_ir.h"
#include "app_ble.h"
#include "gatts_api.h"
#include "hid_service.h"
#include "ble_api.h"
#include "timer.h"
#include "mxd2660_gpio.h"
#include "app_hid_kbd.h"
#include "ble_msg_api.h"

timer_node_t    kbd_adv_timer;
uint8 keyindex[HID_KEYBOARD_IN_RPT_LEN]; 
uint8 g_mxd_save_key=0;
APP_PERIPHERAL_MSG g_kmsg;
uint8 g_keyboard_type = 0;  //default is generic desk top

/**
 * @brief  convert generic keyboard key to report map index
 * @param  p_key_msg msg from kbd
 * @param  p_data hid report buffer
 * @return none
 */
bool kbd_hid_generic_report(APP_PERIPHERAL_MSG *p_key_msg, uint8 *p_data)
{
    uint8 i;
    uint8 flag=1;
	
    for(i = 0; i < 2; i++)
    {
        switch(p_key_msg->hid_key[i])
        {
            case KEY_BACK:
                p_data[i + 2] = HID_KEYBOARD_BACK;
                break;
            
            case KEY_RIGHT:
                p_data[i + 2] = HID_KEYBOARD_RIGHT_ARROW;
                break;

            case KEY_VOL_UP:
                p_data[i + 2] = HID_KEYBOARD_VOLUME_UP;
                break;

            case KEY_VOL_DOWN:
                p_data[i + 2] = HID_KEYBOARD_VOLUME_DOWN;
                break;

            case KEY_UP:
                p_data[i + 2] = HID_KEYBOARD_UP_ARROW; 
                break;

            case KEY_OK:
                p_data[i + 2] = HID_KEYBOARD_ENTER;
                break;

            case KEY_DOWN:
                p_data[i + 2] = HID_KEYBOARD_DOWN_ARROW; 
                break;

            case KEY_HOME:
                p_data[i + 2] = HID_KEYBOARD_HOME;
                break;
            
            case KEY_POWER:
                p_data[i + 2] = HID_KEYBPAD_POWER;
                break;

            case KEY_LEFT:
                p_data[i + 2] = HID_KEYBOARD_LEFT_ARROW; 
                break;

            case KEY_MENU:
                p_data[i + 2] = HID_KEYBPAD_MENU;
                break;

            case KEY_REPEAT:
            default:
                break;
        }
    }
    
    if(p_data[2] == 0)  //no key press
    {
        flag = 0;               
    }
    return flag;
}

/**
 * @brief  convert consumer keyboard key to report map index
 * @param  p_key_msg msg from kbd
 * @param  p_data hid report buffer
 * @return none
 */
bool kbd_hid_consumer_report(APP_PERIPHERAL_MSG *p_key_msg, uint8 *p_data)
{
	uint8 flag=1;
	
	switch(p_key_msg->hid_key[0])
    {
        //consumer control key
    case KEY_NUM1:
        p_data[0] = HID_CONSUMER_VOL_UP;   //vol up
        break;

    case KEY_NUM2:
        p_data[0] = HID_CONSUMER_VOL_DW;  //vol down
        break;

    case KEY_NUM3:
        p_data[0] = HID_CONSUMER_PWR;  //power
        break;

    case KEY_NUM4:
        p_data[0] = HID_CONSUMER_VOL_MUTE;  //mute
        break;
    
//    case KEY_NUM5:
//        p_data[0] = HID_CONSUMER_PLAY;  //PLAY
//        break;
//    
//    case KEY_NUM6:
//        p_data[0] = HID_CONSUMER_NEXT;  //NEXT
//        break;
//    
//    case KEY_NUM7:
//        p_data[0] = HID_CONSUMER_PREV;  //PREV
//        break;
//    
//    case KEY_NUM8:
//        p_data[0] = HID_CONSUMER_STOP;  //STOP
//        break;
    
    case KEY_BACK:
        p_data[0] = HID_CONSUMER_PLAY;  //PLAY
        break;
    
    case KEY_RIGHT:
        p_data[0] = HID_CONSUMER_NEXT;  //NEXT
        break;

    case KEY_VOL_UP:
        p_data[0] = HID_CONSUMER_PREV;  //PREV
        break;

    case KEY_VOL_DOWN:
        p_data[0] = HID_CONSUMER_STOP;  //STOP
        break;
    
    case KEY_REPEAT:
        break;

    default:
        flag = 0;
        break;

    }
	return flag;
}



/**
 * @brief  kbd msg to ble
 * @param  pKeyMsg msg from kbd
 * @return none
 */
void kbd_msgToBle_handle(APP_PERIPHERAL_MSG *p_key_msg)
{
    uint16 att_handle;

    if(p_key_msg->hid_key[0] == KEY_REPEAT)
    {
        return;
    }

    memset(keyindex, 0, 8);

//    if(kbd_hid_generic_report(p_key_msg, keyindex))  //generic desktop
//    {
//        g_keyboard_type = 0;  //generic desk top key
//    }
//    else 
        if(kbd_hid_consumer_report( p_key_msg, keyindex)) 
    {
        g_keyboard_type = 1;  //consumer key
    }
    
    switch(g_keyboard_type)
    {
        case 0:
            att_handle = hid_get_rpt_att_handle(HID_RPT_ID_KEY_IN, HID_REPORT_TYPE_INPUT);
            if(att_handle == 0xffff  )
            {
                return;
            }
            GATTS_send_notify(att_handle, keyindex, HID_KEYBOARD_IN_RPT_LEN, 0);  
            break;
            
        case 1:
            att_handle = hid_get_rpt_att_handle(HID_RPT_ID_CC_IN, HID_REPORT_TYPE_INPUT);
            if(att_handle == 0xffff  )
            {
                return;
            }
            GATTS_send_notify(att_handle, keyindex, HID_KEYBOARD_IN_RPT_LEN, 0);  
            break;
                       
        default:
            break;
    }

     
}

void kbd_timeout_handler(void* p_time_out)
{
    timer_stop(&kbd_adv_timer);
    if(g_ble_Status.is_ble_connected) 
    {
        ble_api_terminate_conn(0, 0x13);
        ble_api_adv_stop();
    }
    else
    {
        ble_api_adv_stop();
    } 
	
}

void kbd_ble_connection_goto_encryted_handler(void )
{
    if(1 == g_mxd_save_key)
    {
        kbd_msgToBle_handle(&g_kmsg);
        g_kmsg.hid_key[0]=0;
        kbd_msgToBle_handle(&g_kmsg);
        g_mxd_save_key=0;
    }
	
}

/**
 * @brief  handle kbd msg
 * @param  p_key_msg msg structure of app kbd
 * @return NONE
 */
void kbd_msg_handle(APP_PERIPHERAL_MSG *p_key_msg)
{
    if(!g_ble_Status.is_ble_connected)  //not ble message need to handle
    {
        //function peripreal message processing
        if(p_key_msg->hid_key[0] >= KEY_BACK  &&  p_key_msg->hid_key[0] <= KEY_NUM4)
        {
            //bee configuration,open the buzzer
            gpio_set_bit(GPIO_4);
            ble_app_start_adv_type();
            /*g_mxd_save_key=1;
            memcpy(&g_kmsg,p_key_msg,sizeof(APP_PERIPHERAL_MSG));*/
        }
        else
        {
            gpio_clear_bit(GPIO_4);  //stop the buzzer
        }
        if(p_key_msg->hid_key[1] == 0)
        {
            app_ir_msg_proc(gkey_ir_remap[p_key_msg->hid_key[0]]);
        }
    }
    else  //ble has been connected
    {
        gpio_clear_bit(GPIO_4);  //stop the buzzer
        if(g_ble_Status.is_ble_encrypted)
        {
            kbd_msgToBle_handle(p_key_msg);
        }
	        if(p_key_msg->hid_key[0] == KEY_OK)
        {
            //ble_app_update_conn();
        }
    }
//    timer_stop(&kbd_adv_timer);
    timer_start(&kbd_adv_timer, 90000, NULL);//wait 6s to disconnect to save energy
}





