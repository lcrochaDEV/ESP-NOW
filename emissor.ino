//Programa: ESP-NOW com ESP8266 NodeMCU - Emissor
//Autor: Arduino e Cia

#include <ESP8266WiFi.h>
#include <espnow.h>

//Coloque na linha abaixo o Mac Address do NodeMCU receptor
uint8_t broadcastAddress[] = {0x60, 0x01, 0x94, 0x0A, 0x87, 0x72};

//Estrutura para envio dos dados. Deve ser a mesma tanto no emissor como no receptor.
typedef struct struct_message {
  String a;
} 
struct_message;

//Definicoes botao
#define pino_botao D2
bool inverte_led = 0;
int valor;

//Cria uma struct_message chamada myData
struct_message myData;

//Callback quando os dados sao enviados
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Status dos dados enviados: ");
  if (sendStatus == 0) {
    Serial.println("Envio ok!");
  } else {
    Serial.println("Falha no envio!");
  }
}

void setup() {
  Serial.begin(115200);

  //Inicializa o pino do botao
  pinMode(pino_botao, INPUT);

  //Coloca o dispositivo no modo Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Inicializa o ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Erro ao inicializar o ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);

  //Registra o destinatario da mensagem
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
  valor = digitalRead(pino_botao);
  if (valor == 1) {
    while (digitalRead(pino_botao) == 1) {
      delay(50);
    }
    Serial.println("Botao pressionado!");
    //Inverte o estado do led
    inverte_led = !inverte_led;

    //Envia a string de acordo com o estado do led
    if (inverte_led == 1) {
      myData.a = "L_Led";
    }
    if (inverte_led == 0) {
      myData.a = "D_Led";
    }

    //Envia a mensagem usando o ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  }
}