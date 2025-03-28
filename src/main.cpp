#include "includes.h"
#include "secrets.h"     //  (Wi-Fi и т.д.)


// Экземпляр GyverStepper
GStepper2<STEPPER2WIRE> stepper(3200, 18, 19); //шагов/оборот, step, dir, en

// Экземпляр GyverHUB
GyverHub hub("MyDevices", "PETALOT", "");  // имя сети, имя устройства, иконка

// Стартовые значения PID
float PID_P = 31;
float PID_I = -0.3;
float PID_D = 32;

// Экземпляр GyverPID
GyverPID regulator(PID_P, PID_I, PID_D); 

// Экземпляр таймера
gh::Timer tmr2(100);
gh::Timer tmr3(200);

// Глобальные переменные-флаги
bool flagHotendEnable = false;  // Флаг включения нагревателя
bool flagStepperEnable = false;  // Флаг включения вращения
bool hotendLedState = false;
bool stepperLedState = false;
bool isfilamentCountingPlay = false;

int hotendSpinnerValue = 120; // Хранит базовое значение для спиннера хотенда и нагрева
int stepperSpeedValue = 20; // Хранит базовое значение для спиннера хотенда и нагрева

// // Настройка ШД
int minStepperSpeed = 1; //(см/минуту)
int maxStepperSpeed = 80; //(см/минуту) ~2об/секунду ШД, ~70см/минуту
int stepsPerRevolution = 3200; // Шагов на оборот ШД
float circumference = 29.02; // Длина окружности катушки(см)
float gearRatio = 46.125; // передаточное число

static float filamentCount = 0; //временный Счетчик филамента
static float currentFilamentCount = 0; //Общий счетчик филамент за сессию

// // Настройка намотки
// float firstCircuitFilamentSpool = 29.02; //(см.) Длина одного витка катушки
// float gearRatio = 46.125; // Коэф. понижения с ШД на катушку

