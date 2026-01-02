#include "EspNow.h"
#include <string.h>

//Callback quando os dados sao enviados
#if defined(ESP32)
void EspNow::OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.printf("\nStatus do envio: %s\n", status == ESP_NOW_SEND_SUCCESS ? "Sucesso" : "Falha");
}
#else
void EspNow::OnDataSent(uint8_t *mac_addr, uint8_t status) {
    Serial.printf("\nStatus do envio: %s\n", status == 0 ? "Sucesso" : "Falha");
}
#endif

EspNow::EspNow(const std::array<uint8_t, 6>& broadcastAddress, int pinNumber)
    : broadcastAddress(broadcastAddress), pinNumber(pinNumber)
{}

void EspNow::nowSetup() {
  //Serial.begin(115200);

    //Inicializa o pino do botao
    if (pinNumber != -1) {
        pinMode(pinNumber, INPUT);
    }

    //Coloca o dispositivo no modo Wi-Fi Station
    WiFi.mode(WIFI_STA);

    //Inicializa o ESP-NOW
    if (esp_now_init() != 0) {
        Serial.println("Erro ao inicializar o ESP-NOW");
        return;
    }
    #if defined(ESP8266)
        esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
        esp_now_register_send_cb(OnDataSent);
        esp_now_add_peer(broadcastAddress.data(), ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
    #elif defined(ESP32)
        esp_now_register_send_cb(OnDataSent);
        esp_now_peer_info_t peerInfo = {};
        memcpy(peerInfo.peer_addr, broadcastAddress, 6);
        peerInfo.channel = 0;
        peerInfo.encrypt = false;
        if (esp_now_add_peer(&peerInfo) != ESP_OK) {
            Serial.println("Falha ao adicionar peer");
        }
    #endif
}
void EspNow::beginRun() {

    if (pinNumber != -1 && digitalRead(pinNumber) == HIGH) { // HIGH ou 1, dependendo da sua ligação
        while (digitalRead(pinNumber) == HIGH) {
        delay(50);
    }
    Serial.println("Botao pressionado!");
    //Inverte o estado do led
    inverte_led = !inverte_led;
    //Envia a string de acordo com o estado do led
    if (inverte_led) {
        strcpy(myData.a, "L_Led");
    } else {
        strcpy(myData.a, "D_Led");
    }
    // Envia a mensagem usando o ESP-NOW
    esp_now_send(broadcastAddress.data(), (uint8_t *) &myData, sizeof(myData));
  }
}

void EspNow::update(uint32_t intervalMs) {
    uint32_t tempoAtual = millis();

    // Técnica de millis() integrada à classe
    if (tempoAtual - _ultimoTempo >= intervalMs) {
        _ultimoTempo = tempoAtual;
        
        // Chama a função de leitura e envio
        beginRun(); 
    }
}