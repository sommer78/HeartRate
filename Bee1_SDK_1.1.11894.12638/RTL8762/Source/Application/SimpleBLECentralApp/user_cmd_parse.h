/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     user_cmd_parse.h
* @brief    Parse user command from lower Data UART data.
* @details  Structure of command parse results.
* @author
* @date     2015-03-19
* @version  v0.1
*********************************************************************************************************
*/
#ifndef __USER_CMD_PARSE_CMD_H
#define __USER_CMD_PARSE_CMD_H

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/****************************************************************************************************************
* macros that other .c files may use all defined here
****************************************************************************************************************/
/** @brief  command parse related macros. */
#define USER_CMD_MAX_COMMAND_LINE   80           /* max. length of command line in bytes */
#define USER_CMD_MAX_BUFFER_LENGTH  256          /* max. length of buffer in bytes */
#define USER_CMD_MAX_RESULT_STRING  256          /* max. length of result string */
#define USER_CMD_MAX_PARAMETERS     6            /* max. number of parameters that the parser will scan */
#define USER_CMD_MAX_HELP_TXT_TX_WSIZE   2       /* max. tx window size for help text output */

/**
 * @brief Data UART command parse value.
 *
 * This is the structure where the command line parser puts its values.
*/
typedef struct _TUserCmdParseValue
{
    char      * pCommand;                    /* pointer to command */
    char      * pRestOfCommandLine;          /* pointer to rest of command line for manual inspection */
    int32_t     iParameterCount;             /* number of found parameters */
    uint32_t    dwParameter[USER_CMD_MAX_PARAMETERS]; /* automatically parsed parameters */
    char      * pParameter[USER_CMD_MAX_PARAMETERS];  /* automatically parsed parameters */
} TUserCmdParseValue;

/**
 * @brief Data UART command parse result.
 *
 * This is the structure where the command line parser puts its result.
*/
typedef enum _TUserCmdParseResult
{
    ResultOk,
    ResultError,
    ResultEmptyCommandLine,
    ResultCommandNotFound,
    ResultWrongParameter,
    ResultWrongNumberOfParameters,
    ResultValueOutOfRange
} TUserCmdParseResult;

/**
 * @brief command interface.
 *.
*/
typedef struct _UserCmdIF
{
    char      cCommandLine[USER_CMD_MAX_COMMAND_LINE];
    char      cBuffer[USER_CMD_MAX_BUFFER_LENGTH];  /* work buffer */
    int32_t   iCommandLength;              /* accumulated length of command       */
    char *    pTxBuffer;                   /* buffer for echo of input            */
    int32_t   iTxLength;                   /* < 0: tx buffer must be allocated    */
    int32_t   iHelpTextSentIdx;            /* index of last help text string sent */
    bool      HelpTextInProgress;
    bool      SendPrompt;
    char      cPrompt[2];
    char      cCrLf[3];
} TUserCmdIF, * PUserCmdIF;

/**
 * @brief functions that can be called from command table.
 *
*/
typedef TUserCmdParseResult (*TUserCmdFunc)(TUserCmdIF * pUserCmdIF,
        TUserCmdParseValue *pParseValue);

/**
 * @brief command table entry.
 *
*/
typedef struct _TUserCmdTableEntry
{
    char              * pCommand;
    char              * pOption;
    char              * pHelp;
    TUserCmdFunc    Func;
} TUserCmdTableEntry;

/****************************************************************************************************************
* exported functions other .c files may use all defined here.
****************************************************************************************************************/
/* initiate command interface struct */
extern void User_CmdInit( TUserCmdIF *pUserCmdIF );
/* collect user command from Data UART strings */
extern bool User_CmdCollect( TUserCmdIF * pUserCmdIF,
                                    uint8_t * pData,
                                    int32_t iLength,
                                    const TUserCmdTableEntry * pCmdTable );

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif /* __USER_CMD_PARSE_CMD_H */

