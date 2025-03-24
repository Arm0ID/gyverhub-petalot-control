#include <GyverHub.h>
GyverHub hub("MyDevices", "PETALOT", "");  // имя сети, имя устройства, иконка
#include "secrets.h"

// Глобальные переменные-флаги
bool flagHotendEnable = false;  // Флаг включения нагревателя
bool flagStepperEnable = false;  // Флаг включения вращения

void buttonStateHandler(int flagId);


// билдер
void build(gh::Builder& b) {

    //Первый виджет, общее состояние системы
    {
    gh::Row r(b);
    b.Label("Status:").size(3).noLabel().align(gh::Align::Left).fontSize(32);
    b.LED().value(0).size(1).noLabel();
    }
    //Второй виджет, управление температурой нагревателя
    {
    gh::Col r(b);
        {
        gh::Row r(b);
        b.Label("Температура:").noTab().noLabel().align(gh::Align::Left).fontSize(24).size(3);
        b.LED("hotendLed").value(0).size(1).noLabel().noTab();
        }
    b.GaugeLinear().value(33).icon("").range(0,300,1).unit("°").noLabel().size(2);
        {
        gh::Row r(b);
        b.Label("Управление хотэндом:").noLabel().align(gh::Align::Left).fontSize(16).size(3);
        b.Spinner().value(230).noLabel().range(190,270,2).size(2);
        if (b.Button().icon("").noLabel().size(1).click()) buttonStateHandler(1);
        }
    }
    //Третий виджет, управление скоростью ШД
    {
        gh::Col r(b);
            {
            gh::Row r(b);
            b.Label("Скорость ШД:").noTab().noLabel().align(gh::Align::Left).fontSize(24).size(3);
            b.LED_("stepperLed").value(0).size(1).noLabel().noTab();
            
            }
        b.GaugeLinear().value(20).icon("").range(5,20,1).unit(" См/мин").noLabel().size(2);
            {
            gh::Row r(b);
            b.Label("Намотки нити:").noLabel().align(gh::Align::Left).fontSize(16).size(3);
            b.Spinner().value(230).noLabel().range(5,20,1).size(2);
            //if (b.Button().icon("").noLabel().size(1).click()) buttonStateHandler(2);
            if (b.Button().click()) hub.update(F("stepperLed")).value(1);
            }
        }
}


// Функция для изменения значения флага
void buttonStateHandler(int flagId) {
    Serial.println("Функция buttonStateHandler, параметр: " + String(flagId));
    if (flagId == 1) {
        flagHotendEnable = !flagHotendEnable;  // Изменяем флаг для первой кнопки
    } else if (flagId == 2) {
        flagStepperEnable = !flagStepperEnable;  // Изменяем флаг для второй кнопки
    }
}


void hubStateHandler() {
    if (flagHotendEnable == true) {
        hub.update(F("hotendLed")).value(1); 
    } else {
        hub.update(F("hotendLed")).value(0);
    }

    if (flagStepperEnable == true) {
        hub.update(F("stepperLed")).value(1);
    } else {
        hub.update(F("stepperLed")).value(0);
    }
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
    Serial.println("Petalot GyverHub запущен!");
}

void loop() {
    // hubStateHandler();
    hub.tick();         // тикаем тут
    
}