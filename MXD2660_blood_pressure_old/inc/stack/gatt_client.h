

#ifndef _GATT_CLIENT_H_
#define _GATT_CLIENT_H_

#include "hci.h"

#if defined __cplusplus
extern "C" {
#endif

    typedef enum
    {
        P_READY,
        P_W2_SEND_SERVICE_QUERY,/*1*/
        P_W4_SERVICE_QUERY_RESULT,
        P_W2_SEND_SERVICE_WITH_UUID_QUERY,
        P_W4_SERVICE_WITH_UUID_RESULT,

        P_W2_SEND_ALL_CHARACTERISTICS_OF_SERVICE_QUERY,/*5*/
        P_W4_ALL_CHARACTERISTICS_OF_SERVICE_QUERY_RESULT,
        P_W2_SEND_CHARACTERISTIC_WITH_UUID_QUERY,
        P_W4_CHARACTERISTIC_WITH_UUID_QUERY_RESULT,

        P_W2_SEND_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY,
        P_W4_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT,

        P_W2_SEND_INCLUDED_SERVICE_QUERY,/*0xB*/
        P_W4_INCLUDED_SERVICE_QUERY_RESULT,
        P_W2_SEND_INCLUDED_SERVICE_WITH_UUID_QUERY,
        P_W4_INCLUDED_SERVICE_UUID_WITH_QUERY_RESULT,

        P_W2_SEND_READ_CHARACTERISTIC_VALUE_QUERY,
        P_W4_READ_CHARACTERISTIC_VALUE_RESULT,

        P_W2_SEND_READ_BLOB_QUERY,/*0x11*/
        P_W4_READ_BLOB_RESULT,

        P_W2_SEND_READ_BY_TYPE_REQUEST,
        P_W4_READ_BY_TYPE_RESPONSE,

        P_W2_SEND_READ_MULTIPLE_REQUEST,
        P_W4_READ_MULTIPLE_RESPONSE,

        P_W2_SEND_WRITE_CHARACTERISTIC_VALUE,/*0x17*/
        P_W4_WRITE_CHARACTERISTIC_VALUE_RESULT,

        P_W2_PREPARE_WRITE,
        P_W4_PREPARE_WRITE_RESULT,
        P_W2_PREPARE_RELIABLE_WRITE,
        P_W4_PREPARE_RELIABLE_WRITE_RESULT,

        P_W2_EXECUTE_PREPARED_WRITE,/*0x1D*/
        P_W4_EXECUTE_PREPARED_WRITE_RESULT,
        P_W2_CANCEL_PREPARED_WRITE,
        P_W4_CANCEL_PREPARED_WRITE_RESULT,
        P_W2_CANCEL_PREPARED_WRITE_DATA_MISMATCH,
        P_W4_CANCEL_PREPARED_WRITE_DATA_MISMATCH_RESULT,

        P_W2_SEND_READ_CLIENT_CHARACTERISTIC_CONFIGURATION_QUERY,/*0x23*/
        P_W4_READ_CLIENT_CHARACTERISTIC_CONFIGURATION_QUERY_RESULT,
        P_W2_WRITE_CLIENT_CHARACTERISTIC_CONFIGURATION,
        P_W4_CLIENT_CHARACTERISTIC_CONFIGURATION_RESULT,

        P_W2_SEND_READ_CHARACTERISTIC_DESCRIPTOR_QUERY,
        P_W4_READ_CHARACTERISTIC_DESCRIPTOR_RESULT,

        P_W2_SEND_READ_BLOB_CHARACTERISTIC_DESCRIPTOR_QUERY,
        P_W4_READ_BLOB_CHARACTERISTIC_DESCRIPTOR_RESULT,/*0x2A*/

        P_W2_SEND_WRITE_CHARACTERISTIC_DESCRIPTOR,
        P_W4_WRITE_CHARACTERISTIC_DESCRIPTOR_RESULT,

        // all long writes use this
        P_W2_PREPARE_WRITE_CHARACTERISTIC_DESCRIPTOR,
        P_W4_PREPARE_WRITE_CHARACTERISTIC_DESCRIPTOR_RESULT,
        P_W2_EXECUTE_PREPARED_WRITE_CHARACTERISTIC_DESCRIPTOR,
        P_W4_EXECUTE_PREPARED_WRITE_CHARACTERISTIC_DESCRIPTOR_RESULT,/*0x30*/

        // gatt reliable write API use this (manual version of the above)
        P_W2_PREPARE_WRITE_SINGLE,
        P_W4_PREPARE_WRITE_SINGLE_RESULT,

        P_W4_CMAC_READY,
        P_W4_CMAC_RESULT,
        P_W2_SEND_SIGNED_WRITE,
        P_W4_SEND_SINGED_WRITE_DONE,
    } gatt_client_state_t;



#define GATT_CLIENT_MAX_SERVICE_NUM 3
#define GATT_CLIENT_MAX_CHAR_NUM 4
#define GATT_CLIENT_MAX_DESC_NUM 1

    /* API_START */

    typedef struct
    {
        uint16 start_group_handle;
        uint16 end_group_handle;
        uint16 uuid16;
        uint8  uuid128[16];
    } gatt_client_service_t;

    typedef struct
    {
        uint16 start_handle;
        uint16 value_handle;
        uint16 end_handle;
        uint16 properties;
        uint16 uuid16;
        uint8  uuid128[16];
    } gatt_client_characteristic_t;

    typedef struct
    {
        uint16 handle;
        uint16 uuid16;
        uint8  uuid128[16];
    } gatt_client_characteristic_descriptor_t;


    typedef struct
    {
        uint16 handle;
        uint16 uuid16;
        uint8  uuid128[16];
    } mxd_characteristic_descriptor_t;
    typedef struct
    {
        uint16 start_handle;
        uint16 value_handle;
        uint16 end_handle;
        uint16 properties;
        uint16 uuid16;
        uint8  uuid128[16];
        uint16 des_cnt;
        mxd_characteristic_descriptor_t des[GATT_CLIENT_MAX_DESC_NUM];
    } mxd_characteristic_t;

    typedef struct
    {
        uint16 start_group_handle;
        uint16 end_group_handle;
        uint16 uuid16;
        uint8  uuid128[16];
        uint16 char_cnt;
        mxd_characteristic_t chara[GATT_CLIENT_MAX_CHAR_NUM];

    } mxd_service_t;

    typedef enum
    {
        CENTRAL_IDLE,
        CENTRAL_W4_NAME_QUERY_COMPLETE,
        CENTRAL_W4_NAME_VALUE,
        CENTRAL_W4_RECONNECTION_ADDRESS_QUERY_COMPLETE,
        CENTRAL_W4_PERIPHERAL_PRIVACY_FLAG_QUERY_COMPLETE,
        CENTRAL_W4_SIGNED_WRITE_QUERY_COMPLETE,
        CENTRAL_W4_PRIMARY_SERVICES,
        CENTRAL_ENTER_SERVICE_UUID_4_DISCOVER_CHARACTERISTICS,
        CENTRAL_ENTER_START_HANDLE_4_DISCOVER_CHARACTERISTICS,
        CENTRAL_ENTER_END_HANDLE_4_DISCOVER_CHARACTERISTICS,
        CENTRAL_W4_CHARACTERISTICS,
        CENTRAL_W4_DISCOVER_CHARACTERISTIC_DESCRIPTORS,
        CENTRAL_W4_READ_CHARACTERISTIC_VALUE_BY_HANDLE,
        CENTRAL_ENTER_HANDLE_4_READ_CHARACTERISTIC_VALUE_BY_UUID,
        CENTRAL_W4_READ_CHARACTERISTIC_VALUE_BY_UUID,
        CENTRAL_ENTER_OFFSET_4_READ_LONG_CHARACTERISTIC_VALUE_BY_HANDLE,
        CENTRAL_W4_READ_LONG_CHARACTERISTIC_VALUE_BY_HANDLE,
        CENTRAL_W4_READ_CHARACTERISTIC_DESCRIPTOR_BY_HANDLE,
        CENTRAL_ENTER_OFFSET_4_READ_LONG_CHARACTERISTIC_DESCRIPTOR_BY_HANDLE,
        CENTRAL_W4_READ_LONG_CHARACTERISTIC_DESCRIPTOR_BY_HANDLE,
        CENTRAL_W4_READ_MULTIPLE_CHARACTERISTIC_VALUES,
        CENTRAL_W4_WRITE_WITHOUT_RESPONSE,
        CENTRAL_W4_WRITE_CHARACTERICISTIC_VALUE,
        CENTRAL_ENTER_HANDLE_4_WRITE_LONG_CHARACTERISTIC_VALUE,
        CENTRAL_W4_WRITE_LONG_CHARACTERISTIC_VALUE,
        CENTRAL_W4_RELIABLE_WRITE,
        CENTRAL_W4_WRITE_CHARACTERISTIC_DESCRIPTOR,
        CENTRAL_ENTER_HANDLE_4_WRITE_LONG_CHARACTERISTIC_DESCRIPTOR,
        CENTRAL_W4_WRITE_LONG_CHARACTERISTIC_DESCRIPTOR,
        CENTRAL_W4_INCLUDED_SERVICES,
        CENTRAL_W4_SIGNED_WRITE,

        CENTRAL_W4_ENTER_HANDLE_4_PREPARE_WRITE,
        CENTRAL_W4_ENTER_OFFSET_4_PREPARE_WRITE,

        CENTRAL_GPA_ENTER_UUID,
        CENTRAL_GPA_ENTER_START_HANDLE,
        CENTRAL_GPA_ENTER_END_HANDLE,
        CENTRAL_GPA_W4_RESPONSE,
        CENTRAL_GPA_W4_RESPONSE2,
        CENTRAL_GPA_W4_RESPONSE3,
        CENTRAL_GPA_W4_RESPONSE4,
    } central_state_t;






#define STREAM_TO_UINT8(u8, p)   {u8 = (uint8)(*(p)); (p) += 1;}
#define STREAM_TO_BDADDR(a, p)   {register int ijk; register uint8 *pbda = (uint8 *)a + BD_ADDR_LEN - 1; for (ijk = 0; ijk < BD_ADDR_LEN; ijk++) *pbda-- = *p++;}
#define ATT_CHARACTERISTIC_GAP_DEVICE_NAME_01_VALUE_HANDLE 0x0003
#define ATT_CHARACTERISTIC_GAP_APPEARANCE_01_VALUE_HANDLE 0x0005
#define ATT_CHARACTERISTIC_GATT_SERVICE_CHANGED_01_VALUE_HANDLE 0x0008


    /**
     * @brief Set up GATT client.
     */
    extern uint16 g_client_serv_state;
    extern central_state_t central_state;
   // extern uint8  pts_suppress_mtu_exchange;
    extern void gatt_client_init(void);
    extern void gatt_client_run(uint8 idx);
    extern void mxd_get_client_init(void);
    extern uint8 gatt_client_discover_primary_services(uint8 idx);
    extern uint8 gatt_client_discover_primary_services_by_uuid16(uint8 idx, uint16 uuid16);
    extern uint8 gatt_client_discover_primary_services_by_uuid128(uint8 idx, uint8 * uuid128);
    extern uint8 gatt_client_discover_characteristics_for_service(uint8 idx, gatt_client_service_t *service);
    extern uint8 gatt_client_discover_characteristics_for_handle_range_by_uuid16(uint8 idx, uint16 start_handle, uint16 end_handle, uint16 uuid16);
    extern uint8 gatt_client_discover_characteristics_for_handle_range_by_uuid128(uint8 idx, uint16 start_handle, uint16 end_handle, uint8 * uuid128);
    extern uint8 gatt_client_discover_characteristic_descriptors(uint8 idx, gatt_client_characteristic_t *characteristic);
    extern uint8 gatt_client_find_included_services_for_service(uint8 idx, gatt_client_service_t *service);
    extern uint8 gatt_client_read_value_of_characteristic_using_value_handle(uint8 idx, uint16 value_handle);
    extern uint8 gatt_client_read_value_of_characteristics_by_uuid16(uint8 idx, uint16 start_handle, uint16 end_handle, uint16 uuid16);
    extern uint8 gatt_client_read_value_of_characteristics_by_uuid128(uint8 idx, uint16 start_handle, uint16 end_handle, uint8 * uuid128);
    extern uint8 gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset(uint8 idx, uint16 characteristic_value_handle, uint16 offset);
    extern uint8 gatt_client_write_value_of_characteristic_without_response(uint8 idx, uint16 value_handle, uint16 value_length, uint8 * value);
    extern uint8 gatt_client_write_value_of_characteristic(uint8 idx, uint16 value_handle, uint16 value_length, uint8 * data);
    extern uint8 gatt_client_write_long_value_of_characteristic_with_offset(uint8 idx, uint16 value_handle, uint16 offset, uint16 value_length, uint8  * data);
    extern void gatt_client_handle_value_notification(uint8 idx, uint8 *packet, uint16 size);
    extern void gatt_client_handle_exchange_mtu_response(uint8 idx, uint8 *packet, uint16 size);
    extern void gatt_client_handle_read_by_group_type_response(uint8 idx, uint8 *packet, uint16 size);
    extern void gatt_client_handle_value_indication(uint8 idx, uint8 *packet, uint16 size);
    extern void gatt_client_handle_read_by_type_response(uint8 idx, uint8 *packet, uint16 size);
    extern void gatt_client_handle_read_response(uint8 idx, uint8 *packet, uint16 size);
    extern void gatt_client_handle_find_by_type_value_response(uint8 idx, uint8 *packet, uint16 size);
    extern void gatt_client_handle_find_information_reply(uint8 idx, uint8 *packet, uint16 size);
    extern void gatt_client_handle_write_response(uint8 idx, uint8 *packet, uint16 size);
    extern void gatt_client_handle_read_blob_response(uint8 idx, uint8 *packet, uint16 size);
    extern void gatt_client_handle_prepare_write_response(uint8 idx, uint8 *packet, uint16 size);
    extern void gatt_client_handle_execute_write_response(uint8 idx, uint8 *packet, uint16 size);
    extern void gatt_client_handle_read_multiple_response(uint8 idx, uint8 *packet, uint16 size);
    extern void gatt_client_handle_error_response(uint8 idx, uint8 *packet, uint16 size);
    extern void gap_scan_adv_report_CB(uint8 * data, uint8 len);
    extern uint8 gatt_client_write_client_characteristic_configuration(uint8 idx, gatt_client_characteristic_t * characteristic, uint16 configuration);
    extern void gatt_timeout_handler(void * idx_ptr);
    extern void gatt_client_report_error_if_pending(uint8 idx, uint8 error_code);
    extern uint16 g_client_serv_cnt;
    extern mxd_service_t *g_client_serv[GATT_CLIENT_MAX_SERVICE_NUM];
#if defined __cplusplus
}
#endif

#endif
