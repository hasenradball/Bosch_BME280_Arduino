/*
  Projekt: Bosch BME280 Arduino Wrapper Class
  Microcontroller: ESPxx, Arduino
  Date: 21.02.2022
  Issuer: Frank Häfele
  Based on: Bosch Sensortec BME280 driver release v3.5.0
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
	sensor_status {BME280_OK},
  _sda_pin {sda_pin},
	_scl_pin {scl_pin},
	_addr {addr},
	_altitude {altitude}  
{
  // set internal _mode
  if (forced_mode) {
    _mode = BME280_FORCED_MODE;
  }
  else {
    _mode = BME280_NORMAL_MODE;
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
  dev.intf_ptr = &_addr;
  
  // I2C init START
  dev.intf = BME280_I2C_INTF;
  dev.read = &BME::Bosch_BME280::I2CRead;
  dev.write = &BME::Bosch_BME280::I2CWrite;
  dev.delay_us = &BME::Bosch_BME280::delay_us;

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
  sensor_status = bme280_init(&dev);
  // if normal mode set settings for normal mode
  setSensorSettings();
  delay(100);
  return sensor_status;
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
  int8_t ret;
  if (_mode == BME280_FORCED_MODE) {
    ret = measure_forced_mode();
  }
  else {
    ret =  measure_normal_mode();
  }
  return ret;
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
	int8_t ret = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);
  return ret;
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
  int8_t ret = BME280_OK;
  // Calculate the minimum delay in ms required between consecutive measurement based upon the sensor enabled
  // and the oversampling configuration.
  uint32_t req_delay = bme280_cal_meas_delay(&dev.settings);
  ret = bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
  // wait request_delay * 1000 us to complete the measurement
  dev.delay_us(req_delay * 1000U, dev.intf_ptr);
	ret = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);
  return ret;
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
  int8_t ret = BME280_OK;
  // Recommended settings of operation: Indoor navigation
  dev.settings.osr_h = BME280_OVERSAMPLING_1X;
  dev.settings.osr_p = BME280_OVERSAMPLING_16X;
  dev.settings.osr_t = BME280_OVERSAMPLING_2X;
  dev.settings.filter = BME280_FILTER_COEFF_16;

  if (_mode == BME280_FORCED_MODE) {
    // ### --- Forced MODE Setting --- ###
    uint8_t settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;
    ret = bme280_set_sensor_settings(settings_sel, &dev);
    ret = bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
  }
  else {
    /* ### --- NORMAL MODE Setting --- ### */
    dev.settings.standby_time = BME280_STANDBY_TIME_62_5_MS;

    uint8_t settings_sel = BME280_OSR_PRESS_SEL;
    settings_sel |= BME280_OSR_TEMP_SEL;
    settings_sel |= BME280_OSR_HUM_SEL;
    settings_sel |= BME280_STANDBY_SEL;
    settings_sel |= BME280_FILTER_SEL;
    ret = bme280_set_sensor_settings(settings_sel, &dev);
    ret = bme280_set_sensor_mode(BME280_NORMAL_MODE, &dev);
  }
  return ret;
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
int8_t BME::Bosch_BME280::I2CRead(uint8_t reg_addr, uint8_t *reg_data, uint32_t cnt, void *intf_ptr) {
  uint8_t dev_addr = *((uint8_t *)intf_ptr);
  //Serial.println("I2C_bus_read");
  int8_t ret = BME280_OK;

  //Serial.println(dev_addr, HEX);
  Wire.beginTransmission(dev_addr);
  
  //Serial.println(reg_addr, HEX);
  Wire.write(reg_addr);
  Wire.endTransmission();
  
  Wire.requestFrom((int)dev_addr, (int)cnt);
  
  uint8_t available = Wire.available();
  if(available != cnt) {
    ret = BME280_E_COMM_FAIL;
  }
  
  for(uint8_t i = 0; i < available; i++) {
    if(i < cnt) {
      *(reg_data + i) = Wire.read();
    }
    else
      Wire.read();
  }
  return ret;
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
int8_t BME::Bosch_BME280::I2CWrite(uint8_t reg_addr, const uint8_t *reg_data, uint32_t cnt, void *intf_ptr) {  
  uint8_t dev_addr = *((uint8_t *)intf_ptr);
  int8_t ret = BME280_OK;
  Wire.beginTransmission(dev_addr);
  Wire.write(reg_addr);
  Wire.write(reg_data, cnt);
  Wire.endTransmission();
  return ret;
}

/**
 * @brief User defined function for delay of micros 
 * 
 * @param period count of micro seconds
 */
void BME::Bosch_BME280::delay_us(uint32_t period, void *intf_ptr __attribute__((unused))) {
  delayMicroseconds(period);
}