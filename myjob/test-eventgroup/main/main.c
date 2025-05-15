#include <stdio.h>
#include"freertos/FreeRTOS.h"
#include"freertos/event_groups.h"
#include"freertos/task.h"
#include"esp_log.h"

#define NUM0_bit BIT0
#define NUM1_bit BIT1

static EventGroupHandle_t xEventGroup = NULL;

void vTask1(void *pvParameters)
{
    while (1)
    {
        //发送事件位
        xEventGroupSetBits(xEventGroup, NUM0_bit);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        xEventGroupSetBits(xEventGroup, NUM1_bit);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void vTask2(void *pvParameters)
{
    EventBits_t uxBits;
    while (1)
    {
        //接收打印事件位
        uxBits = xEventGroupWaitBits(xEventGroup, NUM0_bit | NUM1_bit, pdTRUE, pdFALSE, portMAX_DELAY);
        if ((uxBits & NUM0_bit))
        {
            ESP_LOGI("Task2", "NUM0_bit is set");
        }
        if ((uxBits & NUM1_bit))
        {
            ESP_LOGI("Task2", "NUM1_bit is set");
        }
    }
}

void app_main(void)
{
    xEventGroup = xEventGroupCreate();
    xTaskCreate(vTask1, "Task1", 2048, NULL, 5, NULL);
    xTaskCreate(vTask2, "Task2", 2048, NULL, 5, NULL);
}
