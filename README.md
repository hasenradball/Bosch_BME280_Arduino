# Bosch BME280 Arduino
based on Bosch BME280_driver v3.5.1

## About
The Bosch BME280 is an environmental sensor which is able to measure temperature, humidity and air pressure.

This library is based on the Bosch Sensortec BME280 driver API v3.5.1, and is intented to measure the these environmental signals via I²C connection on an Arduino based or ESP based microcontroller.

The github repository of Bosch Sensortec is: https://github.com/BoschSensortec/BME280_driver

The website of the BME280 on Bosch Sensortec is: https://www.bosch-sensortec.com/products/environmental-sensors/humidity-sensors-bme280/

## Functionality
The original Bosch driver is included in this package and it has not been modified in any way.
The Bosch BME280 sensor do have 3 operation modes.
1. Sleep mode - the sensor is in sleep mode after power on reset. No measurements are performed and power consumtion is on minimum.
2. Forced mode - one single measurement is performed and returns then to sleep mode. The measurements can be obtained from the data registers.
3. Normal mode - cyclic measurements are performed. The measurements can be obtained fron the data registers.

## Namespace
This Bosch BME280 Wraper uses a namespace as `BME` so if you construct the object you have to call:
```
BME::Bosch_BME280 bme{-1, -1, BME280_I2C_ADDR_PRIM, 249.67F, false};
```

## Methods
### Public
The are the following public methods:

#### Constructor
You call the constructor with various parameters:
* the SDA and SCL pin (in case of ESP variants), `-1` in case for Arduino variants
* address of the BME280 (0x76 or 0x77)
* altitude for the calculation of the sea level pressure
* a Bool - `true` if use forced mode or `false` if use normal mode
```
Bosch_BME280(sda_pin, scl_pin, addr, altitude, forced_mode)
```
#### Init I²C and Sensor Init
```
begin()
```
#### Measurement
```
measure()
```
#### Data Query
These four methods returns the temperature, humidity and pressure in float.
```
getTemperature()
getHumidity()
getPressure()
getSealevelForAltitude()
```
### Short Example
```
#include <Arduino.h>
#include <Bosch_BME280_Arduino.h>

BME::Bosch_BME280 bme{-1, -1, BME280_I2C_ADDR_PRIM, 249.67F, true};

void setup() {
    Serial.begin(115200);
    while (!Serial) {
      yield();
    }

    // init Bosch BME 280 Sensor
    if (bme.begin() != 0) {
      Serial.println("\n\t>>> ERROR: Init of Bosch BME280 Sensor failed! <<<");
    }
}

void loop() {
    static unsigned long tic {millis()};
    unsigned long ms = millis();
    if (ms - tic >= 2000) {
      tic = ms;
      bme.measure();
      Serial.print("\n\tTemperature:\t");
      Serial.println(bme.getTemperature());
      Serial.print("\tHumidity:\t");
      Serial.println(bme.getHumidity());
      Serial.print("\tPressure at NN:\t");
      Serial.println(bme.getSealevelForAltitude());
    }
}
```


## Compatibility
* Tested with Arduino Nano, ESP8266 and ESP32

## Copyright
The Files:
* bme280.c
* bme280.h
* bme280_defs.h

are Copyright (c) 2013 - 2017 by Bosch Sensortec GmbH
