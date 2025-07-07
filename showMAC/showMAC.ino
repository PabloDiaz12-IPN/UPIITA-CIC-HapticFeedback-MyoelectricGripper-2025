#include <esp_now.h>
#include <WiFi.h>

void setup() {
  delay(5000);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
}


void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
}
