#ifndef MQTT_H
#define MQTT_H

void mqtt_start();
void mqtt_send(char * topico, char * mensagem);
void mqtt_task(void *params);

#endif // MQTT
