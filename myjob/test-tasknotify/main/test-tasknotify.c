#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

static TaskHandle_t task1_handle;
static TaskHandle_t task2_handle;

void task1(void *param)
{
    uint32_t value = 0;
    while (1)
    {
        xTaskNotify(task2_handle, value, eSetValueWithOverwrite);
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
        value++;
    }
}

void task2(void *param)
{
    uint32_t value;
    while (1)
    {
        xTaskNotifyWait(0, ULONG_MAX, &value, portMAX_DELAY);
        ESP_LOGI("task2", "Received value: %d", value);
    }
}
void app_main(void)
{
    xTaskCreate(task1, "task1", 2048, NULL, 5, &task1_handle);
    xTaskCreate(task2, "task2", 2048, NULL, 5, &task2_handle);
}


