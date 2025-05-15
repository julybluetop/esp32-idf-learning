#include <stdio.h>
#include"freertos/FreeRTOS.h"
#include"freertos/task.h"
#include"freertos/semphr.h"
#include"driver/gpio.h"
#include"dht11.h"
#include"esp_log.h"


SemaphoreHandle_t dht11_mutex;

void taskA(void* param)
{
    int temp, humidity;
    while (1)
    {
        xSemaphoreTake(dht11_mutex, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(500));
        if(DHT11_StartGet(&temp,&humidity))
        {
            ESP_LOGE("DHT11","taskA:temp:%d,humidity:%d",temp/10,humidity);
        }
        xSemaphoreGive(dht11_mutex);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void taskB(void* param)
{
    int temp, humidity;
    while (1)
    {
        xSemaphoreTake(dht11_mutex, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(500));
        if(DHT11_StartGet(&temp,&humidity))
        {
            ESP_LOGE("DHT11","taskB:temp:%d,humidity:%d",temp/10,humidity);
        }
        xSemaphoreGive(dht11_mutex);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
void app_main(void)
{
    dht11_mutex = xSemaphoreCreateMutex();
    DHT11_Init(GPIO_NUM_25);
    xTaskCreate(taskA, "taskA", 2048, NULL, 3, NULL);
    xTaskCreate(taskB, "taskB", 2048, NULL, 3, NULL);
}
