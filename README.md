# Bosch BME280 Arduino
[![Compile examples folder](https://github.com/hasenradball/Bosch_BME280_Arduino/actions/workflows/compile_examples.yml/badge.svg?branch=master)](https://github.com/hasenradball/Bosch_BME280_Arduino/actions/workflows/compile_examples.yml)
[![Spell Check](https://github.com/hasenradball/Bosch_BME280_Arduino/actions/workflows/spell_checker.yml/badge.svg?branch=master)](https://github.com/hasenradball/Bosch_BME280_Arduino/actions/workflows/spell_checker.yml)
[![Generate Doxygen Documentation](https://github.com/hasenradball/Bosch_BME280_Arduino/actions/workflows/doxygen.yml/badge.svg?branch=master)](https://github.com/hasenradball/Bosch_BME280_Arduino/actions/workflows/doxygen.yml)


based on Bosch BME280_driver v3.5.1

![BME280_Module](./docs/Bosch_BME280_module.jpeg)

List of content<br>
* [About](#about)<br>
* [Library Documentation](#library-documentation)<br>
* [Functionality](#functionality)<br>
* [Namespace](#namespace)<br>
* [Methods](#methods)<br>
* [Example](#example)<br>
* [Compatibility](#compatibility)
* [Copyright](#copyright)


## About
The Bosch BME280 is an environmental sensor which is able to measure temperature, humidity and air pressure.

This library is based on the Bosch Sensortec BME280 driver API v3.5.1, and is intended to measure these environmental signals via I²C connection on an Arduino based or ESP based microcontroller.

The github repository of Bosch Sensortec is: [Github BOSCH Sensor Driver](https://github.com/BoschSensortec/BME280_driver)

The website of the BME280 on Bosch Sensortec is: [Bosch Sensortec BME280](https://www.bosch-sensortec.com/products/environmental-sensors/humidity-sensors-bme280/)

## Library Documentation
The functional documentation of this wrapper library you will find in the provided [refman.pdf](./docs/latex/refman.pdf), or under github-pages [github.io](https://hasenradball.github.io/Bosch_BME280_Arduino).


## Functionality
The original Bosch driver is included in this package and it has not been modified in any way.
The Bosch BME280 sensor do have 3 operation modes.
1. **Sleep mode** - the sensor is in sleep mode after power on reset. No measurements are performed and power consumption is on minimum.
2. **Forced mode** - one single measurement is performed and returns then to sleep mode. The measurements can be obtained from the data registers.
3. **Normal mode** - cyclic measurements are performed. The measurements can be obtained from the data registers.

## Namespace
This Bosch BME280 wrapper uses a namespace as `BME` so if you construct the object you have to call:
```
BME::Bosch_BME280 bme{BME280_I2C_ADDR_PRIM, 249.67F, false};
```

## Methods
### Public
The are the following public methods:

#### Constructor
You call the constructor with various parameters:
* address of the BME280 (0x76 or 0x77)
* altitude for the calculation of the sea level pressure
* a Bool - `true` if use forced mode or `false` if use normal mode
```
Bosch_BME280(addr, altitude, forced_mode)
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
#### Sensor Status
Also it is possible to get and set the sensor status.
```
int8_t status = getSensorStatus();
setSensorStatus(status);
```

### Example
See also in:
* [Arduino_example.ino](https://github.com/hasenradball/Bosch_BME280_Arduino/blob/master/examples/Arduino_example/Arduino_example.ino)
* [ESP32_example.ino](https://github.com/hasenradball/Bosch_BME280_Arduino/blob/master/examples/ESP32_example/ESP32_example.ino)
* [ESP8266_example.ino](https://github.com/hasenradball/Bosch_BME280_Arduino/blob/master/examples/ESP8266_example/ESP8266_example.ino)

```
#include <Arduino.h>
#include <Bosch_BME280_Arduino.h>

BME::Bosch_BME280 bme{BME280_I2C_ADDR_PRIM, 249.67F, true};

void setup() {
    Serial.begin(115200);
    while (!Serial) {
      yield();
    }

   // SDA, SCL needed for ESPs
#if defined (ESP8266)
  Wire.begin(SDA, SCL);
#elif defined (ESP32)
  Wire.setPins(SDA, SCL);
  Wire.begin();
#else
  Wire.begin();
#endif

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
Tested with:
* Arduino Nano
* ESP8266
* ESP32
* Arduino Nano 33 IOT

## Copyright
The Files of the original Bosch BME280 driver API:
* bme280.c
* bme280.h
* bme280_defs.h

are Copyright (c) 2013 - 2017 by Bosch Sensortec GmbH

[back to top](#bosch-bme280-arduino)