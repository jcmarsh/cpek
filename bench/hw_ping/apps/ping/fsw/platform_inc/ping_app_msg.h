/*******************************************************************************
** File:
**   ping_app_msg.h 
**
** Purpose: 
**  Define Ping App  Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef _ping_app_msg_h_
#define _ping_app_msg_h_

/*
** Ping App command codes
*/
#define PING_APP_NOOP_CC                 0
#define PING_APP_RESET_COUNTERS_CC       1

/*************************************************************************/
/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} PING_NoArgsCmd_t;

/*************************************************************************/
/*
** Type definition (Ping App housekeeping)
*/
typedef struct 
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              ping_command_error_count;
    uint8              ping_command_count;
    uint8              spare[2];

}   OS_PACK ping_hk_tlm_t  ;

#define PING_APP_HK_TLM_LNGTH   sizeof ( ping_hk_tlm_t )

#endif /* _ping_app_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
