#ifndef _GPIO_SETUP
#define _GPIO_SETUP

#include <stdio.h>
#include "driver/gpio.h"

#define GPIO_OUTPUT          0
#define GPIO_INPUT           1
#define GPIO_INPUT_PULLUP    2
#define GPIO_INPUT_PULLDOWN  3
#define GPIO_ANALOG          4

void pin_mode(gpio_num_t pin, int mode);
void digital_write(gpio_num_t pin, uint32_t level);
uint32_t digital_read(gpio_num_t pin);

#endif // GPIO_SETUP
