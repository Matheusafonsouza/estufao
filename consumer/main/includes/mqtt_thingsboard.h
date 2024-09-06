#ifndef MQTT_H_THINGSBOARD
#define MQTT_H_THINGSBOARD

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "cJSON.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

typedef struct esp_mqtt_client *esp_mqtt_client_handle_t;

void mqtt_thingsboard_start();
void mqtt_thingsboard_send(char * topico, char * mensagem);
void mqtt_thingsboard_task(void *params);

#endif // MQTT_THINGSBOARD
