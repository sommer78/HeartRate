#ifndef __MXD_CMD_TBL_H__
#define __MXD_CMD_TBL_H__

#ifndef __USE_32BIT_PROCESSOR__
#define __USE_32BIT_PROCESSOR__
typedef unsigned char MXD_U8, *PMXD_U8;
typedef unsigned short MXD_U16, *PMXD_U16;
typedef unsigned int MXD_U32, *PMXD_U32;
typedef signed char MXD_S8, *PMXD_S8;
typedef signed short MXD_S16, *PMXD_S16;
typedef signed int MXD_S32, *PMXD_S32;
typedef signed char MXD_BOOL;
typedef char MXD_CHAR;
#endif

typedef MXD_U8 (*T_ACI_CMD_FUNC)(MXD_U8* pBuf);
typedef MXD_U8 (*T_ACI_EVENT_FUNC)(MXD_U8* pRetParams);


typedef struct mxd_hci_cmd_tbl_item
{
    T_ACI_CMD_FUNC      pfAciCmdFunc;
    T_ACI_EVENT_FUNC    pfAciEventFunc;
} T_HCI_CMD_TBL_ITEM, *P_T_CMD_TBL_ITEM;

#endif /* __MXD_CMD_TBL_H__ */