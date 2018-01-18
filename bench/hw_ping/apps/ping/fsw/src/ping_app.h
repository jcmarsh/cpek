/*******************************************************************************
** File: ping_app.h
**
** Purpose:
**   This file is main hdr file for the Ping application.
**
**
*******************************************************************************/

#ifndef _ping_app_h_
#define _ping_app_h_

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

#define PING_PIPE_DEPTH                     32

/************************************************************************
** Type Definitions
*************************************************************************/

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (PING_AppMain), these
**       functions are not called from any other source module.
*/
void PING_AppMain(void);
void PING_AppInit(void);
void PING_ProcessCommandPacket(void);
void PING_ProcessGroundCommand(void);
void PING_ReportHousekeeping(void);
void PING_ResetCounters(void);

boolean PING_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif /* _ping_app_h_ */
