#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <freertos/queue.h>

#include <util/log.h>
#include <util/clock.h>

#include <rw_lib.h>

int timer_running = 0;
void vTimerCallbackPing(xTimerHandle timer) {
  timer_running = 0;
}

/*
 * Ping starts timing, sends the message, and stops timing after a response.
 */
void vTaskRWPG(void * param) {
  unsigned long counter = 0;
  xTimerHandle timer_id;
  int retval;
  uint8_t param_type = 1;
  float param_value;
  
  log_info("RWPG", "Nice to make your aquaintance.", NULL);

  vTaskDelay(10000);

  retval = rwlib_turnon(0x34);
  log_info("RWPG", "I2C send recieved: %d", retval);

  vTaskDelay(10000);

  // Ping doesn't work
  retval = rwlib_ping(0x34);
  log_info("RW", "I2C ping happend (maybe): %d", retval);

  vTaskDelay(10000);

  timer_id = xTimerCreate((const signed char *) "RWPG Timer", 1000,
			  pdFALSE, &timer_id, vTimerCallbackPing);
  xTimerStart(timer_id, 0);

  timer_running = 1;
  while(1) {
    if (timer_running) {
      // log_info("RWPG", "Param req. for the reaction wheel.", NULL);

      // RW wheel code
      retval = rwlib_receiveParamTelem(0x34, &param_type, &param_value);
      counter++;
      //log_info("RWPG", "Ret: %d\tWheel param num: %d, value: %f",
      //       retval, param_type, param_value);


      //vTaskDelay(200); // remove this!
    } else {
      log_info("RWPG", "%ld pings sent in 1 sec.", counter);
      counter = 0;
      xTimerStart(timer_id, 0);
      timer_running = 1;
    }
  }
}
