#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <util/log.h>

extern xQueueHandle ping_to_pong_32_Q;
extern xQueueHandle pong_to_ping_32_Q;

/*
 * Pong recieves a message, and sends a response
 */
void vTaskPong32(void * param) {
  char datas[32] = {0};
  log_info("Pong 32 here! What up?\n", NULL);

  while(1) {
    // log_info("PONG", "I ought to be consumming.", NULL);

    xQueueReceive(ping_to_pong_32_Q, datas, 10000);
    //    log_info("PONG", "Consumed %ld", datas);

    xQueueSend(pong_to_ping_32_Q, datas, 0);
  }
}
