#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <freertos/queue.h>

#include <util/log.h>
#include <util/clock.h>

extern xQueueHandle ping_to_pong_Q;
extern xQueueHandle pong_to_ping_Q;
extern xQueueHandle ping_to_pong_32_Q;
extern xQueueHandle pong_to_ping_32_Q;
extern xQueueHandle ping_to_pong_64_Q;
extern xQueueHandle pong_to_ping_64_Q;
extern xQueueHandle ping_to_pong_128_Q;
extern xQueueHandle pong_to_ping_128_Q;
extern xQueueHandle ping_to_pong_256_Q;
extern xQueueHandle pong_to_ping_256_Q;
extern xQueueHandle ping_to_pong_512_Q;
extern xQueueHandle pong_to_ping_512_Q;
extern xQueueHandle ping_to_pong_1024_Q;
extern xQueueHandle pong_to_ping_1024_Q;

unsigned long count = 0;
volatile int timer_running = 0;

void vTimerCallbackPing(xTimerHandle timer) {
  timer_running = 0;
}

/*
 * Ping starts timing, sends the message, and stops timing after a response.
 */
void vTaskPing(void * param) {
  unsigned long datas = 0;
  char data_32[32]     = {0};
  char data_64[64]     = {0};
  char data_128[128]   = {0};
  char data_256[256]   = {0};
  char data_512[512]   = {0};
  char data_1024[1024] = {0};
  xTimerHandle timer_id;
  int packet_mode = 0;
  int run_count = 0;

  log_info("PING", "Nice to make your aquaintance.", NULL);

  vTaskDelay(5000);

  timer_id = xTimerCreate((const signed char *) "Ping Timer", 1000,
			  pdFALSE, &timer_id, vTimerCallbackPing);
  xTimerStart(timer_id, 0);

  timer_running = 1;
  while(1) {
    if (timer_running && packet_mode < 7) {

      /*
      switch (packet_mode) {
      case (0):
      
        xQueueSend(ping_to_pong_Q, &datas, 0);
        xQueueReceive(pong_to_ping_Q, &datas, 10000);
        count++;
        break;
      case (1):
        xQueueSend(ping_to_pong_32_Q, data_32, 0);
        xQueueReceive(pong_to_ping_32_Q, data_32, 10000);
        count++;
        break;
      case (2):
        xQueueSend(ping_to_pong_64_Q, data_64, 0);
        xQueueReceive(pong_to_ping_64_Q, data_64, 10000);
        count++;
        break;
      case (3):
        xQueueSend(ping_to_pong_128_Q, data_128, 0);
        xQueueReceive(pong_to_ping_128_Q, data_128, 10000);
        count++;
        break;
      case (4):
        xQueueSend(ping_to_pong_256_Q, data_256, 0);
        xQueueReceive(pong_to_ping_256_Q, data_256, 10000);  
        count++; 
        break;
      case (5):
        xQueueSend(ping_to_pong_512_Q, data_512, 0);
        xQueueReceive(pong_to_ping_512_Q, data_512, 10000);  
        count++;
        break;
      case (6):
      */ 
        xQueueSend(ping_to_pong_1024_Q, data_1024, 0);
        xQueueReceive(pong_to_ping_1024_Q, data_1024, 10000);
        count++;
      /*
        break;
      } 
      */
    } else if (packet_mode < 7) {
      printf("PING; %ld pings mode %d sent in 1 sec.\n", count, packet_mode);
      count = 0;
      run_count++;
      if (run_count == 10) {
        packet_mode++;
        run_count = 0;
        vTaskDelay(500);
      }
      xTimerStart(timer_id, 0);
      timer_running = 1;
    } else {
      vTaskDelay(5000);
    }
  }
}
