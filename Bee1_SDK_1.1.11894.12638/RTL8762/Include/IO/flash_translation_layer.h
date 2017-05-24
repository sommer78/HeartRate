#ifndef flash_translation_layer_h
#define flash_translation_layer_h

//////////////////////////////////////////////////////////

#if WIN32
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;
typedef int            int32_t;
#define FMC_PAGE_SIZE  (0x800)
#define FMC_SUCCESS (0)
#else // BEE
#include <stdint.h>
#endif

#ifdef  __cplusplus
extern  "C" {
#endif  // __cplusplus


// return 0 success
// return !0 fail
uint32_t Save_To_Storage(void* pdata, uint16_t offset, uint16_t size);

// return 0 success
// return !0 fail
uint32_t Load_From_Storage(void* pdata, uint16_t offset, uint16_t size);


uint32_t FTL_init(uint8_t pagenum);

// logical_addr is 4 bytes alignment addr
uint32_t FTL_read(uint16_t logical_addr);
uint32_t FTL_write(uint16_t logical_addr, uint32_t w_data);
uint32_t FTL_get_error(void);

enum
{
    FTL_ioctl_debug = 0,
    //FTL_ioctl_get_last_error = 1,
    FTL_ioctl_clear_all = 2,
};
uint32_t FTL_ioctl(uint32_t cmd, uint32_t p1, uint32_t p2);

#if WIN32
uint32_t FTL_de_init(void);
#endif

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // flash_translation_layer_h
