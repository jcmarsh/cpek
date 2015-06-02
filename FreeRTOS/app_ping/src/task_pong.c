#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <util/log.h>

extern xQueueHandle ping_to_pong_Q;
extern xQueueHandle pong_to_ping_Q;

/*
 * Pong recieves a message, and sends a response
 */
void vTaskPong(void * param) {
  unsigned long datas = 0;
  log_info("Pong here! What up?", NULL);

  while(1) {
    // log_info("PONG", "I ought to be consumming.", NULL);

    xQueueReceive(ping_to_pong_Q, &datas, 10000);
    //    log_info("PONG", "Consumed %ld", datas);

    datas++;

    xQueueSend(pong_to_ping_Q, &datas, 0);
  }
}
