#include <GyverHub.h>
#include "secrets.h"
#include "thermosense.h"
#include "GyverPID.h"

// Экземпляр GyverHUB
GyverHub hub("MyDevices", "PETALOT", "");  // имя сети, имя устройства, иконка

// Стартовые значения PID
float PID_P = 19.2;
float PID_I = 1;
float PID_D = 24.0;

// Экземпляр GyverPID
GyverPID regulator(PID_P, PID_I, PID_D);

// Экземпляр таймера
gh::Timer tmr2(100);

// Глобальные переменные-флаги
bool flagHotendEnable = false;  // Флаг включения нагревателя
bool flagStepperEnable = false;  // Флаг включения вращения
bool hotendLedState = false;
bool stepperLedState = false;
static int tempCounter = 0; // Временное хранилище для построения графиков

int hotendSpinnerValue = 120; // Хранит базовое значение спиннера хотенда и нагрев

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
    //Четвертый виджет, настройка PID
    {
        gh::Col r(b);
            b.Space().size(1,10);
            {
                gh::Row r(b);
                b.Label("Калибровка PID:").noTab().noLabel().align(gh::Align::Left).fontSize(24).size(3);
                #ifdef buildGraph
                if (b.Button().icon("").noLabel().noTab().size(1,10).click()) {
                    Serial.println("");
                    Serial.println("Сброс и начало новых измерений до 1000.");
                    Serial.println("");
                    tempCounter = 0;
                }
                #endif
                b.LED_("heatingLed").value(0).size(1).label("нагрев:").noTab().fontSize(12);
            }
            {
                gh::Row r(b);
                b.Space().size(1);
                b.Input_("inputPID_P", &PID_P).value(PID_P).size(3).fontSize(16).label("Коэф. P");
                b.Space().size(1);
                b.Input_("inputPID_I", &PID_I).value(PID_I).size(3).fontSize(16).label("Коэф. I");
                b.Space().size(1);
                b.Input_("inputPID_D", &PID_D).value(PID_D).size(3).fontSize(16).label("Коэф. D");
                b.Space().size(1);

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
        
        #ifdef buildGraph
        
        if (tempCounter < 1000) {
            ++tempCounter;
            Serial.println(Temp);
        }
            
        #endif


        // Логирование
        #ifdef logEnable
            String L = "Температура хотенда: " + String(Temp) +  
                        " Команда GyverPID: " + String(regulator.getResult()) + 
                        " Установлена температура: " + hub.getValue("hotendSpinner");
            Serial.println(L);
        #endif

        regulator.Kp = hub.getValue("inputPID_P").toFloat();
        regulator.Ki = hub.getValue("inputPID_I").toFloat();
        regulator.Kd = hub.getValue("inputPID_D").toFloat();

        if (regulator.getResult() > 0) hub.update("heatingLed").value(1);
        else hub.update("heatingLed").value(0);
    }

}


void setup() {

    #ifdef logEnable
    Serial.begin(115200);
    #endif
    Serial.begin(115200);
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