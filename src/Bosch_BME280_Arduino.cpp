/**
 * @file    Bosch_BME280_Arduino.h
 * @author  Frank Häfele
 * @date    21.02.2022
 * @version 1.2.0
 * @brief   Bosch BME280 Arduino Wrapper Class based on BME280 Bosch driver v3.5.1
 */
#include <Bosch_BME280_Arduino.h>
#include <Wire.h>

BME::Bosch_BME280::Bosch_BME280(uint8_t addr, float altitude, bool forced_mode) :
   _altitude {altitude},
   _sensor_status {BME280_OK},
   _addr {addr}
{
  // set internal _mode
  if (forced_mode) {
    _mode = BME280_POWERMODE_FORCED;
  }
  else {
    _mode = BME280_POWERMODE_NORMAL;
  }
}

int8_t BME::Bosch_BME280::begin() {
  _dev.intf_ptr = &_addr;
  
  // I2C init START
  _dev.intf = BME280_I2C_INTF;
  _dev.read = &BME::Bosch_BME280::I2CRead;
  _dev.write = &BME::Bosch_BME280::I2CWrite;
  _dev.delay_us = &BME::Bosch_BME280::delay_us;

  // Init of sensor
  _sensor_status = bme280_init(&_dev);
  bme280_print_error_codes("bme280_init", _sensor_status);
  // if normal mode set settings for normal mode
  setSensorSettings();
  delay(100);
  return _sensor_status;
}

int8_t BME::Bosch_BME280::measure() {
  int8_t result;
  if (_mode == BME280_POWERMODE_FORCED) {
    result = measure_forced_mode();
  }
  else {
    result =  measure_normal_mode();
  }
  return result;
}

void BME::Bosch_BME280::setSensorStatus(int8_t sensor_status) {
  _sensor_status = sensor_status;
}

int8_t BME::Bosch_BME280::measure_normal_mode() {
	int8_t result = bme280_get_sensor_data(BME280_ALL, &_bme280_data, &_dev);
  bme280_print_error_codes("bme280_get_sensor_data", result);
  return result;
}

int8_t BME::Bosch_BME280::measure_forced_mode() {
  int8_t result {BME280_OK};
  // Calculate the minimum delay in ms required between consecutive measurement based upon the sensor enabled
  // and the oversampling configuration.
  result = bme280_cal_meas_delay(&_period, &_settings);
  bme280_print_error_codes("bme280_cal_meas_delay", result);
  //Serial.printf("\nCalculated measurement delay: %6d µs\n\n", _period);
  result = bme280_set_sensor_mode(BME280_POWERMODE_FORCED, &_dev);
  bme280_print_error_codes("bme280_set_sensor_mode", result);
  // wait request_delay in µs to complete the measurement
  _dev.delay_us(_period, _dev.intf_ptr);
	result = bme280_get_sensor_data(BME280_ALL, &_bme280_data, &_dev);
  bme280_print_error_codes("bme280_get_sensor_data", result);
  return result;
}

