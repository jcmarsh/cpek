/*******************************************************************************
** File: ping_app.c
**
** Purpose:
**   This file contains the source code for the Ping App.
**
*******************************************************************************/

/*
**   Include Files:
*/

#include "ping_app.h"
#include "ping_app_perfids.h"
#include "ping_app_msgids.h"
#include "ping_app_msg.h"
#include "ping_app_events.h"
#include "ping_app_version.h"
#include "pong_app_msgids.h"
#include "pong_app_msg.h"

#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <freertos/queue.h>

/*
** global data
*/

ping_hk_tlm_t    PING_HkTelemetryPkt;
PONG_NoArgsCmd_t PONG_cmd;
CFE_SB_PipeId_t    PING_CommandPipe;
CFE_SB_MsgPtr_t    PING_MsgPtr;

static CFE_EVS_BinFilter_t  PING_EventFilters[] = {  /* Event ID    mask */
  {PING_STARTUP_INF_EID,       0x0000},
  {PING_COMMAND_ERR_EID,       0x0000},
  {PING_COMMANDNOP_INF_EID,    0x0000},
  {PING_COMMANDRST_INF_EID,    0x0000},
};

unsigned long count = 0;
int timer_running = 0;

void vTimerCallbackPing(xTimerHandle time) {
  timer_running = 0;
}

void PING_AppMain( void ) {
  int32  status;
  uint32 RunStatus = CFE_ES_APP_RUN;
  xTimerHandle timer_id;

  //CFE_ES_PerfLogEntry(PING_APP_PERF_ID);

  PING_AppInit();
  CFE_SB_InitMsg(&PONG_cmd, PING_TO_PONG_MID, sizeof(PONG_NoArgsCmd_t), TRUE);
  CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)  &PONG_cmd, PONG_PING_CC);

  timer_id = xTimerCreate((const signed char *) "Ping Timer", 1000,
			  pdFALSE, &timer_id, vTimerCallbackPing);
  xTimerStart(timer_id, 0);
  timer_running = 1;

  printf("PING set timer, ready to rock.\n");
  /*
  ** PING Runloop
  */
  while (CFE_ES_RunLoop(&RunStatus) == TRUE) {
    //CFE_ES_PerfLogExit(PING_APP_PERF_ID);
    if (timer_running) {
      // Send a cmd to pong
      CFE_SB_SendMsg((CFE_SB_MsgPtr_t) ((int)&PONG_cmd));

      /* Pend on receipt of command packet -- timeout set to 500 millisecs */
      status = CFE_SB_RcvMsg(&PING_MsgPtr, PING_CommandPipe, 500);
      if (status == CFE_SUCCESS) {
	PING_ProcessCommandPacket();
      }
    } else {
      printf("PING; %ld pings sent in 1 sec.\n", count);
      count = 0;
      xTimerStart(timer_id, 0);
      timer_running = 1;
    }
  }

  CFE_ES_ExitApp(RunStatus);
} /* End of PING_AppMain() */

void PING_AppInit(void) {
  /*
  ** Register the app with Executive services
  */
  CFE_ES_RegisterApp() ;

  /*
  ** Register the events
  */ 
  CFE_EVS_Register(PING_EventFilters,
		   sizeof(PING_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
		   CFE_EVS_BINARY_FILTER);

  /*
  ** Create the Software Bus command pipe and subscribe to housekeeping
  **  messages
  */
  CFE_SB_CreatePipe(&PING_CommandPipe, PING_PIPE_DEPTH,"PING_CMD_PIPE");
  CFE_SB_Subscribe(PING_APP_CMD_MID, PING_CommandPipe);
  CFE_SB_Subscribe(PING_APP_SEND_HK_MID, PING_CommandPipe);
  CFE_SB_Subscribe(PONG_TO_PING_MID, PING_CommandPipe);

  PING_ResetCounters();

  CFE_SB_InitMsg(&PING_HkTelemetryPkt,
		 PING_APP_HK_TLM_MID,
		 PING_APP_HK_TLM_LNGTH, TRUE);

  CFE_EVS_SendEvent (PING_STARTUP_INF_EID, CFE_EVS_INFORMATION,
		     "Ping App Initialized. Version %d.%d.%d.%d",
		     PING_APP_MAJOR_VERSION,
		     PING_APP_MINOR_VERSION, 
		     PING_APP_REVISION, 
		     PING_APP_MISSION_REV);
} /* End of PING_AppInit() */

void PING_ProcessCommandPacket(void) {
  CFE_SB_MsgId_t  MsgId;

  MsgId = CFE_SB_GetMsgId(PING_MsgPtr);

  switch (MsgId) {
  case PING_APP_CMD_MID:
    PING_ProcessGroundCommand();
    break;
  case PING_APP_SEND_HK_MID:
    PING_ReportHousekeeping();
    break;
  case PONG_TO_PING_MID:
    //printf("Circle complete.\n");
    count++;
    break;
  default:
    PING_HkTelemetryPkt.ping_command_error_count++;
    CFE_EVS_SendEvent(PING_COMMAND_ERR_EID,CFE_EVS_ERROR,
		      "Ping: invalid command packet,MID = 0x%x", MsgId);
    break;
  }

  return;
} /* End PING_ProcessCommandPacket */

void PING_ProcessGroundCommand(void) {
  uint16 CommandCode;

  CommandCode = CFE_SB_GetCmdCode(PING_MsgPtr);

  /* Process "known" Ping app ground commands */
  switch (CommandCode) {
  case PING_APP_NOOP_CC:
    PING_HkTelemetryPkt.ping_command_count++;
    CFE_EVS_SendEvent(PING_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
		      "Ping: NOOP command");
    break;
  case PING_APP_RESET_COUNTERS_CC:
    PING_ResetCounters();
    break;
    /* default case already found during FC vs length test */
  default:
    break;
  }
  return;
} /* End of PING_ProcessGroundCommand() */

void PING_ReportHousekeeping(void) {
  CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &PING_HkTelemetryPkt);
  CFE_SB_SendMsg((CFE_SB_Msg_t *) &PING_HkTelemetryPkt);
  return;
} /* End of PING_ReportHousekeeping() */

void PING_ResetCounters(void) {
  /* Status of commands processed by the Ping App */
  PING_HkTelemetryPkt.ping_command_count       = 0;
  PING_HkTelemetryPkt.ping_command_error_count = 0;
  
  CFE_EVS_SendEvent(PING_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		    "Ping: RESET command");
  return;
} /* End of PING_ResetCounters() */

boolean PING_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength) {     
  boolean result = TRUE;
  
  uint16 ActualLength = CFE_SB_GetTotalMsgLength(msg);

  /*
  ** Verify the command packet length.
  */
  if (ExpectedLength != ActualLength) {
    CFE_SB_MsgId_t MessageID   = CFE_SB_GetMsgId(msg);
    uint16         CommandCode = CFE_SB_GetCmdCode(msg);
    
    CFE_EVS_SendEvent(PING_LEN_ERR_EID, CFE_EVS_ERROR,
		      "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
		      MessageID, CommandCode, ActualLength, ExpectedLength);
    result = FALSE;
    PING_HkTelemetryPkt.ping_command_error_count++;
  }

  return(result);
} /* End of PING_VerifyCmdLength() */
