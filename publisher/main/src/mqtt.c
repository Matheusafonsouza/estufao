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

#include "mqtt.h"

#define TAG "MQTT"

extern float MOISTURE_VALUE;
extern float TEMPERATURE;
extern float HUMIDITY;
extern SemaphoreHandle_t mqtt_semaphore;

esp_mqtt_client_handle_t esp_client;

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Ultimo erro %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Evento dispachado do loop base %s, ID %d", base, (int)event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        xSemaphoreGive(mqtt_semaphore);
        msg_id = esp_mqtt_client_subscribe(client, "v1/devices/me/rpc/request/+", 0);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        ESP_LOGI(TAG, "TOPICO %.*s\r", event->topic_len, event->topic);
        ESP_LOGI(TAG, "DATA %.*s\r", event->data_len, event->data);

        cJSON *root = cJSON_Parse(event->data);

        if (root != NULL)
        {
            cJSON *method = cJSON_GetObjectItemCaseSensitive(root, "method");

            if (method != NULL && cJSON_IsString(method))
                {
                ESP_LOGE(TAG, "Evento RPC desconhecido: %s", method->valuestring);
            }
            else
            {
                printf("Erro ao obter o método via JSON");
            }

            cJSON_Delete(root);
        }

        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "String do ultimo erro: (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "ID de evento desconhecido:%d", event->event_id);
        break;
    }
}

void mqtt_esp_start()
{
    esp_mqtt_client_config_t mqtt_config = {
        .broker.address.uri = "mqtt://192.168.1.17:1883",
        .credentials.username = "user2",
        .credentials.authentication.password = "120405",
    };
    esp_client = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(esp_client, ESP_EVENT_ANY_ID, mqtt_event_handler, esp_client);
    esp_mqtt_client_start(esp_client);
}

void mqtt_esp_send(char *topico, char *mensagem)
{
    int message_id = esp_mqtt_client_publish(esp_client, topico, mensagem, 0, 1, 0);
}

void mqtt_esp_task(void *params)
{
    char mensagem[200];
    char jsonAtributos[200];

    if (xSemaphoreTake(mqtt_semaphore, portMAX_DELAY))
    {
        while (true)
        {
            sprintf(mensagem, "{\"moisture\": %f, \"temperature\": %f, \"humidity\": %f}", MOISTURE_VALUE, TEMPERATURE, HUMIDITY);
            mqtt_esp_send("telemetry", mensagem);
            vTaskDelay(3000 / portTICK_PERIOD_MS);
        }
    }
}
