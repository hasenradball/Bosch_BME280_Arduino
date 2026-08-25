/**
 * @author  Frank Häfele
 * @date    25.08.2026
 * @brief   Example code for humidity sensing
 */

#include <Arduino.h>
#include <Wire.h>
#include <Bosch_BME280_Arduino.h>

// Example for humidity sensing
// mode:         forced (less current consumption) or normal mode possible
// oversampling: X1 for temperature and humidity
// IIR-filter:   no filter
// cycle:        each 1 s
BME::Bosch_BME280 bme{BME280_I2C_ADDR_PRIM, 249.67F, true};

bme280_settings p2_humidity_sensing {
   BME280_NO_OVERSAMPLING,
   BME280_OVERSAMPLING_1X,
   BME280_OVERSAMPLING_1X,
   BME280_FILTER_COEFF_OFF,
   BME280_STANDBY_TIME_1000_MS
};

constexpr unsigned int cycleTime_ms {1000};

void setup() {
    Serial.begin(115200);
    while (!Serial) {
      yield();
    }

   Wire.begin();
   // init Bosch BME 280 Sensor
   if (bme.begin(&p2_humidity_sensing) != 0) {
      Serial.println("\n\t>>> ERROR: Init of Bosch BME280 Sensor failed! <<<");
   }
}

void loop() {
    static unsigned long tic {millis()};
    unsigned long ms = millis();
    if (ms - tic >= cycleTime_ms) {
      tic = ms;
      bme.measure();
      Serial.print("\n\tTemperature:\t");
      Serial.println(bme.getTemperature());
      Serial.print("\tHumidity:\t");
      Serial.println(bme.getHumidity());
      //Serial.print("\tPressure at NN:\t");
      //Serial.println(bme.getSealevelForAltitude());
    }
}