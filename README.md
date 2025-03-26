# gyverhub-petalot-control

<div align="center">
  <img src="screenshots/tractor-solid.svg" width="30%">
</div>

Проект для управления Petalot через веб-интерфейс GyverHub.

## Основные функции
- Удаленное управление через Wi-Fi.
- Мониторинг температуры, скорости и других параметров.
- Отдельный настроенный PID тюнер для хотэнда от 3D принтера(pid_autotune).

## Зависимости
- **[GyverHub](https://github.com/GyverLibs/GyverHub)**: Веб-интерфейс для управления.
- **[Petalot](https://github.com/function3d/petalot)**: Машина для переработки пластика.
- **[NTC with ESP32](https://github.com/e-tinkers/ntc-thermistor-with-arduino-and-esp32/tree/master)**: Проект по измерениям терморезистора (прикручен алгоритм фильтрации)

## Железо/Компоненты
- **ESP32**
- **NTC3950 100кОм**
- **Резистор 100кОм**
- **Конденсаторы 0,1мкФ**
- **Нагревательный блок с нагревательным элементом**
- **Транзистор IRLZ44n**
- **Резистор 10кОм и 100 Ом**

## Настройка Wi-Fi
Создайте файл `src/secrets.h` со следующим содержимым:

```cpp
#ifndef SECRETS_H
#define SECRETS_H

const char* WIFI_SSID = "ваш_SSID";
const char* WIFI_PASSWORD = "ваш_пароль";

#endif
```

## Возможность конфигурации:
- **Включение/отключение логирования**
- **Настройка медианного фильтра сигнала с терморезистора**
- **Включение/Отключение вывода данных для построения графика температур**

