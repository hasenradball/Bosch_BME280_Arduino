#include <Arduino.h>
#include <Wire.h>
#include <Bosch_BME280_Arduino.h>

// global instance
BME::Bosch_BME280 bme{BME280_I2C_ADDR_PRIM, 249.67F, true};

void setup() {
    Serial.begin(115200);
    while (!Serial) {
      yield();
    }

   Wire.begin();
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