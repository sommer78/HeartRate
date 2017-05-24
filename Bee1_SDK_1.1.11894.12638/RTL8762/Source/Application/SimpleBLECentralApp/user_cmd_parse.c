enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     user_cmd_parse.c
* @brief    Parse user command from lower Data UART data.
* @details  Parse user commands and execute right commands.
* @author
* @date     2015-03-19
* @version  v0.1
*********************************************************************************************************
*/
#include <string.h>
#include "data_uart.h"
#include "user_cmd_parse.h"

/** @brief  this external function for comparing string. */
extern int32_t _strcmp(const char *cs, const char *ct);

/**
 * @brief  Check, if a character is a white space.
 *
 * @param c         char data to check.
 * @return check result.
 * @retval 1  TRUE.
 * @retval 1  FALSE.
*/
static bool User_CmdIsWhiteSpace(char c)
{
    return (((c >= 9) && (c <= 13)) || (c == 32));
}

/**
 * @brief  Skip white spaces in buffer.
 *
 * @param buffer
 * @return pointer to skipped white spaces' new buffer.
*/
static char *User_CmdSkipSpaces(char *buffer)
{
    char *p = buffer;

    while (User_CmdIsWhiteSpace(*p)) /* white space */
    {
        p++;
    }
    return p;
}

/**
 * @brief  Find end of a word.
 *
 * @param buffer
 * @return
*/
static char *User_CmdFindEndOfWord(char *buffer)
{
    char *p = buffer;

    while (!User_CmdIsWhiteSpace(*p) && (*p != '\0'))
    {
        p++;
    }
    return p;
}

/**
 * @brief  Read ASCII string and convert to uint32_t.
 *
 * @param p
 * @return
*/
static uint32_t User_CmdString2uint32_t(char *p)
{
    uint32_t result = 0;
    char     ch;
    bool     hex = false;

    /* check if value is dec */
    if (p[0] == 'x')
    {
        hex = true;
        p = &p[1];
    }
    else if ((p[0] == '0') && (p[1] == 'x'))
    {
        hex = true;
        p = &p[2];
    }

    for (;;)
    {
        ch = *( p++ ) | 0x20;               /* convert to lower case */

        if ( hex)                           /* dec value */
        {
            /* hex value */
            if ((ch >= 'a') && (ch <= 'f'))
            {
                ch -= ('a' - 10);
            }
            else if ((ch >= '0') && (ch <= '9'))
            {
                ch -= '0';
            }
            else
            {
                break;
            }
            result =  (result << 4);
            result += (ch & 0x0f);
        }
        else
        {
            if ( ch < '0' || ch > '9' )
                break;  /* end of string reached */
            result = 10 * result + ch - '0';
        }
    }
    return (result);
}

/**
 * @brief  Send result, display in UART Assitant.
 *
 * @param pUserCmdIF command parsed.
 * @param Result                command parse result.
 * @return none
*/
static void User_CmdSendResult(TUserCmdIF * pUserCmdIF, TUserCmdParseResult Result)
{
    switch (Result)
    {
    case ResultError:
        dataUARTUserIFSendString(pUserCmdIF, "Error\n\r", NULL);
        break;
    case ResultCommandNotFound:
        dataUARTUserIFSendString(pUserCmdIF, "Command not found\n\r", NULL);
        break;
    case ResultWrongNumberOfParameters:
        dataUARTUserIFSendString(pUserCmdIF, "Wrong number of parameters\n\r", NULL);
        break;
    case ResultWrongParameter:
        dataUARTUserIFSendString(pUserCmdIF, "Wrong parameter\n\r", NULL);
        break;
    case ResultValueOutOfRange:
        dataUARTUserIFSendString(pUserCmdIF, "Value out of range\n\r", NULL);
        break;
    default:
        break;
    }
}

/**
 * @brief  flush buffer with echo characters.
 *
 * @param pUserCmdIF command parsed.
 * @return none
*/
static void User_CmdFlushEchoChars( TUserCmdIF * pUserCmdIF )
{
    if ( pUserCmdIF->iTxLength > 0 )
    {
        dataUARTUserIFSendChar(pUserCmdIF, 0);     /* flush buffer with echo data */
        pUserCmdIF->iTxLength   = 0;
        pUserCmdIF->pTxBuffer   = NULL;
    }
}

