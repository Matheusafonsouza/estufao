#ifndef _MOISTURE_
#define _MOISTURE_

#include "esp_adc/adc_oneshot.h"

void adc_init();
void adc_config_pin(adc_channel_t channel);
void adc_deinit();
int analog_read(adc_channel_t channel);
void moisture_task(void *params);

#endif // _MOISTURE_
