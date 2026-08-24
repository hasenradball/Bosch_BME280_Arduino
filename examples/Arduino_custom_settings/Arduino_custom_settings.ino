#include <Arduino.h>
#include <Wire.h>
#include <Bosch_BME280_Arduino.h>

// global instance => normal mode
BME::Bosch_BME280 bme{BME280_I2C_ADDR_PRIM, 249.67F, false};

bme280_settings my_sensor_settings{
   BME280_OVERSAMPLING_1X,
   BME280_OVERSAMPLING_1X,
   BME280_OVERSAMPLING_1X,
   BME280_FILTER_COEFF_OFF,
   BME280_STANDBY_TIME_500_MS
};

void setup() {
    Serial.begin(115200);
    while (!Serial) {
      yield();
    }

   Wire.begin();
   // init Bosch BME 280 Sensor
   if (bme.begin(&my_sensor_settings) != 0) {
      Serial.println("\n\t>>> ERROR: Init of Bosch BME280 Sensor failed! <<<");
   }
}

void loop() {
    static unsigned long tic {millis()};
    unsigned long ms = millis();
    if (ms - tic >= 500) {
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