#pragma once


// Стандартные библиотеки
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

// Библиотеки Arduino и ESP32
#include "pins_arduino.h"
#include "io_pin_remap.h"
#include "HardwareSerial.h"
#include "soc/soc_caps.h"
#include "driver/uart.h"
#include "freertos/queue.h"

// Пользовательские заголовочные файлы
#include "settings.h"    // Настройки проекта
#include "median.h"
#include "thermosense.h" // Работа с термоизмерениями
#include "GyverPID.h"    // PID-регулятор
#include "GyverStepper2.h" // Управление шаговым двигателем

// Основная библиотека GyverHub
#include <GyverHub.h>
