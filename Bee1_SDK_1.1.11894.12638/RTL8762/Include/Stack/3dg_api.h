#ifndef _3DG_API_H_
#define _3DG_API_H_

#include "basic_types.h"

#define DG3_EVENT_MSG         0x02


/** @brief message type definitions */
typedef enum
{
    blueAPI_EventHciInitResp = 1,
    blueAPI_EventHciEvtInd
} TBlueAPI_HCI_OP;
typedef TBlueAPI_HCI_OP * PBlueAPI_HCI_OP;

//HCI init req/resp
typedef struct
{
    void *AppCallBack;
} TBlueAPI_HciInterfaceInitReq;

typedef struct
{
    uint8_t    cause;
} TBlueAPI_HciInterfaceInitResp;
typedef TBlueAPI_HciInterfaceInitResp * PBlueAPI_HciInterfaceInitResp;



//HCI Evt Indication
typedef struct
{
    uint8_t hci_evt_buffer[257];
} TBlueAPI_HciEvtInd;
typedef TBlueAPI_HciEvtInd * PBlueAPI_HciEvtInd;



typedef union {
    TBlueAPI_HciInterfaceInitResp HciInterfaceInitResp;

    TBlueAPI_HciEvtInd         HciEvtInd;
} TBlueAPI_HciUsData;



typedef struct _TBlueAPI_HciDsMessage
{
    uint8_t Command;
    uint16_t Length;
    TBlueAPI_HciUsData HciUsData;
} TBlueAPI_HciUsMessage;
typedef TBlueAPI_HciUsMessage * PBlueAPI_HciUsMessage;


typedef void ( * PBlueAPI_EvtCallBack)(PBlueAPI_HciUsMessage pMsg);

//export Apis
/**
 * @brief  Initialize HCI interface.
 *
 *  Initialize HCI inteface first, after successfully initialize HCI interface,
 *  App can send hci command to and receive hci event from Bee Stack.
 *  Use HCI interface API, app can implement 3DG profile.
 * @param AppCallBack  Callback function provided by the APP to handle messages sent from the BT Stack.
 * @retval TRUE successfully
 * @retval FALSE failed
*/
BOOL blueAPI_HciInterfaceInit( void *AppCallBack);

/**
 * @brief  Send HCI Command Packet to stack.
 *
 *  The HCI Command Packet is used to send commands to the stack from
 *  the app.
 * @param opCode  The Opcode parameter is divided into two fields, called
 *                          the OpCode Group Field (OGF) and OpCode Command Field (OCF). The
 *                          OGF occupies the upper 6 bits of the Opcode, while the OCF occupies the
 *                          remaining 10 bits.
 * @param paralen  Lengths of all of the parameters contained in this packet measured in
 *                          octets. (N.B.: total length of parameters, not number of parameters).
 * @param pPara  pointer to parameters
 * @retval TRUE successfully
 * @retval FALSE failed
*/
BOOL blueAPI_SendHciComamand(uint16_t opCode, uint8_t paralen, uint8_t *pPara);

/**
 * @brief  Get buffer to send hci command.
 *
 * Get the buffer from the data ram pool.
 * @param len   The buffer length to allocate.
 * @return  void *
*/
void * blueAPI_CmdBufferGet(uint16_t len);

/**
 * @brief  Release buffer get by blueAPI_CmdBufferGet.
 *
 * @param pBuffer  Pointer to memory that need to release (address of begin of message header).
 * @return  void
*/
void blueAPI_CmdBufferRelease(void * pBuffer);

/**
 * @brief  Release Event buffer.
 *
 * @param pBuffer  Pointer to memory that need to release (address of begin of message header).
 * @return  void
*/
void blueAPI_EvtBufferRelease(void * pBuffer);
#endif

