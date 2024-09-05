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
int DISPLAY_MODE = 0;
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

        if (DISPLAY_MODE == 0)
        {
            sprintf(&lineChar[0], " Temp. Ar: %.1fC", TEMPERATURE);
            ssd1306_display_text(&dev, 2, lineChar, strlen(lineChar), false);
        }
        else if (DISPLAY_MODE == 1)
        {
            sprintf(&lineChar[0], " Umid. Ar: %.1f%%", HUMIDITY);
            ssd1306_display_text(&dev, 4, lineChar, strlen(lineChar), false);
        }
        else if (DISPLAY_MODE == 2)
        {
            sprintf(&lineChar[0], " Um. Solo: %.1f%%", MOISTURE_VALUE);
            ssd1306_display_text(&dev, 6, lineChar, strlen(lineChar), false);
        }

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

void change_display_mode()
{
    oled_clear();

    if (DISPLAY_MODE == 0)
    {
        DISPLAY_MODE = 1;
        ESP_LOGI(TAG, "Modo atualizado, mostrando emoji");
        return;
    }

    if (DISPLAY_MODE == 1)
    {
        DISPLAY_MODE = 2;
        ESP_LOGI(TAG, "Modo atualizado, mostrando dados dos sensores");
        return;
    }

    if (DISPLAY_MODE == 2)
    {
        DISPLAY_MODE = 0;
        ESP_LOGI(TAG, "Modo atualizado, mostrando dados dos sensores");
        return;
    }

    ESP_LOGE(TAG, "Modo desconhecido %d", DISPLAY_MODE);
}
