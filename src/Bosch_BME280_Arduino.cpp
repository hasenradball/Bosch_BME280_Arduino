/*
  Projekt: Bosch BME280 Arduino Wrapper Class
  Microcontroller: ESPxx, Arduino
  Date: 21.02.2022
  Issuer: Frank Häfele
  Based on: Bosch Sensortec BME280 driver release v3.5.1
*/
#include <Bosch_BME280_Arduino.h>

/**
 * @brief Construct a new bme::Bosch_BME280 Object
 * 
 * @param sda_pin Pin for SDA Signal (-1 for Arduino usage)
 * @param scl_pin Pin for SCL Signal (-1 for Arduino usage)
 * @param addr I²C-Address for sensor (0x76 default)
 * @param altitude Altitude for the calculation of the Air Pressure at NN
 * @param forced_mode if true the sensor makes one measurement and goes to sleep (no continous measurement)
 */
BME::Bosch_BME280::Bosch_BME280(int8_t sda_pin, int8_t scl_pin, uint8_t addr, float altitude, bool forced_mode) :
  _sda_pin {sda_pin},
	_scl_pin {scl_pin},
  _sensor_status {BME280_OK},
	_addr {addr},
	_altitude {altitude}  
{
  // set internal _mode
  if (forced_mode) {
    _mode = BME280_POWERMODE_FORCED;
  }
  else {
    _mode = BME280_POWERMODE_NORMAL;
  }
}

/**
 * @brief setup the I2C Wiring and init the Sensor
 * 
 * @return sensor status
 *
 * @retval   0 -> Success.
 * @retval > 0 -> Warning.
 * @retval < 0 -> Fail.
 */
int8_t BME::Bosch_BME280::begin() {
  _dev.intf_ptr = &_addr;
  
  // I2C init START
  _dev.intf = BME280_I2C_INTF;
  _dev.read = &BME::Bosch_BME280::I2CRead;
  _dev.write = &BME::Bosch_BME280::I2CWrite;
  _dev.delay_us = &BME::Bosch_BME280::delay_us;

  // SDA, SCL needed for ESPs
#if defined (ESP8266)
  Wire.begin(_sda_pin, _scl_pin);
#elif defined (ESP32)
  Wire.setPins(_sda_pin, _scl_pin);
  Wire.begin();
#else
  Wire.begin();
#endif
  // I2C init END
  // Init of sensor
  _sensor_status = bme280_init(&_dev);
  bme280_print_error_codes("bme280_init", _sensor_status);
  // if normal mode set settings for normal mode
  setSensorSettings();
  delay(100);
  return _sensor_status;
}

/**
 * @brief measure function
 * 
 * @return sensor status
 *
 * @retval   0 -> Success.
 * @retval > 0 -> Warning.
 * @retval < 0 -> Fail.
 */
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

/**
 * @brief set sensor status
 * 
 * @param sensor_status 
 */
void BME::Bosch_BME280::setSensorStatus(int8_t sensor_status) {
  _sensor_status = sensor_status;
}

/**
 * @brief measurement in normal mode
 * 
 * @return sensor status
 *
 * @retval   0 -> Success.
 * @retval > 0 -> Warning.
 * @retval < 0 -> Fail.
 */
int8_t BME::Bosch_BME280::measure_normal_mode() {
	int8_t result = bme280_get_sensor_data(BME280_ALL, &_bme280_data, &_dev);
  bme280_print_error_codes("bme280_get_sensor_data", result);
  return result;
}

/**
 * @brief measure in forced mode. In forced mode the sesnor takes one measurement and then goes to sleep
 * 
 * @return sensor status
 *
 * @retval   0 -> Success.
 * @retval > 0 -> Warning.
 * @retval < 0 -> Fail.
 */
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

/**
 * @brief set Sensor settings for forced or normal mode of BME280
 * 
 * @return sensor status
 * 
 * @retval   0 -> Success.
 * @retval > 0 -> Warning.
 * @retval < 0 -> Fail.
 */
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


/**
 * @brief print the bme280 specific error codes
 * 
 * @param api_name name of api
 * @param result code or result
 */
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

/**
 * @brief User defined function for I2C Read
 * 
 * @param reg_addr Register Address
 * @param reg_data Register Data
 * @param cnt count of Bytes
 * @param intf_ptr Pointer of user defined function
 * 
 * @return sensor communcation status
 *
 * @retval  0 -> Success.
 * @retval > 0 -> Warning.
 * @retval < 0 -> Fail.
 */
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

/**
 * @brief User defined function for I2C Write
 * 
 * @param reg_addr Register Address
 * @param reg_data Register Data
 * @param cnt count of Bytes
 * @param intf_ptr Pointer of user defined function
 *
 * @return sensor communcation status
 *
 * @retval   0 -> Success.
 * @retval > 0 -> Warning.
 * @retval < 0 -> Fail.
 */
BME280_INTF_RET_TYPE BME::Bosch_BME280::I2CWrite(uint8_t reg_addr, const uint8_t *reg_data, uint32_t cnt, void *intf_ptr) {  
  uint8_t dev_addr = *((uint8_t *)intf_ptr);
  int8_t result {BME280_OK};
  Wire.beginTransmission(dev_addr);
  Wire.write(reg_addr);
  Wire.write(reg_data, cnt);
  Wire.endTransmission();
  return result;
}

/**
 * @brief User defined function for delay of micros 
 * 
 * @param period count of micro seconds
 */
void BME::Bosch_BME280::delay_us(uint32_t period, void *intf_ptr __attribute__((unused))) {
  delayMicroseconds(period);
}