/***************************************************************************//**
 * @file aht20.h
 * @brief AHT20 Temperature and Humidity Sensor Driver Header
 * 
 * This header file declares functions for interfacing with AHT20 sensor
 * via I2C communication protocol.
 * 
 * @author Team 1
 * @date November 30, 2025
 * 
 * Sensor Information:
 * - Manufacturer: Aosong Electronics
 * - Interface: I2C
 * - Address: 0x38
 * - Supply Voltage: 2.0V - 5.5V
 ******************************************************************************/

#ifndef AHT20_H_
#define AHT20_H_

#include <stdint.h>
#include <stdbool.h>

/**************************************************************************//**
 * @brief Initialize AHT20 sensor
 * 
 * Sends initialization/calibration command to the sensor.
 * Must be called once at system startup before any measurements.
 * 
 * @note Requires I2C peripheral to be initialized first
 * @note Takes approximately 150ms to complete
 *****************************************************************************/
void AHT20_Init(void);

/**************************************************************************//**
 * @brief Read temperature and humidity from AHT20
 * 
 * Triggers a new measurement and retrieves the sensor data.
 * 
 * @param[out] temp Pointer to store temperature value in Celsius
 * @param[out] humi Pointer to store humidity value in percentage (0-100%)
 * 
 * @return true if measurement successful, false if I2C error occurred
 * 
 * @note Measurement takes approximately 80ms
 * @note Temperature range: -40°C to +85°C
 * @note Humidity range: 0% to 100% RH
 * @note Accuracy: ±0.3°C (temperature), ±2% RH (humidity)
 * 
 * @code
 * float temperature, humidity;
 * if (AHT20_Read(&temperature, &humidity)) {
 *     // Use temperature and humidity values
 * }
 * @endcode
 *****************************************************************************/
bool AHT20_Read(float *temp, float *humi);

#endif /* AHT20_H_ */
