/**
 * @author  Frank Häfele
 * @date    25.08.2026
 * @brief   Example code for gaming
 */

#include <Arduino.h>
#include <Wire.h>
#include <Bosch_BME280_Arduino.h>

// Example for gaming
// mode:         normal mode => false
// oversampling: X4 for pressure, x1 for temperature, x0 for humidity
// IIR-filter:   filter coeff 16
// cycle:        each 500 µs
BME::Bosch_BME280 bme{BME280_I2C_ADDR_PRIM, 249.67F, false};

bme280_settings p4_gaming {
   BME280_OVERSAMPLING_4X,
   BME280_OVERSAMPLING_1X,
   BME280_NO_OVERSAMPLING,
   BME280_FILTER_COEFF_16,
   BME280_STANDBY_TIME_0_5_MS
};

constexpr unsigned int cycleTime_us {500};

void setup() {
    Serial.begin(115200);
    while (!Serial) {
      yield();
    }

   Wire.begin();
   // init Bosch BME 280 Sensor
   if (bme.begin(&p4_gaming) != 0) {
      Serial.println("\n\t>>> ERROR: Init of Bosch BME280 Sensor failed! <<<");
   }
}

void loop() {
    static unsigned long tic {micros()};
    unsigned long us = micros();
    if (us - tic >= cycleTime_us) {
      tic = us;
      bme.measure();
      Serial.print("\n\tTemperature:\t");
      Serial.println(bme.getTemperature());
      //Serial.print("\tHumidity:\t");
      //Serial.println(bme.getHumidity());
      Serial.print("\tPressure at NN:\t");
      Serial.println(bme.getSealevelForAltitude());
    }
}