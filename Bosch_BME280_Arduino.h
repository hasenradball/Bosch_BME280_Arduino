#ifndef _BOSCH_BME280_ARDUINO_H_
#define _BOSCH_BME280_ARDUINO_H_
#include <Arduino.h>
#include <Wire.h>
#include "BME280_API/bme280.h"

namespace BME {
  class Bosch_BME280 {
    public:
      int8_t sensor_status;

      explicit Bosch_BME280(int8_t sda_pin = -1, int8_t scl_pin = -1, uint8_t addr = BME280_I2C_ADDR_PRIM, float altitude = 249.67F, bool forced_mode = true);
      int8_t begin(void);
      int8_t measure();
      
      float getTemperature() const {return (float) comp_data.temperature;};
      float getHumidity() const {return (float) comp_data.humidity;};
      float getPressure() const {return (float) comp_data.pressure / 100.0F;};
      float getSealevelForAltitude() const {return (float) comp_data.pressure / 100.0 / pow(1.0 - (_altitude / 44330.0), 5.255);};
      
    private:
      int8_t _sda_pin, _scl_pin;
      uint8_t _addr, _mode;
      float _altitude;
      
      struct bme280_dev dev;
      struct bme280_data comp_data;
      
      int8_t setSensorSettings();
      int8_t measure_normal_mode();
      int8_t measure_forced_mode();

      static int8_t I2CRead(uint8_t reg_addr, uint8_t *reg_data, uint32_t cnt, void *intf_ptr);
      static int8_t I2CWrite(uint8_t reg_addr, const uint8_t *reg_data, uint32_t cnt, void *intf_ptr);
      static void delay_us(uint32_t period, void *intf_ptr);
  };
}
#endif
