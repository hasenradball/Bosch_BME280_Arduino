#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <Bosch_BME280_Arduino.h>


// ============================================
// ### --- START: Klassen Instanzen --- ###
// ============================================

BME::Bosch_BME280 sensor_bme280{0, 2, BME280_I2C_ADDR_PRIM, 249.76F, true};

// ============================================
// ### --- ENDE: Klassen --- ###
// ============================================


// ============================================
// ### --- START: Setup --- ###
// ============================================
void setup() {
  WiFi.mode(WIFI_OFF);
  // Serielle Schnittstelle initialisieren
  Serial.begin(115200);
  while (!Serial) {
      yield();
    }
  Serial.println(F("\n ### >>> ESP01 Test - read Bosch BME280 Sensor Data <<< ###"));

  Serial.println(F("\t>>> init Sensor"));
  
  sensor_bme280.begin();
}

// ============================================
// ### --- ENDE: Setup --- ###
// ============================================

// ============================================
// ### --- START: Main Loop --- ###
// ============================================

void loop() {
  static unsigned long last_ms {10000};
  unsigned long ms {millis()};

  if(ms - last_ms >= 10000) {
    last_ms = ms;
    sensor_bme280.measure();

    Serial.printf("\n\n\tTemperature:\t%.2f\n", sensor_bme280.getTemperature());
    Serial.printf("\tHumidity:\t%.2f\n", sensor_bme280.getHumidity());
    Serial.printf("\tPressure:\t%.2f\n", sensor_bme280.getPressure());
    Serial.printf("\tPressure at NN:\t%.2f\n", sensor_bme280.getSealevelForAltitude());
  }
}
// ============================================
// ### --- ENDE: Main Loop --- ###
// ============================================