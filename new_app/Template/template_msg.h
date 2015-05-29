/*******************************************************************************
** File:
**   $application_name$_app_msg.h 
**
** Purpose: 
**  Define $Application Name$ App  Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef _$application_name$_app_msg_h_
#define _$application_name$_app_msg_h_

/*
** $Application Name$ App command codes
*/
#define $APPLICATION_NAME$_APP_NOOP_CC                 0
#define $APPLICATION_NAME$_APP_RESET_COUNTERS_CC       1

/*************************************************************************/
/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} $APPLICATION_NAME$_NoArgsCmd_t;

/*************************************************************************/
/*
** Type definition ($Application Name$ App housekeeping)
*/
typedef struct 
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              $application_name$_command_error_count;
    uint8              $application_name$_command_count;
    uint8              spare[2];

}   OS_PACK $application_name$_hk_tlm_t  ;

#define $APPLICATION_NAME$_APP_HK_TLM_LNGTH   sizeof ( $application_name$_hk_tlm_t )

#endif /* _$application_name$_app_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
