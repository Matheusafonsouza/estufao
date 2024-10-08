/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_adc/adc_oneshot.h"
#include "gpio_setup.h"

#define TAG "MOISTURE"
#define MOISTURE ADC_CHANNEL_6

float MOISTURE_VALUE;
adc_oneshot_unit_handle_t adc1_handle;
adc_oneshot_unit_init_cfg_t init_config1;

void adc_init(adc_unit_t adc_unit)
{
    init_config1.unit_id = adc_unit;
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));
}

void adc_config_pin(adc_channel_t channel)
{
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_11,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, channel, &config));
}

void adc_deinit()
{
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));
}

int analog_read(adc_channel_t channel)
{
    int adc_raw;

    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, channel, &adc_raw));

    return adc_raw;
}

void moisture_task(void *params)
{
    adc_init(ADC_UNIT_1);
    pin_mode(MOISTURE, GPIO_ANALOG);

    while (true)
    {
        int moisture = analog_read(MOISTURE);

        MOISTURE_VALUE = 100.0 - ((moisture / 4095.0) * 100.0);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
