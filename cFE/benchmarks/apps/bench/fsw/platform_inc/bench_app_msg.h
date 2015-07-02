/*******************************************************************************
** File:
**   bench_app_msg.h 
**
** Purpose: 
**  Define Bench App  Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef _bench_app_msg_h_
#define _bench_app_msg_h_

/*
** Bench App command codes
*/
#define BENCH_APP_NOOP_CC                 0
#define BENCH_APP_RESET_COUNTERS_CC       1

/*************************************************************************/
/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} BENCH_NoArgsCmd_t;

/*************************************************************************/
/*
** Type definition (Bench App housekeeping)
*/
typedef struct 
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              bench_command_error_count;
    uint8              bench_command_count;
    uint8              spare[2];

}   OS_PACK bench_hk_tlm_t  ;

#define BENCH_APP_HK_TLM_LNGTH   sizeof ( bench_hk_tlm_t )

#endif /* _bench_app_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
