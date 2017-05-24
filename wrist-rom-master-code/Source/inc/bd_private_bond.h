/*************************************************************************
*
* Wearable device team
* This file used to implement privat bond
*
**************************************************************************/

#ifndef _PRIVATE_BOND_H_
#define _PRIVATE_BOND_H_
#include <stdint.h>
#include "config.h"

#define   USER_ID_LENGTH (32)

#define  FLASH_PAGE_USER_ID                     FLASH_PAGE_PRIVATE_BOND


uint32_t bond_read_user_id(void);
uint32_t bond_store_user_id(void);

void bond_press_handle(void );
void bond_success_event_dispatch(void);
void bond_fail_event_dispatch(void);
uint32_t check_user_id_bonded(const uint8_t* user_id, uint8_t length);

void login_success_event_dispatch(void);
void login_fail_event_dispatch(void);
bool check_has_bonded(void);
void set_device_has_bonded(bool value);

#endif //_PRIVATE_BOND_H_
