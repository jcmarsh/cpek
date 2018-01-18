/*******************************************************************************
** File:
**   pong_app_msg.h 
**
** Purpose: 
**  Define Pong App  Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef _pong_app_msg_h_
#define _pong_app_msg_h_

/*
** Pong App command codes
*/
#define PONG_APP_NOOP_CC                 0
#define PONG_APP_RESET_COUNTERS_CC       1
#define PONG_PING_CC                     2

/*************************************************************************/
/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} PONG_NoArgsCmd_t;

/*************************************************************************/
/*
** Type definition (Pong App housekeeping)
*/
typedef struct 
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              pong_command_error_count;
    uint8              pong_command_count;
    uint8              spare[2];

}   OS_PACK pong_hk_tlm_t  ;

#define PONG_APP_HK_TLM_LNGTH   sizeof ( pong_hk_tlm_t )

#endif /* _pong_app_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
