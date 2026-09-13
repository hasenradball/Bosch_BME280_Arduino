/**
 * @author  Frank Häfele
 * @date    25.08.2026
 * @brief   Example code for humidity sensing
 */

#include <Arduino.h>
#include <Wire.h>
#include <Bosch_BME280_Arduino.h>

constexpr Enum class oversampling { 
   BME280_NO_OVERSAMPLING = 0,
   BME280_OVERSAMPLING_1X = 1,
   BME280_OVERSAMPLING_2X = 2,
   BME280_OVERSAMPLING_4X = 4,
   BME280_OVERSAMPLING_8X = 8,
   BME280_OVERSAMPLING_16X = 16,
   BME280_OVERSAMPLING_MAX = 16
};

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
constexpr int getOversamplingFactor(uint8_t ovSetting) {
   if (ovSetting == 0x00) {
      return 0;
   }
   else if (ovSetting == BME280_OVERSAMPLING_MAX) {
      return BME280_OVERSAMPLING_MAX;
   }
   else {
      return (1 << ovSetting - 1);
   }
}

constexpr float getStandbyTime(uint8_t standbySetting) {
   if (standbySetting == BME280_STANDBY_TIME_0_5_MS) {
      return 0.5f;
   }
   else if (standbySetting == BME280_STANDBY_TIME_62_5_MS) {
      return 62.5f;
   }
   else if (standbySetting == BME280_STANDBY_TIME_125_MS) {
      return 125.0f;
   }
   else if (standbySetting == BME280_STANDBY_TIME_250_MS) {
      return 250.0f;
   }
   else if (standbySetting == BME280_STANDBY_TIME_500_MS) {
      return 500.0f;
   }
   else if (standbySetting == BME280_STANDBY_TIME_1000_MS) {
      return 1000.0f;
   }
   else if (standbySetting == BME280_STANDBY_TIME_10_MS) {
      return 10.0f;
   }
   else if (standbySetting == BME280_STANDBY_TIME_20_MS) {
      return 20.0f;
   }
}

/**
 * @brief calculate cycletime from bme oversampling and standby time
 * 
 * @param settings the bme280 settings
 * @return int pause time in ms for delay or timer
 */
constexpr int measurementCycleTime(bme280_settings& settings) {
   // see Bosch Datasheet Appendix B: page 51
   // max Measure time is defined as:
   // T_Measure_MAX =     1.25
   //                    + (2.3 * T_oversampling)
   //                    + (2.3 * P_oversamplinmg + 0.575)
   //                    + (2.3 * H_oversampling + 0.575)
   // So lets start ...
   float measureTimeT = 2.3f * getOversamplingFactor(settings.osr_t);
   float measureTimeP = 2.3f * getOversamplingFactor(settings.osr_p) + 0.575f;
   float measureTimeH = 2.3f * getOversamplingFactor(settings.osr_h) + 0.575f;
   float measureTime = 1.25f + measureTimeT + measureTimeP + measureTimeH;
   float standbyTime = getStandbyTime(settings.standby_time);

   int measurementCycle = static_cast<int>(measureTime + standbyTime + 0.5f);
   Serial.print("\n\tMeasurment cycle time max:\t");
   Serial.println(measurementCycle);
   Serial.print(" ms \n");
   return measurementCycle;
}

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