# Projeto Estufão 


O Projeto Estufão é um sistema de monitoramento de condições ambientais, focado no cuidado com plantas. Ele monitora dados de temperatura do ar, umidade do ar, e 
umidade do solo, usando sensores apropriados para essas medições. ele se concentra em fornecer informações objetivas e em tempo real sobre o ambiente da planta.

## Componentes 

Servidor Central - Dashboard Thingsboard, Cliente ESP32 distribuído e as seguintes entradas e saídas  associadas: 

| Entradas | 
| - | 
| Botão (Presente na placa)|
| Sensor de Temperatura e Umidade do Ar DHT22 |
| Sensor Resistivo de Umidade de Solo | 


| Saídas | 
| - | 
| LED (Presente na placa) |
| OLED SSD1306 |

## Funcionalidades

O módulo OLED SSD1306 possui somente um modo de informações. As informações apresentadas são as leituras de temperatura do ar, umidade do ar e umidade do solo. Acionando o botão as informações 
do módulo OLED SSD1306 serão alternadas.

A partir do uso do Servidor Central Thingsboard foi criado o Dashboard para visualizar em tempo real  as leituras dos sensores (telemetria e estado dos atributos), que são enviadas via MQTT. Além disso, o Dashboard apresenta gráficos com a variação dessas leituras ao longo do tempo.

Todos os estados dos atributos são armazenados na memória NVS, de modo que ao ser reinicializada, a placa volta exatamente para o estado onde estava.

---
## Fundamentos de Sistemas Embarcados 2024.1

| Estudante | Matrícula | 
| - | - |
| Mateus Maia | 180106805 | 
| Matheus Afonso | 180127641  | 
