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
        // Verificar se o botão foi pressionado (o nível é baixo devido ao pull-up)
        if (digital_read(BUTTON_PIN) == 0)
        {
            change_display_mode();
            ESP_LOGI(TAG, "Botão pressionado!");
            // Acender o LED
            digital_write(LED_PIN, 1);
            // Aguardar um curto período para evitar leituras múltiplas rápidas
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        else
        {
            // Desligar o LED quando o botão não está pressionado
            digital_write(LED_PIN, 0);
        }
        // Aguardar um curto período antes de verificar novamente
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
