/*
 * gomspace_config_arm.h
 *
 *  Created on: 03/09/2010
 *      Author: oem
 */

#ifndef GOMSPACE_CONFIG_ARM_H_
#define GOMSPACE_CONFIG_ARM_H_

#include <stdio.h>
#include <assert.h>

#define LITTLE_ENDIAN

#define configASSERT			        assert
#define configUSE_PREEMPTION		        1
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configTICK_RATE_HZ                      ( ( portTickType ) 1000 )
#define configMAX_PRIORITIES		        ( ( unsigned portBASE_TYPE ) 256 )
#define configMINIMAL_STACK_SIZE	        ( ( unsigned portSHORT ) 1024 )
#define configMAX_TASK_NAME_LEN		        ( 20 )
#define configUSE_16_BIT_TICKS		        0
#define configIDLE_SHOULD_YIELD		        1
#define configUSE_MUTEXES		        1
#define configQUEUE_REGISTRY_SIZE	        0
#define configUSE_APPLICATION_TASK_TAG          1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES           1

#define configCHECK_FOR_STACK_OVERFLOW		2

#define configUSE_TRACE_FACILITY	        1
#define configGENERATE_RUN_TIME_STATS 		1
#define configUSE_STATS_FORMATTING_FUNCTIONS    1 
extern int vGetRuntimeCounterValue(void);
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#define portGET_RUN_TIME_COUNTER_VALUE() vGetRuntimeCounterValue()

/* Timer configuration */
#define configUSE_TIMERS                  1
#define configTIMER_TASK_PRIORITY         (configMAX_PRIORITIES / 2)         
#define configTIMER_QUEUE_LENGTH          20 
#define configTIMER_TASK_STACK_DEPTH      1024 

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet            1
#define INCLUDE_uxTaskPriorityGet           1
#define INCLUDE_vTaskDelete                 1
#define INCLUDE_vTaskCleanUpResources       0
#define INCLUDE_vTaskSuspend                1
#define INCLUDE_vTaskDelayUntil             1
#define INCLUDE_vTaskDelay                  1
#define INCLUDE_xTaskGetCurrentTaskHandle   1

/* Low frequency clock */
#define configUSE_LF_CLOCK	       	    1

#endif /* GOMSPACE_CONFIG_ARM_H_ */
