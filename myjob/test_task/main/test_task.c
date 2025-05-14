#include <stdio.h>
#include"freertos/FreeRTOS.h"
#include"freertos/task.h"
#include"esp_log.h"
void testA(void *arg)
{
    while (1)
    {
        ESP_LOGI("testA", "Hello World");
        vTaskDelay(pdMS_TO_TICKS(500)); // Delay for 1 second
    }
}
void app_main(void)
{
    xTaskCreatePinnedToCore(testA, "Hello World", 2048, NULL, 3, NULL, 0); // Create task testA on core 0
}
