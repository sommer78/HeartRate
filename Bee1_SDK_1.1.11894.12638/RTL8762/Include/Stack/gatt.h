/**
************************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     gatt.h
* @brief    Define GATT attributes table structure
* @details   User can use struct TAttribAppl to create a table, and then use blueAPI_GATTServiceRegisterReq to register this table. 
* @author   ranhui & jane
* @date     2015-10-29
* @version  v1.2
*************************************************************************************************************
*/
#ifndef __GATT_H
#define __GATT_H


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Attributes_Table GATT Attributes Table Structure
  * @{
  */
/** @defgroup uuid_def GATT Common UUID Definitions
  * @brief UUID Definitions  
  * @{
  */
/** @defgroup service_uuid_def GATT service and characteristic definition codes
  * @brief GATT service and characteristic definition codes  
  * @{
  */
#define GATT_UUID_PRIMARY_SERVICE       0x2800
#define GATT_UUID_SECONDARY_SERVICE     0x2801
#define GATT_UUID_INCLUDE               0x2802
#define GATT_UUID_CHARACTERISTIC        0x2803
/** @} End of service_uuid_def */

/** @defgroup char_uuid_def GATT characteristic descriptors
  * @brief GATT characteristic descriptors  
  * @{
  */
#define GATT_UUID_CHAR_EXTENDED_PROP    0x2900
#define GATT_UUID_CHAR_USER_DESCR       0x2901
#define GATT_UUID_CHAR_CLIENT_CONFIG    0x2902
#define GATT_UUID_CHAR_SERVER_CONFIG    0x2903
#define GATT_UUID_CHAR_FORMAT           0x2904
#define GATT_UUID_CHAR_AGGR_FORMAT      0x2905
#define GATT_UUID_CHAR_VALID_RANGE      0x2906
#define GATT_UUID_CHAR_EXTERNAL_REPORT_REFERENCE 0x2907
#define GATT_UUID_CHAR_REPORT_REFERENCE 0x2908
#define GATT_UUID_CHAR_SENSING_CONFIGURATION 0x290B
#define GATT_UUID_CHAR_SENSING_MEASUREMENT 0x290C
#define GATT_UUID_CHAR_SENSING_TRIGGER_SETTING 0x290D

/** @} End of char_uuid_def */

/** @defgroup char_uuid_def1 GAP service/profile UUID
  * @brief GAP service/profile UUID  
  * @{
  */
#define GATT_UUID_CHAR_DEVICE_NAME          0x2A00
#define GATT_UUID_CHAR_APPEARANCE           0x2A01
#define GATT_UUID_CHAR_PER_PRIV_FLAG        0x2A02
#define GATT_UUID_CHAR_RECONN_ADDRESS       0x2A03
#define GATT_UUID_CHAR_PER_PREF_CONN_PARAM  0x2A04
#define GATT_UUID_CHAR_SERVICE_CHANGED      0x2A05
/** @} End of char_uuid_def1 */
/** @} End of uuid_def */
/** @defgroup gatt_const GATT Common Constants
  * @brief GATT common constants  
  * @{
  */
/** @} End of gatt_const */
/** @defgroup gatt_perm TAttribAppl.wPermissions
  * @brief GATT attribute read/write permissions, encryption key size  
  * @{
  */
/* GATT attribute read/write permissions, encryption key size */
/** @brief TAttribAppl.wPermissions bits 0..15:                       */
#define GATT_PERM_NONE                   0x00
#define GATT_PERM_ALL                    0x01  /**< @brief bits 0..1 (rd), 4..5 (wr), 8..9 (notif/ind) */
#define GATT_PERM_AUTHEN_REQ             0x02
#define GATT_PERM_AUTHEN_MITM_REQ        0x03
#define GATT_PERM_AUTHOR_REQ             0x04  /**< @brief bits 2 (rd), 6 (wr), 10 (notif/ind) */
#define GATT_PERM_RESERVED               0x08  /**< @brief bits 3 (rd), 7 (wr), 11 (notif/ind) */


#define GATT_PERM_READ                   GATT_PERM_ALL            /**< @brief read (bits 0..3) */
#define GATT_PERM_READ_AUTHEN_REQ        GATT_PERM_AUTHEN_REQ
#define GATT_PERM_READ_AUTHEN_MITM_REQ   GATT_PERM_AUTHEN_MITM_REQ
#define GATT_PERM_READ_AUTHOR_REQ        GATT_PERM_AUTHOR_REQ

#define GATT_PERM_READ_AUTHEN_GET(x)     (x & 0x03)
#define GATT_PERM_READ_AUTHOR_GET(x)     (x & 0x04)


#define GATT_PERM_WRITE                  (GATT_PERM_ALL << 4)      /**< @brief write (bits 4..7) */
#define GATT_PERM_WRITE_AUTHEN_REQ       (GATT_PERM_AUTHEN_REQ << 4)
#define GATT_PERM_WRITE_AUTHEN_MITM_REQ  (GATT_PERM_AUTHEN_MITM_REQ << 4)
#define GATT_PERM_WRITE_AUTHOR_REQ       (GATT_PERM_AUTHOR_REQ << 4)

#define GATT_PERM_WRITE_AUTHEN_GET(x)    ((x >> 4) & 0x03)
#define GATT_PERM_WRITE_AUTHOR_GET(x)    ((x >> 4) & 0x04)


#define GATT_PERM_NOTIF_IND                  (GATT_PERM_ALL << 8)  /**< @brief notification/indication (bits 8..11) */
#define GATT_PERM_NOTIF_IND_AUTHEN_REQ       (GATT_PERM_AUTHEN_REQ << 8)
#define GATT_PERM_NOTIF_IND_AUTHEN_MITM_REQ  (GATT_PERM_AUTHEN_MITM_REQ << 8)
#define GATT_PERM_NOTIF_IND_AUTHOR_REQ       (GATT_PERM_AUTHOR_REQ << 8)

#define GATT_PERM_NOTIF_IND_AUTHEN_GET(x)    ((x >> 8) & 0x03)
#define GATT_PERM_NOTIF_IND_AUTHOR_GET(x)    ((x >> 8) & 0x04)


/** @brief key size - 1 (bits 12..15) */
#define GATT_PERM_KEYSIZE(size)          ((size-1) << 12)
#define GATT_PERM_KEYSIZE_GET(x, size)   {                             \
        size = ((x >> 12) & 0x0F);  \
        if ( size > 0 )             \
            size++;                   \
    }
