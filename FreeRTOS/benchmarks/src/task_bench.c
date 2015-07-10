#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <util/log.h>
#include <util/clock.h>

#include <whet.h>
#include <dhry.h>

void vTaskBench(void * param) {
  TickType_t last, current;
  
  log_info("BENCH", "Nice to make your aquaintance.", NULL);

  vTaskDelay(10000);

  // Run Dhrystone test
  last = xTaskGetTickCount();

  runDhrystone(10);

  current = xTaskGetTickCount();

  printf("Ticks elapsed Dhrystone: %d\n", current - last);

  // Run Whetstone test
  last = xTaskGetTickCount();

  runWhetstone(10);

  current = xTaskGetTickCount();

  printf("Ticks elapsed Whetstone: %d\n", current - last);


  while (1) {
    vTaskDelay(1000);
  }
}
