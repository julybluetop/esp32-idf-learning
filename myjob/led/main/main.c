// 包含必要的头文件
#include <stdio.h>                    // 标准输入输出库
#include "freertos/FreeRTOS.h"        // FreeRTOS核心头文件
#include "freertos/task.h"            // FreeRTOS任务管理
#include"driver/gpio.h"               // GPIO驱动
#include "esp_log.h"                  // ESP32日志系统
#include"driver/ledc.h"               // LED控制器驱动

// 定义常量
#define LED_GPIO          GPIO_NUM_27  // 定义LED连接的GPIO引脚号
#define FULL_EV_BIT       BIT0         // 定义LED满亮事件位
#define EMPTY_EV_BIT      BIT1         // 定义LED熄灭事件位
static EventGroupHandle_t ledc_event_handle;  // 声明事件组句柄

// LED控制器完成回调函数
bool IRAM_ATTR ledc_finish_cb(const ledc_cb_param_t *param,void *user_arg) //IRAM_ATTR标记使函数在内存中执行，提高效率
{   
    BaseType_t taskWoken;  // 定义任务唤醒标志
    if (param->duty)       // 如果当前占空比不为0
    {
        xEventGroupSetBitsFromISR(ledc_event_handle, FULL_EV_BIT,&taskWoken);  // 设置满亮事件位
    }
    else                   // 如果当前占空比为0
    {
        xEventGroupSetBitsFromISR(ledc_event_handle, EMPTY_EV_BIT,&taskWoken);  // 设置熄灭事件位
    }
    return taskWoken;      // 返回任务唤醒标志
}

// LED运行任务函数
void led_run_tesk(void *arg)
{
    EventBits_t ev;       // 定义事件位变量
    while (1)             // 无限循环
    {
        // 等待LED事件，超时时间5000ms
        ev = xEventGroupWaitBits(ledc_event_handle, FULL_EV_BIT | EMPTY_EV_BIT, pdTRUE, pdFALSE, pdMS_TO_TICKS(5000));
        if (ev & FULL_EV_BIT)  // 如果是满亮事件
        {
            ESP_LOGI("LED", "LED is full");  // 记录日志
            // 设置LED渐变至熄灭，时间2000ms
            ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0, 2000);
            ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
        }
        else               // 如果是熄灭事件
        {
            ESP_LOGI("LED", "LED is empty");  // 记录日志
            // 设置LED渐变至半亮(8192/2^13)，时间2000ms
            ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 8192, 2000);
            ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
        }
    }
}

// 主函数
void app_main(void)
{
    // 配置GPIO
    gpio_config_t led_cfg = {
        .pin_bit_mask = (1 << LED_GPIO),         // 设置GPIO引脚掩码
        .mode = GPIO_MODE_OUTPUT,                 // 设置为输出模式
        .pull_up_en = GPIO_PULLUP_DISABLE,       // 禁用上拉
        .pull_down_en = GPIO_PULLDOWN_DISABLE,   // 禁用下拉
    };
    gpio_config(&led_cfg);  // 应用GPIO配置

    // 配置LEDC定时器
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,       // 低速模式
        .timer_num = LEDC_TIMER_0,               // 使用定时器0
        .duty_resolution = LEDC_TIMER_13_BIT,    // 13位分辨率
        .freq_hz = 5000,                         // PWM频率5kHz
        .clk_cfg = LEDC_AUTO_CLK,               // 自动选择时钟源
    };
    ledc_timer_config(&ledc_timer);  // 应用定时器配置

    // 配置LEDC通道
    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,       // 低速模式
        .channel = LEDC_CHANNEL_0,               // 使用通道0
        .timer_sel = LEDC_TIMER_0,               // 使用定时器0
        .intr_type = LEDC_INTR_DISABLE,         // 禁用中断
        .gpio_num = LED_GPIO,                    // LED引脚
        .duty = 0,                               // 初始占空比0
        .hpoint = 0,                            // 高电平起始点
    };
    ledc_channel_config(&ledc_channel);  // 应用通道配置

    ledc_fade_func_install(0);  // 安装LEDC渐变功能

    // 设置初始渐变效果
    ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, 8192, 2000);
    // 启动渐变，不等待完成
    ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_NO_WAIT);

    ledc_event_handle = xEventGroupCreate();  // 创建事件组
    // 配置LEDC回调函数
    ledc_cbs_t ledc_cbs = {
        .fade_cb = ledc_finish_cb,
    };

    // 注册LEDC回调函数
    ledc_cb_register(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, &ledc_cbs, NULL);

    // 创建LED运行任务
    xTaskCreate(led_run_tesk, "led_run_tesk", 2048, NULL, 3, NULL);
    ESP_LOGI("LED", "LED task created");  // 记录任务创建完成日志
}
