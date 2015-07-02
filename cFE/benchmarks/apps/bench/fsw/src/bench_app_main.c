/*******************************************************************************
** File: bench_app.c
**
** Purpose:
**   This file contains the source code for the Bench App.
**
*******************************************************************************/

/*
**   Include Files:
*/

#include "dhry.h"
#include "whet.h"
#include "bench_app.h"
#include "bench_app_perfids.h"
#include "bench_app_msgids.h"
#include "bench_app_msg.h"
#include "bench_app_events.h"
#include "bench_app_version.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
//#include <freertos/timers.h>
//#include <freertos/queue.h>

/*
** global data
*/

bench_hk_tlm_t    BENCH_HkTelemetryPkt;
CFE_SB_PipeId_t    BENCH_CommandPipe;
CFE_SB_MsgPtr_t    BENCH_MsgPtr;

static CFE_EVS_BinFilter_t  BENCH_EventFilters[] = {  /* Event ID    mask */
  {BENCH_STARTUP_INF_EID,       0x0000},
  {BENCH_COMMAND_ERR_EID,       0x0000},
  {BENCH_COMMANDNOP_INF_EID,    0x0000},
  {BENCH_COMMANDRST_INF_EID,    0x0000},
};

// xTaskGetTickCount
void BENCH_AppMain(void) {
  int retval;
  TickType_t last;
  TickType_t current;
  int32  status;
  uint32 RunStatus = CFE_ES_APP_RUN;

  CFE_ES_PerfLogEntry(BENCH_APP_PERF_ID);

  BENCH_AppInit();

  // Run Dhrystone test
  last = xTaskGetTickCount();

  runDhrystone(10);

  current = xTaskGetTickCount();

  printf("Ticks elapsed Dhrystone: %d\n", current - last);

  // Run Whetstone test
  last = xTaskGetTickCount();

  runWhetstone(10);

  current = xTaskGetTickCount();

  printf("Ticks elapsed Whetstone: %d\n", current - last);



  /*
  ** BENCH Runloop
  */
  while (CFE_ES_RunLoop(&RunStatus) == TRUE) {

  }
  CFE_ES_ExitApp(RunStatus);

} /* End of BENCH_AppMain() */

void BENCH_AppInit(void) {
  /*
  ** Register the app with Executive services
  */
  CFE_ES_RegisterApp() ;

  /*
  ** Register the events
  */ 
  CFE_EVS_Register(BENCH_EventFilters,
		   sizeof(BENCH_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
		   CFE_EVS_BINARY_FILTER);

  /*
  ** Create the Software Bus command pipe and subscribe to housekeeping
  **  messages
  */
  CFE_SB_CreatePipe(&BENCH_CommandPipe, BENCH_PIPE_DEPTH,"BENCH_CMD_PIPE");
  CFE_SB_Subscribe(BENCH_APP_CMD_MID, BENCH_CommandPipe);
  CFE_SB_Subscribe(BENCH_APP_SEND_HK_MID, BENCH_CommandPipe);
  
  BENCH_ResetCounters();
  
  CFE_SB_InitMsg(&BENCH_HkTelemetryPkt,
		 BENCH_APP_HK_TLM_MID,
		 BENCH_APP_HK_TLM_LNGTH, TRUE);
  
  CFE_EVS_SendEvent (BENCH_STARTUP_INF_EID, CFE_EVS_INFORMATION,
		     "Bench App Initialized. Version %d.%d.%d.%d",
		     BENCH_APP_MAJOR_VERSION,
		     BENCH_APP_MINOR_VERSION, 
		     BENCH_APP_REVISION, 
		     BENCH_APP_MISSION_REV);
} /* End of BENCH_AppInit() */

void BENCH_ProcessCommandPacket(void) {
  CFE_SB_MsgId_t  MsgId;

  MsgId = CFE_SB_GetMsgId(BENCH_MsgPtr);

  switch (MsgId) {
  case BENCH_APP_CMD_MID:
    BENCH_ProcessGroundCommand();
    break;
  case BENCH_APP_SEND_HK_MID:
    BENCH_ReportHousekeeping();
    break;
  default:
    BENCH_HkTelemetryPkt.bench_command_error_count++;
    CFE_EVS_SendEvent(BENCH_COMMAND_ERR_EID,CFE_EVS_ERROR,
		      "Bench: invalid command packet,MID = 0x%x", MsgId);
    break;
  }

  return;
} /* End BENCH_ProcessCommandPacket */

void BENCH_ProcessGroundCommand(void) {
  uint16 CommandCode;

  CommandCode = CFE_SB_GetCmdCode(BENCH_MsgPtr);

  /* Process "known" Bench app ground commands */
  switch (CommandCode) {
  case BENCH_APP_NOOP_CC:
    BENCH_HkTelemetryPkt.bench_command_count++;
    CFE_EVS_SendEvent(BENCH_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
		      "Bench: NOOP command");
    break;
  case BENCH_APP_RESET_COUNTERS_CC:
    BENCH_ResetCounters();
    break;  
    /* default case already found during FC vs length test */
  default:
    break;
  }
  return;
} /* End of BENCH_ProcessGroundCommand() */

void BENCH_ReportHousekeeping(void) {
  CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &BENCH_HkTelemetryPkt);
  CFE_SB_SendMsg((CFE_SB_Msg_t *) &BENCH_HkTelemetryPkt);
  return;
} /* End of BENCH_ReportHousekeeping() */

void BENCH_ResetCounters(void) {
  /* Status of commands processed by the Bench App */
  BENCH_HkTelemetryPkt.bench_command_count       = 0;
  BENCH_HkTelemetryPkt.bench_command_error_count = 0;

  CFE_EVS_SendEvent(BENCH_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		    "Bench: RESET command");
  return;
} /* End of BENCH_ResetCounters() */

boolean BENCH_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength) {     
  boolean result = TRUE;

  uint16 ActualLength = CFE_SB_GetTotalMsgLength(msg);

  /*
  ** Verify the command packet length.
  */
  if (ExpectedLength != ActualLength) {
    CFE_SB_MsgId_t MessageID   = CFE_SB_GetMsgId(msg);
    uint16         CommandCode = CFE_SB_GetCmdCode(msg);

    CFE_EVS_SendEvent(BENCH_LEN_ERR_EID, CFE_EVS_ERROR,
		      "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
		      MessageID, CommandCode, ActualLength, ExpectedLength);
    result = FALSE;
    BENCH_HkTelemetryPkt.bench_command_error_count++;
  }

  return(result);
} /* End of BENCH_VerifyCmdLength() */

