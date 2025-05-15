#include <stdio.h>
#include"freertos/FreeRTOS.h"
#include"freertos/task.h"
#include"freertos/semphr.h"
#include"driver/gpio.h"
#include"esp_log.h"


SemaphoreHandle_t bin_sem;
void taskA(void* param)
{
    while (1)
    {
        xSemaphoreGive(bin_sem);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    
}
void taskB(void* param)
{
    while (1)
    {
        if(pdTRUE == xSemaphoreTake(bin_sem, portMAX_DELAY))
        {
            ESP_LOGI("bin", "taskB take binsem");
        }
    }
    
}
void app_main(void)
{
    bin_sem = xSemaphoreCreateBinary();
    xTaskCreate(taskA, "taskA", 2048, NULL, 3, NULL);
    xTaskCreate(taskB, "taskB", 2048, NULL, 3, NULL);
}