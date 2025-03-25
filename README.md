# gyverhub-petalot-control

<div align="center">
  <img src="screenshots/tractor-solid.svg" width="30%">
</div>

Проект для управления Petalot через веб-интерфейс GyverHub.

## Основные функции
- Удаленное управление через Wi-Fi.
- Мониторинг температуры, скорости и других параметров.

## Зависимости
- **[GyverHub](https://github.com/GyverLibs/GyverHub)**: Веб-интерфейс для управления.
- **[Petalot](https://github.com/function3d/petalot)**: Машина для переработки пластика.
- **[NTC with ESP32](https://github.com/e-tinkers/ntc-thermistor-with-arduino-and-esp32/tree/master)**: Проект по измерениям терморезистора (прикручен алгоритм фильтрации)

## Железо/Компоненты
- **ESP32**
- **NTC3950 100KoM**
- **Резистор 100KoM**

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
- **Настройка плавности измерений (среднее арифм.)**

