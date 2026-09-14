/**
 * @author  Frank Häfele
 * @date    25.08.2026
 * @brief   Example code for humidity sensing
 */

#include <Arduino.h>
#include <Wire.h>
#include <Bosch_BME280_Arduino.h>
#include <Bosch_BME280_Tools.h>

// Example for humidity sensing
// mode:         forced (less current consumption) or normal mode possible
// oversampling: X1 for temperature and humidity
// IIR-filter:   no filter
BME::Bosch_BME280 bme{BME280_I2C_ADDR_PRIM, 249.67F, true};

bme280_settings p2_humidity_sensing {
   BME280_NO_OVERSAMPLING,
   BME280_OVERSAMPLING_1X,
   BME280_OVERSAMPLING_1X,
   BME280_FILTER_COEFF_OFF,
   BME280_STANDBY_TIME_1000_MS
};

unsigned int cycleTime_ms {0};

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
   if (bme.begin(&p2_humidity_sensing) != 0) {
      Serial.println("\n\t>>> ERROR: Init of Bosch BME280 Sensor failed! <<<");
   }
   cycleTime_ms = BME280_Tools::measurementCycleTime(p2_humidity_sensing);
   Serial.print("\n\tMeasurement cycle time max:\t");
   Serial.print(cycleTime_ms);
   Serial.print(" ms \n");
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