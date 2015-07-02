/*******************************************************************************
** File: bench_app.h
**
** Purpose:
**   This file is main hdr file for the Bench application.
**
**
*******************************************************************************/

#ifndef _bench_app_h_
#define _bench_app_h_

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>

/***********************************************************************/

#define BENCH_PIPE_DEPTH                     32

/************************************************************************
** Type Definitions
*************************************************************************/

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (BENCH_AppMain), these
**       functions are not called from any other source module.
*/
void BENCH_AppMain(void);
void BENCH_AppInit(void);
void BENCH_ProcessCommandPacket(void);
void BENCH_ProcessGroundCommand(void);
void BENCH_ReportHousekeeping(void);
void BENCH_ResetCounters(void);

boolean BENCH_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif /* _bench_app_h_ */
