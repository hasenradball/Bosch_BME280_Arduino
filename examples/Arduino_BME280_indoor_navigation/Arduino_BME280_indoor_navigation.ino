/**
 * @author  Frank Häfele
 * @date    25.08.2026
 * @brief   Example code for indoor navigation
 */

#include <Arduino.h>
#include <Wire.h>
#include <Bosch_BME280_Arduino.h>
#include <Bosch_BME280_Tools.h>

// Example for indoor navigation
// mode:         normal mode => false
// oversampling: X16 for pressure, x2 for temperature, x1 for humidity
// IIR-filter:   filter coeff 16
BME::Bosch_BME280 bme{BME280_I2C_ADDR_PRIM, 249.67F, false};

bme280_settings p3_indoor_navigation {
   BME280_OVERSAMPLING_16X,
   BME280_OVERSAMPLING_2X,
   BME280_OVERSAMPLING_1X,
   BME280_FILTER_COEFF_16,
   BME280_STANDBY_TIME_0_5_MS
};

unsigned int cycleTime_ms {0};

void setup() {
    Serial.begin(115200);
    while (!Serial) {
      yield();
    }

#if defined (__AVR__)
   Wire.begin();
#elif defined (ESP8266)
   Wire.begin(0, 2);
#endif
   // init Bosch BME 280 Sensor
   if (bme.begin(&p3_indoor_navigation) != 0) {
      Serial.println("\n\t>>> ERROR: Init of Bosch BME280 Sensor failed! <<<");
   }
   cycleTime_ms = BME280_Tools::measurementCycleTime(p3_indoor_navigation);
   Serial.print("\n\tMeasurement cycle time max:\t");
   Serial.print(cycleTime_ms);
   Serial.print(" ms \n");
}

void loop() {
    static unsigned long tic {millis() - cycleTime_ms};
    unsigned long ms = millis();
    if (ms - tic >= cycleTime_ms) {
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