void build(gh::Builder& b) { // билдер

    //Первый виджет, общее состояние системы
    {
    gh::Row r(b);
    b.Label("Status:").size(3).noLabel().align(gh::Align::Left).fontSize(32);
    b.LED().value(0).size(1).noLabel();
    }
    //Блок управления температурой нагревателя
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
    b.GaugeLinear_("stepperGaugeLinear").value(20).icon("").range(minStepperSpeed,maxStepperSpeed,1).unit(" См/мин").noLabel().size(2);
        {
        gh::Row r(b);
        b.Label("Намотки нити:").noLabel().align(gh::Align::Left).fontSize(16).size(3);
        b.Spinner_("stepperSpinner",&stepperSpeedValue).value(20).noLabel().range(minStepperSpeed,maxStepperSpeed,1).size(2);
        if (b.Button().icon("").noLabel().size(1).click()) {
            #ifdef logEnable
            Serial.println("Нажата кнопка включения/выключения ШД.");
            #endif
            flagStepperEnable = !flagStepperEnable;
        }
        }
    }
    // Блок с намотанном кол-вом
    {
        gh::Row r(b);
        b.Display_("displayFilamentCount", &filamentCount).value(filamentCount).size(4).fontSize(16).label("Наматано за ссесию(~см): ");
        if (b.Button().icon("").noLabel().noTab().size(1).click()) currentFilamentCount = 0;
        if (b.Button_("buttonPlay").icon("").noLabel().size(1).noTab().click()) {
            isfilamentCountingPlay = !isfilamentCountingPlay;
            if (isfilamentCountingPlay) stepper.reset();
            if (!isfilamentCountingPlay) {
                // Переход в режим PAUSE: сброс временного счетчика
                stepper.reset(); // Сбрасываем позицию мотора
                filamentCount = 0; // Сбрасываем временный счетчик
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
            #ifdef buildGraph
            b.Label("Онлайн катировки:").noTab().noLabel().align(gh::Align::Left).fontSize(16).size(3, 16);
            {
                gh::Row r(b);
                b.Display_("displayP").label("Коэф. P").align(gh::Align::Center).fontSize(16).size(1, 10);
                b.Display_("displayI").label("Коэф. I").align(gh::Align::Center).fontSize(16).size(1, 10);
                b.Display_("displayD").label("Коэф. D").align(gh::Align::Center).fontSize(16).size(1, 10);
            }
            {
                gh::Row r(b);
                b.Display_("displayTemp").label("Temp").align(gh::Align::Center).fontSize(16).size(1, 10);
                b.Display_("displayResult").label("Вывод Result(PID):").align(gh::Align::Center).fontSize(16).size(1, 10);
                b.Display_("displayOutput").label("Вывод Output(PID):").align(gh::Align::Center).fontSize(16).size(1, 10);
            }
            #endif
            #ifdef stepperLogging
                b.Display_("displayStepperPos").label("stepper pos").align(gh::Align::Center).fontSize(16);
            #endif
    }
}


void hubStateHandler() {

        double Temp = thermosenseMeasurment();

        //Обновляем температуру в GUI
        hub.update("hotendGaugeLinear").value(Temp);
      
        if (flagHotendEnable == true) {
            if (Temp > 20) { // Защита от отвала датчика
                regulator.input = Temp;
                analogWrite(32, int(regulator.getResult()));
                regulator.setpoint = hub.getValue("hotendSpinner").toInt();
                if (regulator.getResult() > 0) hub.update("heatingLed").value(1);
                else hub.update("heatingLed").value(0);
                hub.update("hotendLed").value(1);
            } else hub.sendPush("Что пошло не так с нагревателем...");
        } else {
            analogWrite(32, 0);
            hub.update("hotendLed").value(0);
            hub.update("heatingLed").value(0);
        }
        #ifdef logEnable // Логирование хотенда
            String L = "Температура хотенда: " + String(Temp) +  
                        " Команда GyverPID: " + String(regulator.getResult()) + 
                        " Установлена температура: " + hub.getValue("hotendSpinner");
            Serial.println(L);
        #endif

        regulator.Kp = hub.getValue("inputPID_P").toFloat();
        regulator.Ki = hub.getValue("inputPID_I").toFloat();
        regulator.Kd = hub.getValue("inputPID_D").toFloat();

        // Расчеты
        float required_speed = hub.getValue("stepperSpinner").toInt();
        float spoolRpm = required_speed / circumference;                     // Обороты катушки (об/мин)
        float motorRpm = spoolRpm * gearRatio;                            // Обороты шагового двигателя (об/мин)
        float motorRps = motorRpm / 60.0;                                  // Обороты шагового двигателя (об/сек)
        float stepFrequencyFloat = motorRps * stepsPerRevolution;       // Частота шагов (шагов/сек)

        //Устанавливаем скорость мотору
        stepper.setSpeed(static_cast<int32_t>(stepFrequencyFloat)); // шагов в секунду
        
        //Обновляем stepper линейную панель
        hub.update("stepperGaugeLinear").value(required_speed);


        int32_t currentSteps = stepper.getCurrent(); // Текущая позиция мотора (шаги)
        float motorRevolutions = static_cast<float>(currentSteps) / stepsPerRevolution; // Обороты двигателя
        float spoolRevolutions = motorRevolutions / gearRatio;                          // Обороты катушки
        float filamentCount = spoolRevolutions * circumference;                        // Длина намотанного филамента
        
  
        if (flagStepperEnable) hub.update("stepperLed").value(true);
        else hub.update("stepperLed").value(false);
        
        // Блок подсчета намотанного на катушку
        if (isfilamentCountingPlay) {
            // Если PLAY
            currentFilamentCount += filamentCount; // Увеличиваем общий счетчик
            hub.update("displayFilamentCount").value(currentFilamentCount); // Обновляем дисплей
            hub.update("buttonPlay").icon(""); // Меняем иконку кнопки на "PLAY"
        } else {
            // Если PAUSE
            hub.update("buttonPlay").icon(""); // Меняем иконку кнопки на "PAUSE"
        }

        











#ifdef stepperLogging
        hub.update("displayStepperPos").value(stepper.pos);
#endif
#ifdef buildGraph
        if (tempCounter < 1000) {
            ++tempCounter;
            Serial.println(Temp);
            
        } else if (tempCounter == 1000) {
            ++tempCounter;
            hub.sendPush("Замер тысячи измерений окончен.");
        }
        // Инфа на дисплеи
        hub.update("displayP").value(regulator.Kp);
        hub.update("displayI").value(regulator.Ki);
        hub.update("displayD").value(regulator.Kd);

        hub.update("displayTemp").value(Temp);
        hub.update("displayResult").value(regulator.getResult());
        hub.update("displayOutput").value(regulator.output);

#endif
    
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

    hub.setVersion("Arm0ID/gyverhub-petalot-control@" + String(VERSION));
}

void loop() {
    if (tmr2) {
        hubStateHandler();
        hub.tick();
    }
    if (flagStepperEnable) stepper.tick(); // Шагаем двигателем    
}