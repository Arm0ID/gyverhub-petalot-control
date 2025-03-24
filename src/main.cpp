#include <GyverHub.h>
GyverHub hub("MyDevices", "PETALOT", "");  // имя сети, имя устройства, иконка
#include "secrets.h"

// билдер
void build(gh::Builder& b) {
}

void setup() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    // настройка MQTT/Serial/Bluetooth..
    hub.onBuild(build); // подключаем билдер
    hub.begin();        // запускаем систему
}

void loop() {
    hub.tick();         // тикаем тут
}