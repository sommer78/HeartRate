enum { __FILE_NUM__ = 0 };

/**
************************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     user_cmd.c
* @brief    User defined profile test commands.
* @details  User command interfaces.
* @author
* @date     2015-03-19
* @version  v0.1
*************************************************************************************************************
*/
#include <string.h>
#include <blueapi.h>
#include "trace.h"
#include "profile.h"
#include "profile_client.h"
#include "gapbondmgr.h"
#include "central.h"
#include "user_cmd.h"
#include "simple_ble_client.h"

static TUserCmdParseResult UserCmdRemoteBdSet(TUserCmdIF * pUserCmdIF,
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdConnect(TUserCmdIF * pUserCmdIF,
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdDisconnect(TUserCmdIF * pUserCmdIF,
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdIOCapSet(TUserCmdIF * pUserCmdIF, 
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdScan(TUserCmdIF * pUserCmdIF,
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdStopScan(TUserCmdIF * pUserCmdIF,
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdSecurityReq(TUserCmdIF * pUserCmdIF,
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdConUpdateReq(TUserCmdIF * pUserCmdIF,
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdServiceDiscovery(TUserCmdIF * pUserCmdIF,
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdRelationshipDiscovery(TUserCmdIF * pUserCmdIF,
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdCharDiscovery(TUserCmdIF * pUserCmdIF,
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdCharDescriptorDiscovery(TUserCmdIF * pUserCmdIF,
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdAttribRead(TUserCmdIF * pUserCmdIF,
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdAttribReadUUID(TUserCmdIF * pUserCmdIF,
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdHandleDisplay(TUserCmdIF * pUserCmdIF,
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdSimpStartDiscover(TUserCmdIF * pUserCmdIF, 
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdSimpReadChar(TUserCmdIF * pUserCmdIF, 
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdSimpCccdOperate(TUserCmdIF * pUserCmdIF, 
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdSimpWriteChar(TUserCmdIF * pUserCmdIF, 
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdTxPowerSet(TUserCmdIF * pUserCmdIF, 
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdRssiRead(TUserCmdIF * pUserCmdIF, 
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdPatchVerGet(TUserCmdIF * pUserCmdIF, 
            TUserCmdParseValue *pParseValue);
static TUserCmdParseResult UserCmdChanClassSet(TUserCmdIF * pUserCmdIF, 
            TUserCmdParseValue *pParseValue);

/*----------------------------------------------------
 * command table
 * --------------------------------------------------*/
const TUserCmdTableEntry UserCmdTable[] =
{
    {
        "rembd",
        "rembd [BD0] [BD1] [BD2] [BD3] [BD4] [BD5] \n\r",
        "Set remote BD (hard coded or based on inquiry result)\n\r",
        UserCmdRemoteBdSet
    },

    {
        "con",
        "con \n\r",
        "connect to remote device\n\r",
        UserCmdConnect
    },

    {
        "disc",
        "disc\n\r",
        "disconnect to remote device\n\r",
        UserCmdDisconnect
    },

    {
        "scan",
        "scan\n\r",
        "scan remote devices\n\r",
        UserCmdScan
    },
    
    {
        "stopscan",
        "stopscan\n\r",
        "stopscan remote device\n\r",
        UserCmdStopScan
    },
    
    {
        "iocapset",
        "iocapset [iocap]\n\r",
        "choose io capability.\n\r",
        UserCmdIOCapSet
    },
    {
        "sauth",
        "sauth\n\r",
        "bond manager authentication request.\n\r",
        UserCmdSecurityReq
    },

    {
        "conupdreq",
        "conupdreq \n\r",
        "LE connection update request\n\r",
        UserCmdConUpdateReq
    },

    {
        "srvdis",
        "srvdis \n\r",
        "Service discovery, discover all primary services.\n\r",
        UserCmdServiceDiscovery
    },

    {
        "reldis",
        "reldis [start] [end]\n\r",
        "Relationship discovery\n\r",
        UserCmdRelationshipDiscovery
    },

    {
        "chardis",
        "chardis [start] [end]\n\r",
        "Characteristic discovery\n\r",
        UserCmdCharDiscovery
    },
    
    {
        "charddis",
        "charddis [start] [end]\n\r",
        "Characteristic descriptor discovery\n\r",
        UserCmdCharDescriptorDiscovery
    },

    {
        "read",
        "read [handle] \n\r",
        "Read attribute\n\r",
        UserCmdAttribRead
    },

    {
        "readu",
        "readu [start] [end] [UUID16]\n\r",
        "Read attribute using UUID16\n\r",
        UserCmdAttribReadUUID
    },
    {
        "lshndl",
        "lshndl [specific client module]\n\r",
        "List of locally stored specific client's handle infos\n\r",
        UserCmdHandleDisplay
    },

    {
        "simpdis",
        "simpdis\n\r",
        "Start discovery simple ble service\n\r",
        UserCmdSimpStartDiscover
    },
    {
        "simpread",
        "simpread [char] [pattern: handle/UUID16]\n\r",
        "Read all related chars by user input\n\r",
        UserCmdSimpReadChar
    },
    {
        "simpcmd",
        "simpcmd [char CCCD] [command: enable/disable]\n\r",
        "Simple BLE notify,ind switch command\n\r",
        UserCmdSimpCccdOperate
    },
    {
        "simpwrite",
        "simpwrite [char] [value]\n\r",
        "Write all related chars by user input\n\r",
        UserCmdSimpWriteChar
    },
    {
        "txpwrset",
        "txpwrset [txpwridx]\n\r",
        "Set the TX Power of BLE RF\n\r",
        UserCmdTxPowerSet
    },
    {
        "rssiread",
        "rssiread\n\r",
        "Read the RSSI value of this local MDL ID\n\r",
        UserCmdRssiRead
    },
    {
        "patchver",
        "patchver\n\r",
        "Get the patch Version\n\r",
        UserCmdPatchVerGet
    },
    {
        "chanclassset",
        "chanclassset [idx0] [idx1] [idx2] [idx3] [idx4]\n\r",
        "Set Host Channel Classification\n\r",
        UserCmdChanClassSet
    },
    /* MUST be at the end: */
    {
        0,
        0,
        0,
        0
    }
};

static TUserCmdParseResult UserCmdConnect(TUserCmdIF * pUserCmdIF, TUserCmdParseValue *pParseValue)
{
    uint8_t DestAddrType = blueAPI_RemoteBDTypeLEPublic;
    uint16_t conIntervalMin = 0x10;
    uint16_t conIntervalMax = 0x10;
    uint16_t slaveLantency = 0;
    uint16_t supervisionTimeout = 1000;
    uint16_t CE_Length = 2 * conIntervalMax - 2;

    centralSetGapParameter(GAPPRRA_CONN_DEST_ADDR_TYPE, sizeof(DestAddrType), &DestAddrType);
    centralSetGapParameter(GAPPRRA_MIN_CONN_INTERVAL, sizeof(conIntervalMin), &conIntervalMin);
    centralSetGapParameter(GAPPRRA_MAX_CONN_INTERVAL, sizeof(conIntervalMax), &conIntervalMax);
    centralSetGapParameter(GAPPRRA_SLAVE_LATENCY, sizeof(slaveLantency), &slaveLantency);
    centralSetGapParameter(GAPPRRA_TIMEOUT_MULTIPLIER, sizeof(supervisionTimeout), &supervisionTimeout);
    centralSetGapParameter(GAPPRRA_CE_LENGTH, sizeof(CE_Length), &CE_Length);

    dataUARTCmdPrint( pUserCmdIF,"UserCmdConnect\r\n%s",pUserCmdIF->cPrompt );
    central_Connect();

    return ( ResultOk );
}

static TUserCmdParseResult UserCmdDisconnect(TUserCmdIF * pUserCmdIF, TUserCmdParseValue *pParseValue)
{
    dataUARTCmdPrint( pUserCmdIF,"UserCmdDisconnect\r\n%s",pUserCmdIF->cPrompt );
    central_Disconnect();
    return ( ResultOk );
}

static TUserCmdParseResult UserCmdIOCapSet(TUserCmdIF * pUserCmdIF, TUserCmdParseValue *pParseValue)
{
    uint8_t ioCap = GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT;
    if (pParseValue->iParameterCount >= 1)
    {
        switch (pParseValue->dwParameter[0])
        {
            case 0:
                ioCap = GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT;
                dataUARTCmdPrint( pUserCmdIF,
                             "GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT\r\n%s",
                             pUserCmdIF->cPrompt );
                break;
            case 1:
                ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
                dataUARTCmdPrint( pUserCmdIF,
                             "GAPBOND_IO_CAP_DISPLAY_ONLY\r\n%s",
                             pUserCmdIF->cPrompt );
                break;
            case 2:
                ioCap = GAPBOND_IO_CAP_KEYBOARD_ONLY;
                dataUARTCmdPrint( pUserCmdIF,
                             "GAPBOND_IO_CAP_KEYBOARD_ONLY\r\n%s",
                             pUserCmdIF->cPrompt );
                break; 
            case 3:
                ioCap = GAPBOND_IO_CAP_DISPLAY_YES_NO;
                dataUARTCmdPrint( pUserCmdIF,
                             "GAPBOND_IO_CAP_DISPLAY_YES_NO\r\n%s",
                             pUserCmdIF->cPrompt );
                break;
            case 4:
                ioCap = GAPBOND_IO_CAP_KEYBOARD_DISPLAY;
                dataUARTCmdPrint( pUserCmdIF,
                             "GAPBOND_IO_CAP_KEYBOARD_DISPLAY\r\n%s",
                             pUserCmdIF->cPrompt );
                break;   
            default:
                dataUARTCmdPrint( pUserCmdIF,
                             "GAPBOND_IO_CAP parameter error!!!\r\n%s",
                             pUserCmdIF->cPrompt );
                break;
        }
    }
    else
    {
        dataUARTCmdPrint( pUserCmdIF,
                             "GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT\r\n%s",
                             pUserCmdIF->cPrompt );
    }
    GAPBondMgr_SetParameter( GAPBOND_IO_CAPABILITIES, sizeof ( uint8_t ), &ioCap );
    GAPBondMgr_SetPairable();

    return ( ResultOk );
}

static TUserCmdParseResult UserCmdScan(TUserCmdIF * pUserCmdIF,TUserCmdParseValue *pParseValue)
{
    dataUARTCmdPrint( pUserCmdIF,"UserCmdScan\r\n%s",pUserCmdIF->cPrompt );
    central_StartScan();
    return ( ResultOk ); 
}

static TUserCmdParseResult UserCmdStopScan(TUserCmdIF * pUserCmdIF,TUserCmdParseValue *pParseValue)
{
    dataUARTCmdPrint( pUserCmdIF,"UserCmdStopScan\r\n%s",pUserCmdIF->cPrompt );
    central_StopScan();
    return ( ResultOk ); 
}

static TUserCmdParseResult UserCmdSecurityReq(TUserCmdIF * pUserCmdIF,TUserCmdParseValue *pParseValue)
{
    dataUARTCmdPrint( pUserCmdIF,"UserCmdSecurityReq\r\n%s",pUserCmdIF->cPrompt );
    GAPBondMgr_Pair();
    return ( ResultOk ); 
}

static TUserCmdParseResult UserCmdConUpdateReq(TUserCmdIF * pUserCmdIF,TUserCmdParseValue *pParseValue)
{
    dataUARTCmdPrint( pUserCmdIF,"UserCmdConUpdateReq\r\n%s",pUserCmdIF->cPrompt );
    central_SendUpdateParam();
    return ( ResultOk ); 
}

static TUserCmdParseResult UserCmdRemoteBdSet(TUserCmdIF * pUserCmdIF,TUserCmdParseValue *pParseValue)
{
    uint8_t DestAddr[6] = {0};
    uint8_t addr_len;
    dataUARTCmdPrint( pUserCmdIF,"UserCmdRemoteBdSet\r\n%s",pUserCmdIF->cPrompt );

    for(addr_len = 0; addr_len < B_ADDR_LEN; addr_len++)
    {
        DestAddr[addr_len] = pParseValue->dwParameter[B_ADDR_LEN - addr_len - 1];
    }
    
    centralSetGapParameter(GAPPRRA_CONN_DEST_ADDR, B_ADDR_LEN, DestAddr);
    return ( ResultOk ); 
}

static TUserCmdParseResult UserCmdServiceDiscovery(TUserCmdIF * pUserCmdIF,TUserCmdParseValue *pParseValue)
{        
    dataUARTCmdPrint( pUserCmdIF,"UserCmdServiceDiscovery\r\n%s",pUserCmdIF->cPrompt );
    
    clientAPI_AllPrimarySrvDiscovery();

    return ( ResultOk ); 
}

static TUserCmdParseResult UserCmdRelationshipDiscovery(TUserCmdIF * pUserCmdIF,TUserCmdParseValue *pParseValue)
{
    TRelationDiscReq relationReq;
    
    dataUARTCmdPrint( pUserCmdIF,"UserCmdRelationshipDiscovery\r\n%s",pUserCmdIF->cPrompt );
    relationReq.wStartHandle = (uint16_t)pParseValue->dwParameter[0];
    relationReq.wEndHandle = (uint16_t)pParseValue->dwParameter[1];
    if(relationReq.wStartHandle > relationReq.wEndHandle)
    {
        dataUARTCmdPrint( pUserCmdIF,"UserCmdRelationshipDiscovery: user input param err, please check!\r\n%s",pUserCmdIF->cPrompt );
        return (ResultError);
    }
    clientAPI_RelationshipDiscovery( 0, relationReq );
    return ( ResultOk );
}

static TUserCmdParseResult UserCmdCharDiscovery(TUserCmdIF * pUserCmdIF,TUserCmdParseValue *pParseValue)
{
    TCharDiscReq charReq;

    dataUARTCmdPrint( pUserCmdIF,"UserCmdCharDiscovery\r\n%s",pUserCmdIF->cPrompt );
    
    charReq.wStartHandle = (uint16_t)pParseValue->dwParameter[0];
    charReq.wEndHandle = (uint16_t)pParseValue->dwParameter[1];
    if(charReq.wStartHandle > charReq.wEndHandle)
    {
        dataUARTCmdPrint( pUserCmdIF,"UserCmdCharDiscovery: user input param err, please check!\r\n%s",pUserCmdIF->cPrompt );
        return (ResultError);
    }
    clientAPI_AllCharDiscovery( 0, charReq );
    return ( ResultOk ); 
}

static TUserCmdParseResult UserCmdCharDescriptorDiscovery(TUserCmdIF * pUserCmdIF,TUserCmdParseValue *pParseValue)
{
    TCharDescriptorDiscReq charDescriptorReq;

    dataUARTCmdPrint( pUserCmdIF,"UserCmdCharDescriptorDiscovery\r\n%s",pUserCmdIF->cPrompt );
    
    charDescriptorReq.wStartHandle = (uint16_t)pParseValue->dwParameter[0];
    charDescriptorReq.wEndHandle = (uint16_t)pParseValue->dwParameter[1];
    if(charDescriptorReq.wStartHandle > charDescriptorReq.wEndHandle)
    {
        dataUARTCmdPrint( pUserCmdIF,"UserCmdCharDescriptorDiscovery: user input param err, please check!\r\n%s",pUserCmdIF->cPrompt );
        return (ResultError);
    }
    clientAPI_AllCharDescriptorDiscovery( 0, charDescriptorReq );
    return ( ResultOk );
}

static TUserCmdParseResult UserCmdAttribRead(TUserCmdIF * pUserCmdIF,TUserCmdParseValue *pParseValue)
{
    TReadHandleReq readHandle;
    
    dataUARTCmdPrint( pUserCmdIF,"UserCmdAttribRead\r\n%s",pUserCmdIF->cPrompt );
    readHandle.wHandle = (uint16_t)pParseValue->dwParameter[0];
    clientAPI_AttribRead( 0, readHandle );
    return ( ResultOk ); 
}

static TUserCmdParseResult UserCmdAttribReadUUID(TUserCmdIF * pUserCmdIF,TUserCmdParseValue *pParseValue)
{
	TReadUUIDReq readUUIDReq;

    dataUARTCmdPrint( pUserCmdIF,"UserCmdAttribReadUUID\r\n%s",pUserCmdIF->cPrompt );
	readUUIDReq.UUID16 = (uint16_t)pParseValue->dwParameter[2];
	readUUIDReq.wStartHandle = (uint16_t)pParseValue->dwParameter[0];
	readUUIDReq.wEndHandle = (uint16_t)pParseValue->dwParameter[1];
    if(readUUIDReq.wStartHandle > readUUIDReq.wEndHandle)
    {
        dataUARTCmdPrint( pUserCmdIF,"UserCmdAttribReadUUID: user input param err, please check!\r\n%s",pUserCmdIF->cPrompt );
        return (ResultError);
    }
	clientAPI_AttribReadUsingUUID(0, readUUIDReq);
    return ( ResultOk ); 
}

static TUserCmdParseResult UserCmdHandleDisplay(TUserCmdIF * pUserCmdIF, TUserCmdParseValue *pParseValue)
{
    /* Indicate which specific client module to show. */
    uint8_t module_type;
    TSimpClientHandleType hdl_idx;
    uint16_t temp_handle;
    dataUARTCmdPrint( pUserCmdIF,"UserCmdHandleDisplay\r\n%s",pUserCmdIF->cPrompt );

    module_type = (uint8_t)pParseValue->dwParameter[0];

    switch(module_type)
    {
        case 0:/* Show Simple BLE Client discovered handle info. */
            for(hdl_idx = HDL_SIMBLE_SRV_START; hdl_idx < HDL_SIMBLE_CACHE_LEN; hdl_idx++)
            {
                temp_handle = SimpBleClient_GetHandle(hdl_idx);
                dataUARTCmdPrint( pUserCmdIF,"-->Index %d -- Handle 0x%x\r\n%s",hdl_idx,temp_handle,pUserCmdIF->cPrompt );
            }
            break;
        default:
            dataUARTCmdPrint( pUserCmdIF,"UserCmdHandleDisplay: user input param err, please check!\r\n%s",pUserCmdIF->cPrompt );
            break;
    }
    return ( ResultOk ); 
}

static TUserCmdParseResult UserCmdSimpStartDiscover(TUserCmdIF * pUserCmdIF, TUserCmdParseValue *pParseValue)
{
    dataUARTCmdPrint( pUserCmdIF,"UserCmdSimpStartDiscover\r\n%s",pUserCmdIF->cPrompt );
    SimpBleClient_StartDiscovery();
    return ( ResultOk );
}

static TUserCmdParseResult UserCmdSimpReadChar(TUserCmdIF * pUserCmdIF, TUserCmdParseValue *pParseValue)
{
    /* Indicate which char to be read. */
    TSimpClientReadType read_char_type = (TSimpClientReadType)pParseValue->dwParameter[0];
    /* Read by handle or UUID, 1--by UUID, 0--by handle. */
    uint8_t read_pattern = (uint8_t)pParseValue->dwParameter[1];
    dataUARTCmdPrint( pUserCmdIF,"UserCmdSimpReadChar\r\n%s",pUserCmdIF->cPrompt );

    if(read_pattern)
    {
        SimpBleClient_ReadByUUID(read_char_type);
    }
    else
    {
        SimpBleClient_ReadByHandle(read_char_type);
    }

    return ( ResultOk );
}

static TUserCmdParseResult UserCmdSimpCccdOperate(TUserCmdIF * pUserCmdIF, TUserCmdParseValue *pParseValue)
{
    /* Indicate which char CCCD command. */
    uint8_t cmd_type = (uint8_t)pParseValue->dwParameter[0];
    /* Enable or disable, 1--enable, 0--disable. */
    bool cmd_data = (bool)pParseValue->dwParameter[1];
    
    dataUARTCmdPrint( pUserCmdIF,"UserCmdSimpCccdOperate\r\n%s",pUserCmdIF->cPrompt );

    switch(cmd_type)
    {
        case 0:/* V3 Notify char notif enable/disable. */
            SimpBleClient_V3NotifyCmd(cmd_data);
            break;
        case 1:/* V4 Indicate char indicate enable/disable. */
            SimpBleClient_V4IndicateCmd(cmd_data);
            break;
        case 2:/* V5 Control Point char indicate enable/disable. */
            SimpBleClient_V5CtlPntIndicateCmd(cmd_data);
            break;
        default:
            dataUARTCmdPrint( pUserCmdIF,"UserCmdSimpCccdOperate: user input param err, please check!\r\n%s",pUserCmdIF->cPrompt );
            break;
    }

    return ( ResultOk );
}

static TUserCmdParseResult UserCmdSimpWriteChar(TUserCmdIF * pUserCmdIF, TUserCmdParseValue *pParseValue)
{
    /* Indicate which char to be written. */
    uint8_t write_type = (uint8_t)pParseValue->dwParameter[0];
    
    dataUARTCmdPrint( pUserCmdIF,"UserCmdSimpWriteChar\r\n%s",pUserCmdIF->cPrompt );

    switch(write_type)
    {
        case 0:/* Write the Control Point, to set the V4 Indicate char value on peer server. */
            SimpBleClient_CtlPntSetV4Indicate((uint8_t)pParseValue->dwParameter[1]);
            break;
        case 1:/* Write the Control Point, to set the V3 Notify char value on peer server. */
            SimpBleClient_CtlPntSetV3Notify((uint8_t)pParseValue->dwParameter[1]);
            break;
        case 2:/* Write the V2 Writable char value on peer server. */
            SimpBleClient_WriteV2WriteChar((uint8_t)pParseValue->dwParameter[1]);
            break;
        default:
            break;
    }

    return ( ResultOk );
}

static TUserCmdParseResult UserCmdTxPowerSet(TUserCmdIF * pUserCmdIF, TUserCmdParseValue *pParseValue)
{
    /* Indicate which char to be written. */
    TBLE_TX_POWER_INDEX tx_power_index = (TBLE_TX_POWER_INDEX)pParseValue->dwParameter[0];
    
    dataUARTCmdPrint( pUserCmdIF,"UserCmdTxPowerSet\r\n%s",pUserCmdIF->cPrompt );

    central_SetTxPower(tx_power_index);

    return ( ResultOk );
}

static TUserCmdParseResult UserCmdRssiRead(TUserCmdIF * pUserCmdIF, TUserCmdParseValue *pParseValue)
{
    dataUARTCmdPrint( pUserCmdIF,"UserCmdRssiRead\r\n%s",pUserCmdIF->cPrompt );

    central_ReadRSSI();

    return ( ResultOk );
}

static TUserCmdParseResult UserCmdPatchVerGet(TUserCmdIF * pUserCmdIF, TUserCmdParseValue *pParseValue)
{
    uint16_t patchVersion;
    dataUARTCmdPrint( pUserCmdIF,"UserCmdPatchVerGet\r\n%s",pUserCmdIF->cPrompt );

    blueAPI_ReadPatchVersion(&patchVersion);

    dataUARTCmdPrint( pUserCmdIF,"UserCmdPatchVerGet: version = %d\r\n%s",patchVersion,pUserCmdIF->cPrompt );

    return ( ResultOk );
}

static TUserCmdParseResult UserCmdChanClassSet(TUserCmdIF * pUserCmdIF, TUserCmdParseValue *pParseValue)
{
    uint8_t chanMap[5] = {0};
    uint8_t i;
    for(i = 0; i < 5; i++)
    {
        chanMap[i] = (uint8_t)pParseValue->dwParameter[i];
    }
    dataUARTCmdPrint( pUserCmdIF,"UserCmdChanClassSet\r\n%s",pUserCmdIF->cPrompt );
    chanMap[4] = chanMap[4] & 0x1F;

    central_SetHostChanClass( chanMap );

    return ( ResultOk );
}

