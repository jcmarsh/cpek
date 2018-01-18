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
extern void BENCH_AppMain(void); 

OS_static_module_table_t OS_static_module_table[OS_MAX_MODULES] = 
{
   { VALID, "/static/bench.elf",        "BENCH_AppMain",      (uint32) &(BENCH_AppMain) },
   { INVALID,"none",              "none",           0                          }
};