int8_t BME::Bosch_BME280::setSensorSettings() {
  int8_t result{BME280_OK};

  // first get all sensor settings
  result = bme280_get_sensor_settings(&_settings, &_dev);
  bme280_print_error_codes("bme280_get_sensor_settings", result);

  // Recommended settings of operation: => weather monitoring
  _settings.osr_p = BME280_OVERSAMPLING_1X;
  _settings.osr_t = BME280_OVERSAMPLING_1X;
  _settings.osr_h = BME280_OVERSAMPLING_1X;
  _settings.filter = BME280_FILTER_COEFF_OFF;

  if (_mode == BME280_POWERMODE_FORCED) {
    // ### --- Forced MODE Setting --- ###
    uint8_t settings_sel = BME280_SEL_OSR_PRESS | BME280_SEL_OSR_TEMP | BME280_SEL_OSR_HUM | BME280_SEL_FILTER;
    result = bme280_set_sensor_settings(settings_sel, &_settings, &_dev);
    bme280_print_error_codes("bme280_set_sensor_settings", result);
    result = bme280_set_sensor_mode(BME280_POWERMODE_FORCED, &_dev);
    bme280_print_error_codes("bme280_set_sensor_mode", result);
  }
  else {
    /* ### --- NORMAL MODE Setting --- ### */
    _settings.standby_time = BME280_STANDBY_TIME_1000_MS;

    uint8_t settings_sel = BME280_SEL_OSR_PRESS;
    settings_sel |= BME280_SEL_OSR_TEMP;
    settings_sel |= BME280_SEL_OSR_HUM;
    settings_sel |= BME280_SEL_STANDBY;
    settings_sel |= BME280_SEL_FILTER;
    result = bme280_set_sensor_settings(settings_sel, &_settings, &_dev);
    bme280_print_error_codes("bme280_set_sensor_settings", result);
    result = bme280_set_sensor_mode(BME280_POWERMODE_NORMAL, &_dev);
    bme280_print_error_codes("bme280_set_sensor_mode", result);
  }
  return result;
}


 void BME::Bosch_BME280::bme280_print_error_codes(const char *api_name, int8_t result) {
  if (result != BME280_OK) {
    Serial.print(api_name);
    Serial.print("\tError [");
    switch (result)
    {
      case BME280_E_NULL_PTR:
          Serial.print(result);
          Serial.print("] : Null pointer error.\n");
          Serial.print("\t\t=> It occurs when the user tries to assign value (not address) to a pointer, which has been initialized to NULL.\r\n\n");
          break;

      case BME280_E_COMM_FAIL:
          Serial.print(result);
          Serial.print("] : Communication failure error.\n");
          Serial.print("\t\t=> It occurs due to read/write operation failure and also due to power failure during communication\r\n\n");
          break;

      case BME280_E_DEV_NOT_FOUND:
          Serial.print(result);
          Serial.print("] : Device not found error.\n");
          Serial.print("\t\t=> It occurs when the device chip id is incorrectly read\r\n\n");
          break;

      case BME280_E_INVALID_LEN:
          Serial.print(result);
          Serial.print("] : Invalid length error.\n");
          Serial.print("\t\t=> It occurs when write is done with invalid length\r\n\n");
          break;

      default:
          Serial.print(result);
          Serial.print("] : Unknown error code\r\n\n");
          break;
    }
  }
 }

BME280_INTF_RET_TYPE BME::Bosch_BME280::I2CRead(uint8_t reg_addr, uint8_t *reg_data, uint32_t cnt, void *intf_ptr) {
  uint8_t dev_addr = *((uint8_t *)intf_ptr);
  //Serial.println("I2C_bus_read");
  int8_t result {BME280_OK};

  //Serial.println(dev_addr, HEX);
  Wire.beginTransmission(dev_addr);
  
  //Serial.println(reg_addr, HEX);
  Wire.write(reg_addr);
  Wire.endTransmission();
  
  Wire.requestFrom((int)dev_addr, (int)cnt);
  
  uint8_t available = Wire.available();
  if(available != cnt) {
    result = BME280_E_COMM_FAIL;
  }
  
  for(uint8_t i = 0; i < available; i++) {
    if(i < cnt) {
      *(reg_data + i) = Wire.read();
    }
    else
      Wire.read();
  }
  return result;
}

BME280_INTF_RET_TYPE BME::Bosch_BME280::I2CWrite(uint8_t reg_addr, const uint8_t *reg_data, uint32_t cnt, void *intf_ptr) {  
  uint8_t dev_addr = *((uint8_t *)intf_ptr);
  int8_t result {BME280_OK};
  Wire.beginTransmission(dev_addr);
  Wire.write(reg_addr);
  Wire.write(reg_data, cnt);
  Wire.endTransmission();
  return result;
}

void BME::Bosch_BME280::delay_us(uint32_t period, void *intf_ptr __attribute__((unused))) {
  delayMicroseconds(period);
}