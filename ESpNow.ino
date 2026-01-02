#include "EspNow.h"

// Endereço MAC do receptor (substitua pelo seu)
uint8_t receptorMAC[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Instancia a classe (Pino 4 para o botão, por exemplo)
EspNow comunicacao(receptorMAC, 4);

void setup() {
  Serial.begin(115200);
  comunicacao.nowSetup();
}

void loop() {
  comunicacao.beginRun();
}


// Chamada com millis
/*
void loop() {
  // A função update verifica o tempo. Se não for a hora, ela pula instantaneamente.
  // Isso permite que o loop() rode milhares de vezes por segundo.
  comunicacao.update(50); 

  // Outras funções rodam aqui sem serem afetadas
  digitalWrite(LED_BUILTIN, HIGH); 
}
*/

// Função que Rode em background 
// 1. No ESP32: Usando o Segundo Núcleo (Task)
// O ESP32 possui dois núcleos. O loop() roda no núcleo 1. Você pode criar uma tarefa para rodar no núcleo 0 de forma totalmente independente.
/*
void TaskBackground(void *pvParameters) {
  for (;;) { // Loop infinito da tarefa
    comunicacao.beginRun();
    vTaskDelay(10 / portTICK_PERIOD_MS); // Pequena pausa para o Watchdog
  }
}

void setup() {
  Serial.begin(115200);
  comunicacao.nowSetup();

  // Cria a tarefa no núcleo 0
  xTaskCreatePinnedToCore(
    TaskBackground,   // Função da tarefa
    "EnvioEspNow",    // Nome
    10000,            // Stack size
    NULL,             // Parâmetros
    1,                // Prioridade
    NULL,             // Handle
    0                 // Núcleo 0
  );
}

void loop() {
  // O loop fica livre para outras coisas!
  Serial.println("Rodando loop principal...");
  delay(1000);
}
*/

// 2. No ESP8266 (ou ESP32): Usando Ticker
// Como o ESP8266 tem apenas um núcleo, ele simula o background usando interrupções de tempo (timers). A biblioteca Ticker é ideal para isso.
/*
#include <Ticker.h>

Ticker timerEspNow;

void setup() {
  Serial.begin(115200);
  comunicacao.nowSetup();

  // Executa a função a cada 50ms (0.05 segundos)
  timerEspNow.attach(0.05, []() {
    comunicacao.beginRun();
  });
}

void loop() {
  // loop() livre para sensores, display, etc.
}
*/

// 3. Usar millis() é conhecida como Multitarefa Cooperativa
// compatível com qualquer placa (ESP32, ESP8266, Arduino Uno, etc.).
/*
unsigned long tempoAnterior = 0;   // Armazena o último tempo registrado
const long intervalo = 50;         // Intervalo desejado em milissegundos

void loop() {
  unsigned long tempoAtual = millis(); // Obtém o tempo atual desde o início do programa

  // Verifica se a diferença entre o tempo atual e o anterior é maior que o intervalo
  if (tempoAtual - tempoAnterior >= intervalo) {
    tempoAnterior = tempoAtual; // Atualiza o marcador de tempo
    
    comunicacao.beginRun(); // Executa sua função de ESP-NOW
  }

  // O código aqui embaixo continua rodando SEM PARAR
  // Você pode ler outros sensores ou piscar LEDs aqui
}
*/