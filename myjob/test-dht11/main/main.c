#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>
#include <driver/rmt_rx.h>
#include <driver/rmt_tx.h>
#include <soc/rmt_reg.h>
#include "driver/gpio.h" 
#include <esp_log.h>
#include <freertos/queue.h>
#include "esp32/rom/ets_sys.h"
#include "dht11.h"
#include"led_ws2812.h"

#define WS2812_GPIO_NUM     GPIO_NUM_26
#define WS2812_LED_NUM      12

#define DHT11_GPIO	25		// DHT11引脚定义
const static char *TAG = "DHT11_Demo";
uint32_t r,g,b;

// 温度 湿度变量
int temp = 0,hum = 0;

// 主函数
void app_main(void)
{
	ws2812_strip_handle_t ws2812_handle = NULL;
    int index = 0;
    ws2812_init(WS2812_GPIO_NUM,WS2812_LED_NUM,&ws2812_handle);

	ESP_ERROR_CHECK(nvs_flash_init());
	vTaskDelay(100 / portTICK_PERIOD_MS);

	ESP_LOGI(TAG, "[APP] APP Is Start!~\r\n");
	ESP_LOGI(TAG, "[APP] IDF Version is %d.%d.%d",ESP_IDF_VERSION_MAJOR,ESP_IDF_VERSION_MINOR,ESP_IDF_VERSION_PATCH);
	ESP_LOGI(TAG, "[APP] Free memory: %lu bytes", esp_get_free_heap_size());
	ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());
	
	DHT11_Init(DHT11_GPIO);
	while (1){
		if (DHT11_StartGet(&temp, &hum)){
			ESP_LOGI(TAG, "temp->%i.%i C     hum->%i%%", temp / 10, temp % 10, hum);
		}
		if (temp<21 || temp>25)
		{
			r = 5;
			g = 1;
			b = 1;
		}
		else
		{
			r = 1;
			g = 5;
			b = 1;
		}
		for(index = 0;index < WS2812_LED_NUM;index++)
        {
            ws2812_write(ws2812_handle,index,r,g,b);
        }
		vTaskDelay(200 / portTICK_PERIOD_MS);
	}
}