/**
 * @brief  execute cmd.
 *
 * @param pUserCmdIF command parsed.
 * @param pParseValue       command parse value.
 * @param pCmdTable         command table, include user self-definition command function.
 * @return command execute result.
*/
static TUserCmdParseResult User_CmdExecute( TUserCmdIF * pUserCmdIF,
        TUserCmdParseValue * pParseValue,
        const TUserCmdTableEntry * pCmdTable )
{
    int32_t              i = 0;
    TUserCmdParseResult  Result = ResultCommandNotFound;

    /* find command in table */
    while ((pCmdTable + i)->pCommand != NULL)
    {
        if ( _strcmp((const char *)(pCmdTable + i)->pCommand,
                     (const char *)pParseValue->pCommand) == 0 )
        {
            /* command found */

            /* check if user wants help */
            if (pParseValue->iParameterCount && *pParseValue->pParameter[0] == '?')
            {
                dataUARTUserIFSendString(pUserCmdIF, (pCmdTable + i)->pOption, NULL);
                Result = ResultOk;
            }
            else
            {
                /* execute command function */
                Result = (pCmdTable + i)->Func(pUserCmdIF, pParseValue);
            }
            /* exit while */
            break;
        }
        i++;
    };

    return ( Result );
}

/**
 * @brief  Parse a command line and return the found
 *       command and parameters in "pParseValue"
 *
 * @param pUserCmdIF command parsed.
 * @param pParseValue       command parse value.
 * @return command parse result.
*/
static TUserCmdParseResult User_CmdParse( TUserCmdIF * pUserCmdIF,
        TUserCmdParseValue * pParseValue )
{
    int32_t              i;
    TUserCmdParseResult  Result;
    char            *p, *q;

    /* clear all results */
    Result = ResultOk;
    pParseValue->pCommand            = NULL;
    pParseValue->pRestOfCommandLine  = NULL;
    pParseValue->iParameterCount     = 0;
    for (i = 0 ; i < USER_CMD_MAX_PARAMETERS; i++)
    {
        pParseValue->pParameter[i]     = NULL;
        pParseValue->dwParameter[i]    = 0;
    }

    /* Parse line */
    p = pUserCmdIF->cCommandLine;

    /*ignore leading spaces */
    p = User_CmdSkipSpaces(p);
    if ( *p == '\0')                      /* empty command line ? */
    {
        Result = ResultEmptyCommandLine;
    }
    else
    {
        /* find end of word */
        q = User_CmdFindEndOfWord( p);
        if (p == q)                        /* empty command line ? */
        {
            Result = ResultEmptyCommandLine;
        }
        else                                /* command found */
        {
            pParseValue->pCommand = p;
            *q = '\0';                        /* mark end of command */
            p = q + 1;

            /* parse parameters */
            if ( *p != '\0')                  /* end of line ? */
            {
                int32_t j;

                pParseValue->pRestOfCommandLine = p;

                j = 0;
                do
                {
                    uint32_t d;
                    /* ignore leading spaces */
                    p = User_CmdSkipSpaces(p);
                    d = User_CmdString2uint32_t(p);

                    pParseValue->pParameter[j]    = p;
                    pParseValue->dwParameter[j++] = d;

                    if ( j >= USER_CMD_MAX_PARAMETERS)
                    {
                        break;
                    }

                    /* find next parameter */
                    p  = User_CmdFindEndOfWord( p);
                    *p++ = '\0';                        /* mark end of parameter */
                }
                while (*p != '\0');
                pParseValue->iParameterCount = j;
            }
        }
    }

    return ( Result );
}

/**
 * @brief  clear command line buffer.
 *
 * @param pUserCmdIF command parsed.
 * @return none.
*/
static void User_CmdClearCommand( TUserCmdIF * pUserCmdIF )
{
    pUserCmdIF->iCommandLength = 0;
    memset( pUserCmdIF->cCommandLine, 0,
            sizeof(pUserCmdIF->cCommandLine) );
}

