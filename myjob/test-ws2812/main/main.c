/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"
#include "led_ws2812.h"

#define WS2812_GPIO_NUM     GPIO_NUM_26
#define WS2812_LED_NUM      12


void app_main(void)
{
    ws2812_strip_handle_t ws2812_handle = NULL;
    int index = 0;
    ws2812_init(WS2812_GPIO_NUM,WS2812_LED_NUM,&ws2812_handle);

    while(1)
    {
        //红色跑马灯
        for(index = 0;index < WS2812_LED_NUM;index++)
        {
            uint32_t r = 1,g = 0,b = 0;
            ws2812_write(ws2812_handle,index,r,g,b);
            vTaskDelay(pdMS_TO_TICKS(80));
        }
        //绿色跑马灯
        for(index = 0;index < WS2812_LED_NUM;index++)
        {
            uint32_t r = 0,g = 1,b = 0;
            ws2812_write(ws2812_handle,index,r,g,b);
            vTaskDelay(pdMS_TO_TICKS(80));
        }
        //蓝色跑马灯
        for(index = 0;index < WS2812_LED_NUM;index++)
        {
            uint32_t r = 0,g = 0,b = 1;
            ws2812_write(ws2812_handle,index,r,g,b);
            vTaskDelay(pdMS_TO_TICKS(80));
        }
    }

}
