#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "pins_arduino.h"
#include "io_pin_remap.h"
#include "HardwareSerial.h"
#include "soc/soc_caps.h"
#include "driver/uart.h"
#include "freertos/queue.h"
#include "settings.h"
#include "median.h"

void thermosenseSetup();
double thermosenseMeasurment();