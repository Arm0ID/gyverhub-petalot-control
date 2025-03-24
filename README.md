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

## Железо/Компоненты
- **ESP32**
- 

## Настройка Wi-Fi
Создайте файл `src/secrets.h` со следующим содержимым:

```cpp
#ifndef SECRETS_H
#define SECRETS_H

const char* WIFI_SSID = "ваш_SSID";
const char* WIFI_PASSWORD = "ваш_пароль";

#endif
```

