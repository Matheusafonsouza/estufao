#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "ssd1306.h"
#include "font8x8_basic.h"

#define TAG "SSD1306"
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

SSD1306_t dev;
extern float TEMPERATURE;
extern float HUMIDITY;
extern float MOISTURE_VALUE;

void oled_clear()
{
    ssd1306_clear_screen(&dev, false);
    ssd1306_display_text(&dev, 0, "--- estufao ---", 16, false);
}

void oled_display_info_task(void)
{
    while (true)
    {
        srand(time(NULL));

        ssd1306_display_text(&dev, 0, "--- estufao ---", 16, false);

        char lineChar[20];

        sprintf(&lineChar[0], " Temp. Ar: %.1fC", TEMPERATURE);
        ssd1306_display_text(&dev, 2, lineChar, strlen(lineChar), false);

        sprintf(&lineChar[0], " Umid. Ar: %.1f%%", HUMIDITY);
        ssd1306_display_text(&dev, 4, lineChar, strlen(lineChar), false);

        sprintf(&lineChar[0], " Um. Solo: %.1f%%", MOISTURE_VALUE);
        ssd1306_display_text(&dev, 6, lineChar, strlen(lineChar), false);

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void oled_start(void)
{
    // Assuming the display is 128x64
    i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
    ssd1306_init(&dev, 128, 64);

    ssd1306_clear_screen(&dev, false);
    ssd1306_contrast(&dev, 0xff);

    ESP_LOGI(TAG, "Configuração finalizada");
}
