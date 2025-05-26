idf_component_register(SRCS "main.c"
    SRCS "main.c" "led_ws2812.c"  # 确保 led_ws2812.c 被包含
                    INCLUDE_DIRS ".")