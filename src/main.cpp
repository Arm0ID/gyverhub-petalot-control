#include <GyverHub.h>
#include "secrets.h"
#include "thermosense.h"
#include "GyverPID.h"

// Экземпляр GyverHUB
GyverHub hub("MyDevices", "PETALOT", "");  // имя сети, имя устройства, иконка

// Экземпляр GyverPID
GyverPID regulator(1, 0, 0);

// Экземпляр таймера
gh::Timer tmr2(300);

// Глобальные переменные-флаги
bool flagHotendEnable = true;  // Флаг включения нагревателя
bool flagStepperEnable = false;  // Флаг включения вращения
static volatile int a = 0;
bool hotendLedState = false;
bool stepperLedState = false;

int hotendSpinnerValue = 70; // Хранит базовое значение спиннера хотенда и нагрев

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
        b.LED_("hotendLed",&hotendLedState).value(0).size(1).noLabel().noTab();
        }
    b.GaugeLinear_("hotendGaugeLinear").value(33).icon("").range(0,300,1).unit("°").noLabel().size(2);
        {
        gh::Row r(b);
        b.Label("Управление хотэндом:").noLabel().align(gh::Align::Left).fontSize(16).size(3);
        b.Spinner_("hotendSpinner",&hotendSpinnerValue).value(hotendSpinnerValue).noLabel().range(40,270,2).size(2); //TODO: верни ограничения на 190
        if (b.Button().icon("").noLabel().size(1).click()) {
            #ifdef logEnable
            Serial.println("Нажата кнопка включения/выключения хотенда.");
            #endif
            flagHotendEnable = !flagHotendEnable;
            hotendLedState = !hotendLedState; 
            hub.update("hotendLed").value(hotendLedState);
        }
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
            if (b.Button().icon("").noLabel().size(1).click()) {
                #ifdef logEnable
                Serial.println("Нажата кнопка включения/выключения ШД.");
                #endif
                flagStepperEnable = !flagStepperEnable;
            }
            }
        }
    
}


void hubStateHandler() {

    if (flagStepperEnable == true) {
        hub.update("stepperLed").value(1);
    } else {
        hub.update("stepperLed").value(0);
    }

    if (tmr2) {
        double Temp = thermosenseMeasurment();

        //Обновляем температуру в GUI
        hub.update("hotendGaugeLinear").value(Temp);
      
        if (flagHotendEnable == true) {
            //Отправляем сигнал хотенду
            regulator.input = Temp;
            analogWrite(32, regulator.getResult());
            regulator.setpoint = hub.getValue("hotendSpinner").toInt();
        } else analogWrite(32, 0);

        // Логирование
        #ifdef logEnable
            String L = "Температура хотенда: " + String(Temp) +  
                        " Команда GyverPID: " + String(regulator.getResult()) + 
                        " Установлена температура: " + hub.getValue("hotendSpinner");
            Serial.println(L);
        #endif
    }
}


void setup() {

    #ifdef logEnable
    Serial.begin(115200);
    #endif

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

    // Инициализация ТермоИзмерений
    thermosenseSetup();

    // Инициализация GyverPID
    regulator.setDirection(NORMAL); // направление регулирования (NORMAL/REVERSE). ПО УМОЛЧАНИЮ СТОИТ NORMAL
    regulator.setLimits(0, 255);    // пределы (ставим для 8 битного ШИМ). ПО УМОЛЧАНИЮ СТОЯТ 0 И 255
    regulator.setpoint = 50;        // сообщаем регулятору температуру, которую он должен поддерживать
}

void loop() {
    hubStateHandler();
    hub.tick();         // тикаем тут     
}