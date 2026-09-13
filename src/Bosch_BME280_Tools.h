/**
 * @file    Bosch_BME280_Tools.h
 * @author  Frank Häfele
 * @date    13.09.2026
 * @brief   Tools for Bosch BME280 Arduino Wrapper Class based on BME280 Bosch driver v3.5.1
 */
#ifndef _BOSCH_BME280_Tools_H_
#define _BOSCH_BME280_Tools_H_

#include "BME280_API/bme280_defs.h"

namespace BME280_Tools {
   
   /**
    * @brief Get the Oversampling Factor for Oversampling Setting
    * 
    * @param ovSetting BME280_Oversampling setting 
    * @return constexpr int 
    */
   inline constexpr int getOversamplingFactor(uint8_t ovSetting) {
      if (ovSetting == 0x00) {
         return 0;
      }
      else if (ovSetting == BME280_OVERSAMPLING_MAX) {
         return BME280_OVERSAMPLING_MAX;
      }
      else {
         return (1 << (ovSetting - 1));
      }
   }

   /**
    * @brief Get the Standby Time in miiliseconds for the Standby Setting
    * 
    * @param standbySetting BME280_STANDBY_TIME setting
    * @return constexpr float standby time in milliseconds
    */
   inline constexpr float getStandbyTime(uint8_t standbySetting) {
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
      return 0.0f;
   }

   /**
    * @brief calculate cycle time from BME280 oversampling and standby time settings
    * 
    * @param settings the bme280 settings
    * @return int pause time in ms for delay or timer
    */
   unsigned int measurementCycleTime(bme280_settings& settings) {
      // see Bosch Datasheet Appendix B: page 51
      // max Measure time is defined as:
      // T_Measure_MAX =     1.25
      //                    + (2.3 * T_oversampling)
      //                    + (2.3 * P_oversamplinmg + 0.575)
      //                    + (2.3 * H_oversampling + 0.575)
      float measureTimeT = 2.3f * getOversamplingFactor(settings.osr_t);
      float measureTimeP = 2.3f * getOversamplingFactor(settings.osr_p) + 0.575f;
      float measureTimeH = 2.3f * getOversamplingFactor(settings.osr_h) + 0.575f;
      float measureTime = 1.25f + measureTimeT + measureTimeP + measureTimeH;
      float standbyTime = getStandbyTime(settings.standby_time);

      unsigned int measurementCycle = static_cast<unsigned int>(measureTime + standbyTime + 0.5f);
      return measurementCycle;
   }

}
#endif