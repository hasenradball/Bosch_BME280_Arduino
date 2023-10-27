#ifndef _BOSCH_BME280_ARDUINO_H_
#define _BOSCH_BME280_ARDUINO_H_
#include <Arduino.h>
#include <Wire.h>
#include "BME280_API/bme280.h"

namespace BME {
  class Bosch_BME280 {
    public:
      explicit Bosch_BME280(int8_t sda_pin = -1, int8_t scl_pin = -1, uint8_t addr = BME280_I2C_ADDR_PRIM, float altitude = 249.67F, bool forced_mode = true);
      
      int8_t begin();
      int8_t measure();
      
      float getTemperature() const {return (float) _bme280_data.temperature;}
      float getHumidity() const {return (float) _bme280_data.humidity;}
      float getPressure() const {return (float) _bme280_data.pressure / 100.0F;}
      float getSealevelForAltitude() const {return (float) _bme280_data.pressure / 100.0 / pow(1.0 - (_altitude / 44330.0), 5.255);}
      int8_t getSensorStatus() const {return _sensor_status;}
      void setSensorStatus(int8_t sensor_status);
      
    private:
      int8_t _sda_pin, _scl_pin;
      int8_t _sensor_status;
      uint8_t _addr, _mode;
      float _altitude;
      
      // variable for delay time in µs
      uint32_t _period;
      struct bme280_dev _dev;
      struct bme280_data _bme280_data;
      struct bme280_settings _settings;
      
      int8_t setSensorSettings();
      int8_t measure_normal_mode();
      int8_t measure_forced_mode();

      void bme280_print_error_codes(const char *api_name, int8_t result);

      static BME280_INTF_RET_TYPE I2CRead(uint8_t reg_addr, uint8_t *reg_data, uint32_t cnt, void *intf_ptr);
      static BME280_INTF_RET_TYPE I2CWrite(uint8_t reg_addr, const uint8_t *reg_data, uint32_t cnt, void *intf_ptr);
      static void delay_us(uint32_t period, void *intf_ptr);
  };
}
#endif
