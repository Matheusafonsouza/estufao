#include <stdio.h>
#include <string.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#define TAG "NVS"

extern float MOISTURE_VALUE;
extern float TEMPERATURE;
extern float HUMIDITY;

int get_nvs(char *chave)
{
    // Inicia o acesso à partição padrão nvs
    ESP_ERROR_CHECK(nvs_flash_init());

    // Inicia o acesso à partição personalizada
    // ESP_ERROR_CHECK(nvs_flash_init_partition("DadosNVS"));

    int32_t valor = 0;
    nvs_handle particao_padrao_handle;

    // Abre o acesso à partição nvs
    esp_err_t res_nvs = nvs_open("armazenamento", NVS_READONLY, &particao_padrao_handle);

    // Abre o acesso à partição DadosNVS
    // esp_err_t res_nvs = nvs_open_from_partition("DadosNVS", "armazenamento", NVS_READONLY, &particao_padrao_handle);

    if (res_nvs == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGE(TAG, "Namespace: armazenamento, não encontrado");
    }
    else
    {
        esp_err_t res = nvs_get_i32(particao_padrao_handle, chave, &valor);

        switch (res)
        {
        case ESP_OK:
            // printf("Valor armazenado: %d\n", (int) valor);
            break;
        case ESP_ERR_NOT_FOUND:
            ESP_LOGE(TAG, "Valor não encontrado");
            return -1;
        default:
            ESP_LOGE(TAG, "Erro ao acessar o NVS (%s)", esp_err_to_name(res));
            return -1;
            break;
        }

        nvs_close(particao_padrao_handle);
    }

    return valor;
}

void save_nvs(int32_t valor, char *chave)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    // ESP_ERROR_CHECK(nvs_flash_init_partition("DadosNVS"));

    nvs_handle particao_padrao_handle;

    esp_err_t res_nvs = nvs_open("armazenamento", NVS_READWRITE, &particao_padrao_handle);
    // esp_err_t res_nvs = nvs_open_from_partition("DadosNVS", "armazenamento", NVS_READWRITE, &particao_padrao_handle);

    if (res_nvs == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGE(TAG, "Namespace armazenamento, não encontrado");
    }

    esp_err_t res = nvs_set_i32(particao_padrao_handle, chave, valor);

    if (res != ESP_OK)
    {
        ESP_LOGE(TAG, "Não foi possível escrever [%s]", esp_err_to_name(res));
    }

    ESP_LOGI(TAG, "Valor escrito [%s]", chave);

    nvs_commit(particao_padrao_handle);
    nvs_close(particao_padrao_handle);
}

void save_nvs_task()
{
    while (true)
    {
        save_nvs(MOISTURE_VALUE, "moisture_value");
        save_nvs(TEMPERATURE, "temperature");
        save_nvs(HUMIDITY, "humidity");

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void set_values_nvs()
{
    float moisture_value_nvs = get_nvs("moisture_value");
    float temperature_nvs = get_nvs("temperature");
    float humidity_nvs = get_nvs("humidity");

    ESP_LOGI(TAG, "Valor armazenado UMIDADE SOLO: %.1f", moisture_value_nvs);
    ESP_LOGI(TAG, "Valor armazenado TEMPERATURA: %.1f", temperature_nvs);
    ESP_LOGI(TAG, "Valor armazenado UMIDADE AR: %.1f", humidity_nvs);

    MOISTURE_VALUE = moisture_value_nvs;
    TEMPERATURE = temperature_nvs;
    HUMIDITY = humidity_nvs;
}
