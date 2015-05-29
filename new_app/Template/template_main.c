/*******************************************************************************
** File: $application_name$_app.c
**
** Purpose:
**   This file contains the source code for the $Application Name$ App.
**
*******************************************************************************/

/*
**   Include Files:
*/

#include "$application_name$_app.h"
#include "$application_name$_app_perfids.h"
#include "$application_name$_app_msgids.h"
#include "$application_name$_app_msg.h"
#include "$application_name$_app_events.h"
#include "$application_name$_app_version.h"

/*
** global data
*/

$application_name$_hk_tlm_t    $APPLICATION_NAME$_HkTelemetryPkt;
CFE_SB_PipeId_t    $APPLICATION_NAME$_CommandPipe;
CFE_SB_MsgPtr_t    $APPLICATION_NAME$_MsgPtr;

static CFE_EVS_BinFilter_t  $APPLICATION_NAME$_EventFilters[] =
       {  /* Event ID    mask */
          {$APPLICATION_NAME$_STARTUP_INF_EID,       0x0000},
          {$APPLICATION_NAME$_COMMAND_ERR_EID,       0x0000},
          {$APPLICATION_NAME$_COMMANDNOP_INF_EID,    0x0000},
          {$APPLICATION_NAME$_COMMANDRST_INF_EID,    0x0000},
       };

/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* $APPLICATION_NAME$_AppMain() -- Application entry point and main process loop          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void $APPLICATION_NAME$_AppMain( void )
{
    int32  status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    CFE_ES_PerfLogEntry($APPLICATION_NAME$_APP_PERF_ID);

    $APPLICATION_NAME$_AppInit();

    /*
    ** $APPLICATION_NAME$ Runloop
    */
    while (CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        CFE_ES_PerfLogExit($APPLICATION_NAME$_APP_PERF_ID);

        /* Pend on receipt of command packet -- timeout set to 500 millisecs */
        status = CFE_SB_RcvMsg(&$APPLICATION_NAME$_MsgPtr, $APPLICATION_NAME$_CommandPipe, 500);
        
        CFE_ES_PerfLogEntry($APPLICATION_NAME$_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            $APPLICATION_NAME$_ProcessCommandPacket();
        }

    }

    CFE_ES_ExitApp(RunStatus);

} /* End of $APPLICATION_NAME$_AppMain() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* $APPLICATION_NAME$_AppInit() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void $APPLICATION_NAME$_AppInit(void)
{
    /*
    ** Register the app with Executive services
    */
    CFE_ES_RegisterApp() ;

    /*
    ** Register the events
    */ 
    CFE_EVS_Register($APPLICATION_NAME$_EventFilters,
                     sizeof($APPLICATION_NAME$_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    /*
    ** Create the Software Bus command pipe and subscribe to housekeeping
    **  messages
    */
    CFE_SB_CreatePipe(&$APPLICATION_NAME$_CommandPipe, $APPLICATION_NAME$_PIPE_DEPTH,"$APPLICATION_NAME$_CMD_PIPE");
    CFE_SB_Subscribe($APPLICATION_NAME$_APP_CMD_MID, $APPLICATION_NAME$_CommandPipe);
    CFE_SB_Subscribe($APPLICATION_NAME$_APP_SEND_HK_MID, $APPLICATION_NAME$_CommandPipe);

    $APPLICATION_NAME$_ResetCounters();

    CFE_SB_InitMsg(&$APPLICATION_NAME$_HkTelemetryPkt,
                   $APPLICATION_NAME$_APP_HK_TLM_MID,
                   $APPLICATION_NAME$_APP_HK_TLM_LNGTH, TRUE);

    CFE_EVS_SendEvent ($APPLICATION_NAME$_STARTUP_INF_EID, CFE_EVS_INFORMATION,
               "$Application Name$ App Initialized. Version %d.%d.%d.%d",
                $APPLICATION_NAME$_APP_MAJOR_VERSION,
                $APPLICATION_NAME$_APP_MINOR_VERSION, 
                $APPLICATION_NAME$_APP_REVISION, 
                $APPLICATION_NAME$_APP_MISSION_REV);
				
} /* End of $APPLICATION_NAME$_AppInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  $APPLICATION_NAME$_ProcessCommandPacket                                        */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the $Application Name$    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void $APPLICATION_NAME$_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId($APPLICATION_NAME$_MsgPtr);

    switch (MsgId)
    {
        case $APPLICATION_NAME$_APP_CMD_MID:
            $APPLICATION_NAME$_ProcessGroundCommand();
            break;

        case $APPLICATION_NAME$_APP_SEND_HK_MID:
            $APPLICATION_NAME$_ReportHousekeeping();
            break;

        default:
            $APPLICATION_NAME$_HkTelemetryPkt.$application_name$_command_error_count++;
            CFE_EVS_SendEvent($APPLICATION_NAME$_COMMAND_ERR_EID,CFE_EVS_ERROR,
			"$Application Name$: invalid command packet,MID = 0x%x", MsgId);
            break;
    }

    return;

} /* End $APPLICATION_NAME$_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* $APPLICATION_NAME$_ProcessGroundCommand() -- $Application Name$ ground commands                    */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

void $APPLICATION_NAME$_ProcessGroundCommand(void)
{
    uint16 CommandCode;

    CommandCode = CFE_SB_GetCmdCode($APPLICATION_NAME$_MsgPtr);

    /* Process "known" $Application Name$ app ground commands */
    switch (CommandCode)
    {
        case $APPLICATION_NAME$_APP_NOOP_CC:
            $APPLICATION_NAME$_HkTelemetryPkt.$application_name$_command_count++;
            CFE_EVS_SendEvent($APPLICATION_NAME$_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
			"$Application Name$: NOOP command");
            break;

        case $APPLICATION_NAME$_APP_RESET_COUNTERS_CC:
            $APPLICATION_NAME$_ResetCounters();
            break;

        /* default case already found during FC vs length test */
        default:
            break;
    }
    return;

} /* End of $APPLICATION_NAME$_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  $APPLICATION_NAME$_ReportHousekeeping                                              */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void $APPLICATION_NAME$_ReportHousekeeping(void)
{
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &$APPLICATION_NAME$_HkTelemetryPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &$APPLICATION_NAME$_HkTelemetryPkt);
    return;

} /* End of $APPLICATION_NAME$_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  $APPLICATION_NAME$_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void $APPLICATION_NAME$_ResetCounters(void)
{
    /* Status of commands processed by the $Application Name$ App */
    $APPLICATION_NAME$_HkTelemetryPkt.$application_name$_command_count       = 0;
    $APPLICATION_NAME$_HkTelemetryPkt.$application_name$_command_error_count = 0;

    CFE_EVS_SendEvent($APPLICATION_NAME$_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		"$Application Name$: RESET command");
    return;

} /* End of $APPLICATION_NAME$_ResetCounters() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* $APPLICATION_NAME$_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
boolean $APPLICATION_NAME$_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
{     
    boolean result = TRUE;

    uint16 ActualLength = CFE_SB_GetTotalMsgLength(msg);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_SB_MsgId_t MessageID   = CFE_SB_GetMsgId(msg);
        uint16         CommandCode = CFE_SB_GetCmdCode(msg);

        CFE_EVS_SendEvent($APPLICATION_NAME$_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              MessageID, CommandCode, ActualLength, ExpectedLength);
        result = FALSE;
        $APPLICATION_NAME$_HkTelemetryPkt.$application_name$_command_error_count++;
    }

    return(result);

} /* End of $APPLICATION_NAME$_VerifyCmdLength() */

