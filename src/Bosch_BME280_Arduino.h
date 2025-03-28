/**
 * @file    Bosch_BME280_Arduino.h
 * @author  Frank Häfele
 * @date    21.02.2022
 * @version 1.2.0
 * @brief   Bosch BME280 Arduino Wrapper Class based on BME280 Bosch driver v3.5.1
 */
#ifndef _BOSCH_BME280_ARDUINO_H_
#define _BOSCH_BME280_ARDUINO_H_
#include <Arduino.h>
#include "BME280_API/bme280.h"

namespace BME {
  class Bosch_BME280 {
    public:
      /**
       * @brief Construct a new bme::Bosch_BME280 Object
       * 
       * @param addr I²C-Address for sensor (0x76 default)
       * @param altitude Altitude for the calculation of the Air Pressure at NN
       * @param forced_mode if true the sensor makes one measurement and goes to sleep (no continuous measurement)
       */
      explicit Bosch_BME280(uint8_t addr = BME280_I2C_ADDR_PRIM, float altitude = 249.67F, bool forced_mode = true);
      
      /**
       * @brief setup the I2C Wiring and init the Sensor
       * 
       * @return sensor status
       *
       * @retval   0: Success
       * @retval  >0: Warning
       * @retval  <0: Fail
       */
      int8_t begin();

      /**
       * @brief measure function
       * 
       * @return sensor status
       *
       * @retval   0: Success
       * @retval  >0: Warning
       * @retval  <0: Fail
       */
      int8_t measure();
      
      /**
       * @brief Get the temperature from the internal BME data object
       * 
       * @return temperature in degree celsius
       */
      float getTemperature() const {return (float) _bme280_data.temperature;}

      /**
       * @brief Get the Humidity from the internal BME data object
       * 
       * @return humidity in %
       */
      float getHumidity() const {return (float) _bme280_data.humidity;}
      
      /**
       * @brief Get the air pressure from the internal BME data object
       * 
       * @return air pressure in hecto pascal (hPa)
       */
      float getPressure() const {return (float) _bme280_data.pressure / 100.0F;}
      
      /**
       * @brief Get the Sealevel For Altitude from the internal BME data object
       * 
       * @return sea level for altitude in meter
       */
      float getSealevelForAltitude() const {return (float) _bme280_data.pressure / 100.0 / pow(1.0 - (_altitude / 44330.0), 5.255);}
      
      /**
       * @brief Get the sensor status 
       * 
       * @return sensor status
       *
       * @retval   0: Success
       * @retval  >0: Warning
       * @retval  <0: Fail
       */
      int8_t getSensorStatus() const {return _sensor_status;}

      /**
       * @brief set sensor status
       * 
       * @param sensor_status 
       */
      void setSensorStatus(int8_t sensor_status);
      
    private:
      /**
       * @brief BME280 device structure (internal)
       * 
       */
      struct bme280_dev _dev;

      /**
       * @brief BME280 data structure (internal)
       * 
       * holds the measured values
       * 
       */
      struct bme280_data _bme280_data;

      /**
       * @brief BME280 settings (internal)
       * 
       */
      struct bme280_settings _settings;
      
      /**
       * @brief member for delay time in µs
       * 
       */
      uint32_t _period;
      
      /**
       * @brief internal member for altitude
       * 
       */
      float _altitude;

      /**
       * @brief internal member for sensor status
       * 
       */
      int8_t _sensor_status;

      // internal members for adress and mode
      uint8_t _addr, _mode;

      /**
       * @brief set sensor settings for forced or normal mode of BME280
       * 
       * @return sensor status
       * 
       * @retval   0: Success
       * @retval  >0: Warning
       * @retval  <0: Fail
       */
      int8_t setSensorSettings();
      
      /**
       * @brief measurement in normal mode
       * 
       * @return sensor status
       *
       * @retval   0: Success
       * @retval  >0: Warning
       * @retval  <0: Fail
     */
      int8_t measure_normal_mode();

      /**
       * @brief measure in forced mode. In forced mode the sensor takes one measurement and then goes to sleep
       * 
       * @return sensor status
       *
       * @retval   0: Success
       * @retval  >0: Warning
       * @retval  <0: Fail
       */
      int8_t measure_forced_mode();

      /**
       * @brief print the bme280 specific error codes
       * 
       * @param api_name name of api
       * @param result code or result
       */
      void bme280_print_error_codes(const char *api_name, int8_t result);

      /**
       * @brief User defined function for I2C Read
       * 
       * @param reg_addr Register Address
       * @param reg_data Register Data
       * @param cnt count of Bytes
       * @param intf_ptr Pointer of user defined function
       * 
       * @return sensor communication status
       *
       * @retval   0: Success
       * @retval  >0: Warning
       * @retval  <0: Fail
       */
      static BME280_INTF_RET_TYPE I2CRead(uint8_t reg_addr, uint8_t *reg_data, uint32_t cnt, void *intf_ptr);

      /**
       * @brief User defined function for I2C Write
       * 
       * @param reg_addr Register Address
       * @param reg_data Register Data
       * @param cnt count of Bytes
       * @param intf_ptr Pointer of user defined function
       *
       * @return sensor communication status
       *
       * @retval   0: Success.
       * @retval  >0: Warning.
       * @retval  <0: Fail.
       */
      static BME280_INTF_RET_TYPE I2CWrite(uint8_t reg_addr, const uint8_t *reg_data, uint32_t cnt, void *intf_ptr);

      /**
       * @brief User defined function for delay of micros 
       * 
       * @param period count of micro seconds
       */
      static void delay_us(uint32_t period, void *intf_ptr);
  };
}
#endif
