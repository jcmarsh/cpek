/*******************************************************************************
** File: pong_app.c
**
** Purpose:
**   This file contains the source code for the Pong App.
**
*******************************************************************************/

/*
**   Include Files:
*/

#include "pong_app.h"
#include "pong_app_perfids.h"
#include "pong_app_msgids.h"
#include "pong_app_msg.h"
#include "pong_app_events.h"
#include "pong_app_version.h"
#include "ping_app_msgids.h"
#include "ping_app_msg.h"

/*
** global data
*/

pong_hk_tlm_t    PONG_HkTelemetryPkt;
PING_NoArgsCmd_t PING_cmd;
CFE_SB_PipeId_t    PONG_CommandPipe;
CFE_SB_MsgPtr_t    PONG_MsgPtr;

static CFE_EVS_BinFilter_t  PONG_EventFilters[] =
       {  /* Event ID    mask */
          {PONG_STARTUP_INF_EID,       0x0000},
          {PONG_COMMAND_ERR_EID,       0x0000},
          {PONG_COMMANDNOP_INF_EID,    0x0000},
          {PONG_COMMANDRST_INF_EID,    0x0000},
       };

void PONG_AppMain( void ) {
  int32  status;
  uint32 RunStatus = CFE_ES_APP_RUN;

  // CFE_ES_PerfLogEntry(PONG_APP_PERF_ID);

  PONG_AppInit();

  CFE_SB_InitMsg(&PING_cmd, PONG_TO_PING_MID, sizeof(PING_NoArgsCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &PING_cmd, PING_PING_CC);

  /*
  ** PONG Runloop
  */
  while (CFE_ES_RunLoop(&RunStatus) == TRUE) {
    // CFE_ES_PerfLogExit(PONG_APP_PERF_ID);

    /* Pend on receipt of command packet -- timeout set to 500 millisecs */
    status = CFE_SB_RcvMsg(&PONG_MsgPtr, PONG_CommandPipe, 500);
    
    // Send Ping
        

    // CFE_ES_PerfLogEntry(PONG_APP_PERF_ID);

    if (status == CFE_SUCCESS) {
      PONG_ProcessCommandPacket();
    }

  }

  CFE_ES_ExitApp(RunStatus);
} /* End of PONG_AppMain() */

void PONG_AppInit(void) {
  /*
  ** Register the app with Executive services
  */
  CFE_ES_RegisterApp() ;

  /*
  ** Register the events
  */ 
  CFE_EVS_Register(PONG_EventFilters,
		   sizeof(PONG_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
		   CFE_EVS_BINARY_FILTER);

  /*
  ** Create the Software Bus command pipe and subscribe to housekeeping
  **  messages
  */
  CFE_SB_CreatePipe(&PONG_CommandPipe, PONG_PIPE_DEPTH,"PONG_CMD_PIPE");
  CFE_SB_Subscribe(PONG_APP_CMD_MID, PONG_CommandPipe);
  CFE_SB_Subscribe(PONG_APP_SEND_HK_MID, PONG_CommandPipe);
  CFE_SB_Subscribe(PING_TO_PONG_MID, PONG_CommandPipe);
  
  PONG_ResetCounters();

  CFE_SB_InitMsg(&PONG_HkTelemetryPkt,
		 PONG_APP_HK_TLM_MID,
		 PONG_APP_HK_TLM_LNGTH, TRUE);

  CFE_EVS_SendEvent (PONG_STARTUP_INF_EID, CFE_EVS_INFORMATION,
		     "Pong App Initialized. Version %d.%d.%d.%d",
		     PONG_APP_MAJOR_VERSION,
		     PONG_APP_MINOR_VERSION, 
		     PONG_APP_REVISION, 
		     PONG_APP_MISSION_REV);
				
} /* End of PONG_AppInit() */

void PONG_ProcessCommandPacket(void) {
  CFE_SB_MsgId_t  MsgId;

  MsgId = CFE_SB_GetMsgId(PONG_MsgPtr);

  switch (MsgId) {
  case PONG_APP_CMD_MID:
    PONG_ProcessGroundCommand();
    break;
  case PONG_APP_SEND_HK_MID:
    PONG_ReportHousekeeping();
    break;
  case PING_TO_PONG_MID:
    // Send a cmd to pong
    CFE_SB_SendMsg((CFE_SB_MsgPtr_t) ((int)&PING_cmd));
    break;
  default:
    PONG_HkTelemetryPkt.pong_command_error_count++;
    CFE_EVS_SendEvent(PONG_COMMAND_ERR_EID,CFE_EVS_ERROR,
		      "Pong: invalid command packet,MID = 0x%x", MsgId);
    break;
  }

  return;
} /* End PONG_ProcessCommandPacket */

void PONG_ProcessGroundCommand(void) {
  uint16 CommandCode;

  CommandCode = CFE_SB_GetCmdCode(PONG_MsgPtr);

  /* Process "known" Pong app ground commands */
  switch (CommandCode) {
  case PONG_APP_NOOP_CC:
    PONG_HkTelemetryPkt.pong_command_count++;
    CFE_EVS_SendEvent(PONG_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
		      "Pong: NOOP command");
    break;
  case PONG_APP_RESET_COUNTERS_CC:
    PONG_ResetCounters();
    break;
    /* default case already found during FC vs length test */
  default:
    break;
  }
  return;
} /* End of PONG_ProcessGroundCommand() */

void PONG_ReportHousekeeping(void) {
  CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &PONG_HkTelemetryPkt);
  CFE_SB_SendMsg((CFE_SB_Msg_t *) &PONG_HkTelemetryPkt);
  return;
} /* End of PONG_ReportHousekeeping() */

void PONG_ResetCounters(void) {
  /* Status of commands processed by the Pong App */
  PONG_HkTelemetryPkt.pong_command_count       = 0;
  PONG_HkTelemetryPkt.pong_command_error_count = 0;

  CFE_EVS_SendEvent(PONG_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		    "Pong: RESET command");
  return;
} /* End of PONG_ResetCounters() */

boolean PONG_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength) {     
  boolean result = TRUE;

  uint16 ActualLength = CFE_SB_GetTotalMsgLength(msg);

  /*
  ** Verify the command packet length.
  */
  if (ExpectedLength != ActualLength) {
    CFE_SB_MsgId_t MessageID   = CFE_SB_GetMsgId(msg);
    uint16         CommandCode = CFE_SB_GetCmdCode(msg);

    CFE_EVS_SendEvent(PONG_LEN_ERR_EID, CFE_EVS_ERROR,
		      "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
		      MessageID, CommandCode, ActualLength, ExpectedLength);
    result = FALSE;
    PONG_HkTelemetryPkt.pong_command_error_count++;
  }
  return(result);
} /* End of PONG_VerifyCmdLength() */
