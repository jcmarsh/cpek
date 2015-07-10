#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <util/log.h>
#include <util/clock.h>

#include <whet.h>
#include <dhry.h>

void vTaskBench(void * param) {
  portTickType last, current, total_ticks;
  int number_of_runs_dhry = 100000;
  int number_of_runs_whet = 250;
  float Microseconds, Dhrystones_Per_Second;
  float whet_seconds, whet_KIPs;

  log_info("BENCH", "Tick rate MS: %d\t Config tick rate HZ: %d", portTICK_RATE_MS, configTICK_RATE_HZ);

  vTaskDelay(10000);

  // Run Dhrystone test
  last = xTaskGetTickCount();

  runDhrystone(number_of_runs_dhry);

  current = xTaskGetTickCount();

  total_ticks = current - last;
  printf("Ticks elapsed Dhrystone: %d\n", total_ticks);

  Microseconds = (total_ticks * portTICK_RATE_MS * 1000) / (float) number_of_runs_dhry;
  Dhrystones_Per_Second = number_of_runs_dhry / ((total_ticks * portTICK_RATE_MS) / 1000.0);
  printf("Microseconds per Dhrystone: %6.1f \n", Microseconds);
  printf("Dhrystones per second: %6.1f \n", Dhrystones_Per_Second);

  // Run Whetstone test
  last = xTaskGetTickCount();

  runWhetstone(number_of_runs_whet);

  current = xTaskGetTickCount();

  total_ticks = current - last;
  printf("Ticks elapsed Whetstone: %d\n", total_ticks);

  /*
C----------------------------------------------------------------
C      Performance in Whetstone KIP's per second is given by
C
C(100*LOOP*II)/TIME
C
C      where TIME is in seconds.
C--------------------------------------------------------------------
  */
  whet_seconds = (total_ticks * portTICK_RATE_MS) / 1000.0;
  whet_KIPs = (100 * number_of_runs_whet * 1) / whet_seconds;
  // printf("C Converted Double Precision Whetstones: %.1f KIPS\n", KIPS);
  printf("Whetstones: %.1f KIPS\n", whet_KIPs);

  while (1) {
    vTaskDelay(1000);
  }
}
