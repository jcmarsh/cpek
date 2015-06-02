#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <freertos/queue.h>

#include <util/log.h>
#include <util/clock.h>

extern xQueueHandle ping_to_pong_Q;
extern xQueueHandle pong_to_ping_Q;

int timer_running = 0;
void vTimerCallbackPing(xTimerHandle timer) {
  timer_running = 0;
}

/*
 * Ping starts timing, sends the message, and stops timing after a response.
 */
void vTaskPing(void * param) {
  unsigned long datas = 0;
  xTimerHandle timer_id;

  log_info("PING", "Nice to make your aquaintance.", NULL);

  timer_id = xTimerCreate((const signed char *) "Ping Timer", 1000,
			  pdFALSE, &timer_id, vTimerCallbackPing);
  xTimerStart(timer_id, 0);

  timer_running = 1;
  while(1) {
    if (timer_running) {
      //      log_info("PING", "Perhaps now would be a good time to produce.", NULL);

      xQueueSend(ping_to_pong_Q, &datas, 0);

      xQueueReceive(pong_to_ping_Q, &datas, 10000);
      //vTaskDelay(200);
    } else {
      log_info("PING", "%ld pings sent in 1 sec.", datas);
      datas = 0;
      xTimerStart(timer_id, 0);
      timer_running = 1;
    }
  }
}
