#include "EspNow.h"

// Endereço MAC do receptor (substitua pelo seu)
std::array<uint8_t, 6> receptorMAC = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Instancia a classe (Pino 4 para o botão, por exemplo)
EspNow comunicacao(receptorMAC, 4);

void setup() {
  Serial.begin(115200);
  comunicacao.nowSetup();
}

void loop() {
  comunicacao.beginRunSent();
}

// Para chamar a classe no setup() quando o módulo for apenas emissor
/*
#include <Arduino.h>
#include "EspNow.h"

// 1. Defina o endereço MAC do módulo RECEPTOR (o que tem o LED)
// Se quiser enviar para todos ao redor, use {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
std::array<uint8_t, 6> macReceptor = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}; 

// 2. Instancie a classe. 
// O primeiro parâmetro é o endereço acima.
// O segundo é o pino onde o BOTÃO está conectado (ex: GPIO 4)
EspNow emissor(macReceptor, 4);

void setup() {
    Serial.begin(115200);

    // 3. Inicializa o Wi-Fi e as configurações do protocolo ESP-NOW
    emissor.nowSetup();
    
    Serial.println("Configurado como Emissor.");
}

void loop() {
    // 4. Chama o update para verificar se o botão foi pressionado
    // O valor 50ms é o intervalo de checagem (debounce)
    emissor.updateSent(50);
}
*/

// Para configurar o módulo como apenas receptor
/*
#include <Arduino.h>
#include "EspNow.h"

// 1. Defina o endereço MAC do EMISSOR (opcional para o receptor simples)
// No receptor, o endereço passado aqui não afeta a recepção de dados, 
// mas é necessário para instanciar a classe.
std::array<uint8_t, 6> macEmissor = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 

// 2. Instancie a classe.
// O segundo parâmetro (2) é o pino onde o LED está conectado.
EspNow receptor(macEmissor, 2);

void setup() {
    Serial.begin(115200);

    // 3. Inicializa o ESP-NOW e registra os callbacks de recepção
    receptor.nowSetup();

    // 4. ESSENCIAL: Configura o pino como SAÍDA e inicializa o LED desligado
    receptor.beginRunRecv();

    Serial.println("Configurado como Receptor. Aguardando comandos...");
}

void loop() {
    // 5. O receptor não precisa de nada no loop!
    // Quando uma mensagem chega, a função OnDataRecv é chamada automaticamente
    // pelo sistema (via interrupção), executando a lógica do LED.
}
*/


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