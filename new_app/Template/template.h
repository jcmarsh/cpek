/*******************************************************************************
** File: $application_name$_app.h
**
** Purpose:
**   This file is main hdr file for the $Application Name$ application.
**
**
*******************************************************************************/

#ifndef _$application_name$_app_h_
#define _$application_name$_app_h_

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

#define $APPLICATION_NAME$_PIPE_DEPTH                     32

/************************************************************************
** Type Definitions
*************************************************************************/

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point ($APPLICATION_NAME$_AppMain), these
**       functions are not called from any other source module.
*/
void $APPLICATION_NAME$_AppMain(void);
void $APPLICATION_NAME$_AppInit(void);
void $APPLICATION_NAME$_ProcessCommandPacket(void);
void $APPLICATION_NAME$_ProcessGroundCommand(void);
void $APPLICATION_NAME$_ReportHousekeeping(void);
void $APPLICATION_NAME$_ResetCounters(void);

boolean $APPLICATION_NAME$_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif /* _$application_name$_app_h_ */
