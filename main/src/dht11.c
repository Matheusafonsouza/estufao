/*
 * MIT License
 *
 * Copyright (c) 2018 Michele Biondi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define TAG "DHT"

#include "driver/gpio.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"

#include "dht11.h"

float TEMPERATURE;
float HUMIDITY;

static gpio_num_t dht_gpio;
static int64_t last_read_time = -2000000;
static struct dht11_reading last_read;

static int _wait_or_timeout(uint16_t microSeconds, int level) {
    int micros_ticks = 0;
    while (gpio_get_level(dht_gpio) == level) {
        if (micros_ticks++ > microSeconds)
            return DHT11_TIMEOUT_ERROR;
        ets_delay_us(1);
    }
    return micros_ticks;
}

static int _check_crc(uint8_t data[]) {
    if (data[4] == (data[0] + data[1] + data[2] + data[3]))
        return DHT11_OK;
    else
        return DHT11_CRC_ERROR;
}

static void _send_start_signal() {
    gpio_set_direction(dht_gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(dht_gpio, 0);
    ets_delay_us(20 * 1000);
    gpio_set_level(dht_gpio, 1);
    ets_delay_us(40);
    gpio_set_direction(dht_gpio, GPIO_MODE_INPUT);
}

static int _check_response() {
    if (_wait_or_timeout(80, 0) == DHT11_TIMEOUT_ERROR)
        return DHT11_TIMEOUT_ERROR;

    if (_wait_or_timeout(80, 1) == DHT11_TIMEOUT_ERROR)
        return DHT11_TIMEOUT_ERROR;

    return DHT11_OK;
}

static struct dht11_reading _timeout_error() {
    struct dht11_reading timeoutError = {DHT11_TIMEOUT_ERROR, -1, -1};
    return timeoutError;
}

static struct dht11_reading _crc_error() {
    struct dht11_reading crcError = {DHT11_CRC_ERROR, -1, -1};
    return crcError;
}

void dht11_init() {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    dht_gpio = DHT_PIN;
}

struct dht11_reading dht11_read() {
    if (esp_timer_get_time() - 2000000 < last_read_time) {
        return last_read;
    }

    last_read_time = esp_timer_get_time();

    uint8_t data[5] = {0, 0, 0, 0, 0};

    _send_start_signal();

    if (_check_response() == DHT11_TIMEOUT_ERROR)
        return last_read = _timeout_error();

    for (int i = 0; i < 40; i++) {
        if (_wait_or_timeout(50, 0) == DHT11_TIMEOUT_ERROR)
            return last_read = _timeout_error();

        if (_wait_or_timeout(70, 1) > 28) {
            data[i / 8] |= (1 << (7 - (i % 8)));
        }
    }

    if (_check_crc(data) != DHT11_CRC_ERROR) {
        last_read.status = DHT11_OK;
        last_read.temperature = data[2];
        last_read.humidity = data[0];
        return last_read;
    } else {
        return last_read = _crc_error();
    }
}

void dht11_task(void *params)
{
    dht11_init();
    while (true) {
        struct dht11_reading result = dht11_read();
        if (result.status == DHT11_OK) {
            TEMPERATURE = result.temperature;
            HUMIDITY = result.humidity;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
