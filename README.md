# 📡 Documentação: Biblioteca EspNow Personalizada

Esta biblioteca encapsula o protocolo **ESP-NOW**, permitindo a comunicação sem fio entre dispositivos ESP32 e ESP8266 de forma bidirecional e simplificada através de **Tags**.

## 🧠 Lógica de Funcionamento

O sistema baseia-se em **Comandos de Texto Dinâmicos**. Em vez de enviar apenas um número (0 ou 1), enviamos uma string formatada como `Acao_Identificador` (ex: `On_Luz`).

1. **Tag System**: Cada dispositivo possui um apelido (`tagName`). Isso permite que um único emissor controle múltiplos receptores de forma seletiva.
2. **Sincronia Local/Remota**: O emissor, ao gerar um comando, aplica-o primeiro ao seu próprio hardware através do `processCommand` e depois o envia via rádio.
3. **Processamento Único**: A função `processCommand` é o cérebro da classe. Ela é estática para poder ser chamada tanto por interrupções de hardware (recebimento de dados) quanto pelo fluxo principal do programa.

---

## 📝 Exemplo 1: Emissor (Botão controla LED remoto)

Neste modo, o ESP foca em monitorar um pino de entrada e disparar a mensagem quando detecta uma mudança de estado.

```cpp
#include <Arduino.h>
#include "EspNow.h"

// SUBSTITUA pelo Endereço MAC do ESP Receptor
std::array<uint8_t, 6> macReceptor = {0x24, 0x6F, 0x28, 0xAE, 0xD5, 0x30};

// Pino 4 como botão, Tag identificadora "Cozinha"
EspNow emissor(macReceptor, 4, "Cozinha");

void setup() {
    Serial.begin(115200);
    
    // Inicializa rádio e peer
    emissor.nowSetup();
    
    // Opcional: Se o emissor também tiver um LED no pino 4 para feedback local
    emissor.beginRunRecv(); 
}

void loop() {
    // Verifica o botão a cada 50ms (non-blocking)
    // Se pressionado, gera "On_Cozinha" ou "Off_Cozinha" e envia
    emissor.updateSent(50); 
}

```

---

## 📝 Exemplo 2: Receptor (Apenas aguarda comandos)

Neste modo, o ESP fica em "escuta". Ele não precisa do endereço MAC do emissor, pois o protocolo ESP-NOW aceita mensagens de qualquer peer registrado ou via broadcast.

```cpp
#include <Arduino.h>
#include "EspNow.h"

// Para o receptor, o endereço MAC de destino pode ser zero (não será usado para enviar)
std::array<uint8_t, 6> macVazio = {0, 0, 0, 0, 0, 0};

// Receptor monitora o Pino 2 (LED), com a Tag "Cozinha"
// Ele só ligará o LED se receber exatamente "On_Cozinha"
EspNow receptor(macVazio, 2, "Cozinha");

void setup() {
    Serial.begin(115200);
    
    // Inicializa rádio
    receptor.nowSetup();
    
    // Configura o pino 2 como saída e prepara para receber
    receptor.beginRunRecv();
}

void loop() {
    // O Receptor não precisa de funções no loop.
    // O processamento acontece via interrupção (OnDataRecv) em background.
}

```

---

## 🛠️ Detalhes dos Métodos Principais

### `processCommand(const char* message)`

É um método estático. Ele reconstrói internamente as strings esperadas baseadas na `_tagNameStatic`.

* Se `message` == `On_Tag` → Pino vai para **HIGH**.
* Se `message` == `Off_Tag` → Pino vai para **LOW**.

### `beginRunSent()`

Este método realiza o "trabalho sujo" do emissor:

1. Faz o **Debounce** do botão (espera soltar).
2. Alterna o estado da variável `inverte_led`.
3. Usa `snprintf` para criar a mensagem: `On_` + `Cozinha`.
4. Envia via rádio.

---

## ⚠️ Observações de Hardware

* **Pull-up/Pull-down**: O código assume que o botão envia sinal **HIGH** quando pressionado. Se seu botão for ativo em LOW, mude a lógica no `if (digitalRead(pinNumber) == HIGH)`.
* **MAC Address**: Cada ESP tem um endereço físico único. Você **deve** descobrir o MAC do receptor e colocá-lo no código do emissor.