/**
 * @brief  print service id related string.
 *
 * @param none.
 * @return print service id related string.
*/
static char * User_CmdServiceID( void )
{
    static char *pStr;

    pStr = "simpleBLECentralApp";
    return ( pStr );
}

/**
 * @brief  collect cmd characters.
 *
 * @param pUserCmdIF store parsed commands.
 * @param pData             data to be parsed.
 * @param iLength               length of data to be command parsed.
 * @param pCmdTable         command table to execute function.
 * @return command collect result.
 * @retval 1 TRUE.
 * @retval 0 FALSE.
*/
bool User_CmdCollect( TUserCmdIF * pUserCmdIF,
                             uint8_t * pData,
                             int32_t iLength,
                             const TUserCmdTableEntry * pCmdTable )
{
    TUserCmdParseValue ParseResult;
    char                 c;

    pUserCmdIF->iTxLength   = -iLength;       /* < 0: tx buffer must be allocated */
    pUserCmdIF->pTxBuffer   = NULL;

    /* discard rx data as long help text output is in progress */
    while (!pUserCmdIF->HelpTextInProgress && iLength--)
    {
        c = *pData++;
        if (c != 0x0)                       /* not ESC character received */
        {
            switch (c)                        /* Normal handling */
            {
            case '\n':
            case '\r':                      /* end of line */
                User_CmdFlushEchoChars( pUserCmdIF );
                pUserCmdIF->SendPrompt = true;

                dataUARTUserIFSendString(pUserCmdIF, pUserCmdIF->cCrLf, NULL);
                if (pUserCmdIF->iCommandLength > 0)  /* at least one character in command line ? */
                {
                    TUserCmdParseResult Result;

                    pUserCmdIF->cCommandLine[pUserCmdIF->iCommandLength] = '\0';
                    Result = User_CmdParse(pUserCmdIF, &ParseResult);
                    if (Result == ResultOk)
                        Result = User_CmdExecute(pUserCmdIF, &ParseResult, pCmdTable);

                    if (Result != ResultOk)
                        User_CmdSendResult(pUserCmdIF, Result);
                }

                if (pUserCmdIF->SendPrompt)
                    dataUARTUserIFSendString(pUserCmdIF, pUserCmdIF->cPrompt, NULL);

                User_CmdClearCommand( pUserCmdIF );

                /* maybe more than 1 cmd in pData: */
                pUserCmdIF->iTxLength   = -iLength;
                pUserCmdIF->pTxBuffer   = NULL;
                break;
            case '\b':                        /* backspace */
                if (pUserCmdIF->iCommandLength > 0)
                {
                    pUserCmdIF->iCommandLength--;
                    pUserCmdIF->cCommandLine[pUserCmdIF->iCommandLength] = '\0';
                }
                if ( pUserCmdIF->iTxLength < 0 )
                    pUserCmdIF->iTxLength -= 2;     /* OK only if BS is first char ... */
                dataUARTUserIFSendChar(pUserCmdIF, c);
                dataUARTUserIFSendChar(pUserCmdIF, ' ');
                dataUARTUserIFSendChar(pUserCmdIF, c);
                break;
            default:
                /* Put character in command buffer */
                if (pUserCmdIF->iCommandLength < USER_CMD_MAX_COMMAND_LINE)
                {
                    dataUARTUserIFSendChar(pUserCmdIF, c);
                    pUserCmdIF->cCommandLine[pUserCmdIF->iCommandLength++] = c;
                }
                break;
            }
        }
    }

    User_CmdFlushEchoChars( pUserCmdIF );

    return ( true );
}

/**
 * @brief   init. cmd set.
 *
 * @param pUserCmdIF store parsed commands.
 * @return none.
*/
void User_CmdInit( TUserCmdIF *pUserCmdIF )
{
    memset( pUserCmdIF, 0, sizeof(TUserCmdIF) );
    pUserCmdIF->cCrLf[0]   = '\r';
    pUserCmdIF->cCrLf[1]   = '\n';
    pUserCmdIF->cPrompt[0] = '>';

    dataUARTCmdPrint( pUserCmdIF, ">> Command Parse Init (%s) <<\r\n%s",
                         User_CmdServiceID(),
                         pUserCmdIF->cPrompt );
}

