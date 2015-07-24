#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <util/log.h>

extern xQueueHandle ping_to_pong_256_Q;
extern xQueueHandle pong_to_ping_256_Q;

/*
 * Pong recieves a message, and sends a response
 */
void vTaskPong256(void * param) {
  char datas[256] = {0};
  log_info("Pong 256 here! What up?\n", NULL);

  while(1) {
    // log_info("PONG", "I ought to be consumming.", NULL);

    xQueueReceive(ping_to_pong_256_Q, datas, 10000);
    //    log_info("PONG", "Consumed %ld", datas);

    xQueueSend(pong_to_ping_256_Q, datas, 0);
  }
}
