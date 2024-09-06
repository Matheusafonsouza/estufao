#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "gpio_setup.h"
#include "oled.h"

#define BUTTON_PIN GPIO_NUM_0
#define LED_PIN GPIO_NUM_2

#define TAG "BUTTON"

void button_task(void *params)
{
    pin_mode(BUTTON_PIN, GPIO_INPUT_PULLUP);
    pin_mode(LED_PIN, GPIO_OUTPUT);

    while (1)
    {
        if (digital_read(BUTTON_PIN) == 0)
        {
            change_display_mode();
            digital_write(LED_PIN, 1);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        else
        {
            digital_write(LED_PIN, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
