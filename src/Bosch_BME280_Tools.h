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
      return ovSetting == 0x00 ? 0 : (1 << (ovSetting - 1));
   }

   /**
    * @brief Get the Standby Time in miiliseconds for the Standby Setting
    * 
    * @param standbySetting BME280_STANDBY_TIME setting
    * @return constexpr float standby time in milliseconds
    */
   inline constexpr float getStandbyTime(uint8_t standbySetting) {
      return standbySetting == BME280_STANDBY_TIME_0_5_MS   ? 0.5f :
             standbySetting == BME280_STANDBY_TIME_62_5_MS  ? 62.5f :
             standbySetting == BME280_STANDBY_TIME_125_MS   ? 125.0f :
             standbySetting == BME280_STANDBY_TIME_250_MS   ? 250.0f :
             standbySetting == BME280_STANDBY_TIME_500_MS   ? 500.0f :
             standbySetting == BME280_STANDBY_TIME_1000_MS  ? 1000.0f :
             standbySetting == BME280_STANDBY_TIME_10_MS    ? 10.0f :
             standbySetting == BME280_STANDBY_TIME_20_MS    ? 20.0f :
             0.0f;
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
      float measureTime = 1.25f;
      measureTime += 2.3f * getOversamplingFactor(settings.osr_t);
      measureTime += 2.3f * getOversamplingFactor(settings.osr_p) + 0.575f;
      measureTime += 2.3f * getOversamplingFactor(settings.osr_h) + 0.575f;
      float standbyTime = getStandbyTime(settings.standby_time);

      unsigned int measurementCycle = static_cast<unsigned int>(measureTime + standbyTime + 0.5f);
      return measurementCycle;
   }
}
#endif