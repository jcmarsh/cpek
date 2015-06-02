/*******************************************************************************
** File: pong_app.h
**
** Purpose:
**   This file is main hdr file for the Pong application.
**
**
*******************************************************************************/

#ifndef _pong_app_h_
#define _pong_app_h_

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

#define PONG_PIPE_DEPTH                     32

/************************************************************************
** Type Definitions
*************************************************************************/

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (PONG_AppMain), these
**       functions are not called from any other source module.
*/
void PONG_AppMain(void);
void PONG_AppInit(void);
void PONG_ProcessCommandPacket(void);
void PONG_ProcessGroundCommand(void);
void PONG_ReportHousekeeping(void);
void PONG_ResetCounters(void);

boolean PONG_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif /* _pong_app_h_ */
