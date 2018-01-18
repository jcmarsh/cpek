/*
** File   :	startuptable.c
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. 
**
** Author :     Alan Cudmore	
**
** Purpose:     
**
*/
/*
** Includes
*/

#include "osapi.h"

/*
** Defines
*/
#define VALID   1
#define INVALID 0

/* 
** typedef struct
**
** {
**   uint32 valid;
**   char   filename[OS_MAX_FILE_NAME];
**   char   entry_point_name[OS_MAX_SYM_LEN];
**   uint32 entry_point_addr;
** 
** } OS_static_module_table_t;
**
*/

/*
** Extern app/lib entry points
*/
extern void CFS_LibInit(void); 
extern void PING_AppMain(void); 
extern void PONG_AppMain(void); 

OS_static_module_table_t OS_static_module_table[OS_MAX_MODULES] = 
{
   { VALID, "/static/cfs_lib.elf",   "CFS_LibInit",     (uint32) &(CFS_LibInit) },
   { VALID, "/static/ping.elf",        "PING_AppMain",      (uint32) &(PING_AppMain) },
   { VALID, "/static/pong.elf",       "PONG_AppMain",     (uint32) &(PONG_AppMain) },
   { INVALID,"none",              "none",           0                          }
};

