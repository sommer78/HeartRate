#ifndef __ANCS_H_
#define __ANCS_H_

extern void ancs_connected_handler(uint8 idx);
extern void ancs_discover_service(uint8 idx, uint8 * uuid128);
extern void ancs_encrypt_chg_handler(uint8 idx, uint8 is_connection_encrypted);
extern void ancs_gatt_query_complete(void);
extern void ancs_notification_dispatch(uint8 idx, uint8 *packet, uint16 size);
extern void ancs_write_get_data(uint8 *buf, uint8 len, uint32 ancs_uid);
extern void ancs_discover_again(uint8 idx);
extern void ancs_onoff_notify(uint8 onOff);

#define ANCS_SERVER_NOT_FOUND 0
#define ANCS_SERVER_FOUND 1
#define ANCS_SERVER_NOTIFY_OFF 2
#define ANCS_SERVER_NOTIFY_ON 3

#endif


