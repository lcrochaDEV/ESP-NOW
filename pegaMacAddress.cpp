#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
 
void setup(){
  delay(2000);
  Serial.begin(115200); /*defined baud rate*/
  Serial.println();
  Serial.print("ESP32 Station Interface MAC Address: ");
  Serial.println(WiFi.macAddress());  /*Prints ESP32 MAC address in Station Mode*/
}
 
void loop(){}