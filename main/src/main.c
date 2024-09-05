#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "freertos/semphr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"

#include "wifi.h"
#include "gpio_setup.h"
#include "moisture.h"
#include "http_client.h"
#include "mqtt.h"
#include "dht11.h"
#include "nvs.h"
#include "oled.h"
#include "button.h"

#define TAG "MAIN"

SemaphoreHandle_t wifi_semaphore;
SemaphoreHandle_t mqtt_semaphore;

void app_main(void)
{
  esp_err_t ret = nvs_flash_init();

  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }

  ESP_ERROR_CHECK(ret);

  set_values_nvs();

  wifi_semaphore = xSemaphoreCreateBinary();
  mqtt_semaphore = xSemaphoreCreateBinary();

  // wifi_start();
  oled_start();

  xTaskCreate(&wifi_task, "conexao_mqtt", 4096, NULL, 1, NULL);
  xTaskCreate(&mqtt_task, "comunicacao_broker", 4096, NULL, 1, NULL);
  xTaskCreate(&dht11_task, "dht", 2048, NULL, 3, NULL);
  xTaskCreate(&moisture_task, "sensor_solo", 2048, NULL, 3, NULL);
  xTaskCreate(&save_nvs_task, "armazenamento_nvs", 2048, NULL, 3, NULL);
  xTaskCreate(&oled_display_info_task, "render_display", 2048, NULL, 2, NULL);
  xTaskCreate(&button_task, "button", 2048, NULL, 2, NULL);
}