/** @} End of gatt_perm */

/** @addtogroup gatt_const GATT common constants
  * @brief GATT common constants  
  * @{
  */
/** @defgroup gatt_prop GATT characteristic properties
  * @brief GATT characteristic properties  
  * @{
  */
#define GATT_CHAR_PROP_BROADCAST          0x01
#define GATT_CHAR_PROP_READ               0x02
#define GATT_CHAR_PROP_WRITE_NO_RSP       0x04
#define GATT_CHAR_PROP_WRITE              0x08
#define GATT_CHAR_PROP_NOTIFY             0x10
#define GATT_CHAR_PROP_INDICATE           0x20
#define GATT_CHAR_PROP_WRITE_AUTH_SIGNED  0x40
#define GATT_CHAR_PROP_EXT_PROP           0x80
/** @} End of gatt_prop */

/** @defgroup gatt_conf GATT client characteristic configuration
  * @brief GATT client characteristic configuration bit field  
  * @{
  */
#define GATT_CLIENT_CHAR_CONFIG_DEFAULT   0x0000
#define GATT_CLIENT_CHAR_CONFIG_NOTIFY    0x0001
#define GATT_CLIENT_CHAR_CONFIG_INDICATE  0x0002
/** @} End of gatt_conf */

/** @brief GATT server characteristic configuration bit field */
#define GATT_SERVER_CHAR_CONFIG_BROADCAST 0x0001
/** @} End of gatt_const */


#define UUID_16BIT_SIZE                 2
#define UUID_128BIT_SIZE                16


#define HI_WORD(x)  ((unsigned short)((x & 0xFF00) >> 8))
#define LO_WORD(x)  ((unsigned char)(x))



/*---------------------------------------------------------------------------
 * GATT server attribute descriptor
 *--------------------------------------------------------------------------*/

/** @defgroup gatt_flag TAttribAppl.wFlags
  * @brief Define this attribute flags  
  * @{
  */
/*--- flags ---*/
#define ATTRIB_FLAG_VOID          0x0000
/** @brief use 128 bit UUID. If set this, can't set ATTRIB_FLAG_VALUE_INCL at the same time. */
#define ATTRIB_FLAG_UUID_128BIT   0x0001
/** @brief attribute value is included following 16 bit UUID: */
#define ATTRIB_FLAG_VALUE_INCL    0x0002
/** @brief application has to supply/write value: */
#define ATTRIB_FLAG_VALUE_APPL    0x0004
/** @brief attribute value is ASCII_Z string: */
#define ATTRIB_FLAG_ASCII_Z       0x0008
/** @brief application has to be informed about CCCD value (changes): */
#define ATTRIB_FLAG_CCCD_APPL     0x0010
/** @brief evaluated only for primary service declaration attributes, if set to 1 the service is available using GATT over BR/EDR links:  */
#define ATTRIB_FLAG_BR_EDR        0x0400
/** @brief evaluated only for primary service declaration attributes, if set to 1 the service is available using GATT over LE links:     */
#define ATTRIB_FLAG_LE            0x0800
/** @} End of gatt_flag */

typedef struct _AttribAppl
{
    uint16_t    wFlags;            /**< @brief @ref gatt_flag */
    uint8_t    bTypeValue[2 + 14]; /**< @brief 16 bit UUID + included value or 128 bit UUID */
    uint16_t    wValueLen;         /**< @brief length of value              */
    void*  pValueContext;          /**< @brief ptr to value if not included or applic. supplied */
    uint16_t    wPermissions;      /**< @brief read/write permissions, encryption key size .  @ref gatt_perm */
} TAttribAppl, * PAttribAppl;

/** @} End of Attributes_Table */
/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __GATT_H */

