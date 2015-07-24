#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <util/log.h>

extern xQueueHandle ping_to_pong_1024_Q;
extern xQueueHandle pong_to_ping_1024_Q;

/*
 * Pong recieves a message, and sends a response
 */
void vTaskPong1024(void * param) {
  char datas[1024] = {0};
  log_info("Pong 1024 here! What up?\n", NULL);

  while(1) {
    // log_info("PONG", "I ought to be consumming.", NULL);

    xQueueReceive(ping_to_pong_1024_Q, datas, 10000);
    //    log_info("PONG", "Consumed %ld", datas);

    xQueueSend(pong_to_ping_1024_Q, datas, 0);
  }
}
