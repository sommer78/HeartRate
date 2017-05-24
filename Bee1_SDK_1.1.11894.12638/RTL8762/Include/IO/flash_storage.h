#ifndef flash_storage_h
#define flash_storage_h

#include <stdint.h>

/////////////////////////////////////////////////////////////////////

typedef struct
{
    uint8_t  addr[6];
    uint8_t  remote_bd_type;
    uint8_t  padding;
} remote_BD_struct;

typedef struct
{
    uint8_t linkKeyLength;
    uint8_t padding[3];
    uint8_t linkKey[28];
} LTK_struct;

typedef struct
{
    uint8_t linkKeyLength;
    uint8_t padding[3];
    uint8_t linkKey[28];
} remLTK_struct;

typedef struct
{
    uint8_t linkKeyLength;
    uint8_t padding[3];
    uint8_t linkKey[28];
} IRK_struct;

typedef struct
{
    uint8_t Local_name[64];
} Local_name_struct;

typedef struct
{
    uint8_t data_length;
    uint8_t padding[3];

    uint8_t val[32];
} cccData_struct;


/////////////////////////////////////////////////////////////////////

#define DEFINE_SAVE_LOAD_FUNCTION(type)\
    uint32_t fs_save_##type(type* pdata);\
    uint32_t fs_load_##type(type* pdata);

/////////////////////////////////////////////////////////////////////

//uint32_t fs_load_remote_BD_struct(remote_BD_struct* pdata);
//uint32_t fs_save_remote_BD_struct(remote_BD_struct* pdata);

DEFINE_SAVE_LOAD_FUNCTION(remote_BD_struct);

DEFINE_SAVE_LOAD_FUNCTION(LTK_struct);
DEFINE_SAVE_LOAD_FUNCTION(remLTK_struct);
DEFINE_SAVE_LOAD_FUNCTION(IRK_struct);

DEFINE_SAVE_LOAD_FUNCTION(Local_name_struct);

DEFINE_SAVE_LOAD_FUNCTION(cccData_struct);

/////////////////////////////////////////////////////////////////////

#endif // flash_storage_h
