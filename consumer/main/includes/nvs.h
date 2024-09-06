#ifndef NVS_H
#define NVS_H

#include <stdint.h>

int get_nvs(char *chave);
void save_nvs(int32_t valor, char *chave);
void save_nvs_task();
void set_values_nvs();

#endif // NVS
