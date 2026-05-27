/***************************************************************************//**
 * @file aht20.c
 * @brief AHT20 Temperature and Humidity Sensor Driver
 * 
 * This driver provides I2C communication interface for AHT20 sensor.
 * Supports temperature and humidity measurement with high accuracy.
 * 
 * @author Team 1
 * @date November 30, 2025
 * 
 * Sensor Specifications:
 * - I2C Address: 0x38
 * - Temperature Range: -40°C to 85°C
 * - Humidity Range: 0% to 100% RH
 * - Accuracy: ±0.3°C, ±2% RH
 ******************************************************************************/

#include "aht20.h"
#include "sl_i2cspm_instances.h"  // Simplicity Studio I2C driver
#include "sl_sleeptimer.h"        // For delay functions

/**
 * @brief AHT20 I2C Address (7-bit address shifted left by 1)
 */
#define AHT20_I2C_ADDRESS (0x38 << 1)

/**************************************************************************//**
 * @brief Initialize AHT20 sensor
 * 
 * Sends initialization/calibration command to AHT20 sensor.
 * Must be called before any measurement operations.
 * 
 * Initialization sequence:
 * 1. Wait 100ms for sensor power-up
 * 2. Send initialization command (0xBE 0x08 0x00)
 * 3. Wait 50ms for sensor to process
 * 
 * @note I2C peripheral must be initialized before calling this function
 *****************************************************************************/
void AHT20_Init(void) {
    sl_sleeptimer_delay_millisecond(100);  // Wait for sensor power-up

    // Send initialization command
    uint8_t init_cmd[] = {0xBE, 0x08, 0x00};
    I2C_TransferSeq_TypeDef seq;

    seq.addr = AHT20_I2C_ADDRESS;
    seq.flags = I2C_FLAG_WRITE;
    seq.buf[0].data = init_cmd;
    seq.buf[0].len = 3;

    I2CSPM_Transfer(sl_i2cspm_sensor, &seq);

    sl_sleeptimer_delay_millisecond(50);  // Wait for sensor to process command
}

/**************************************************************************//**
 * @brief Read temperature and humidity from AHT20
 * 
 * Triggers a measurement and reads back the sensor data.
 * 
 * Measurement sequence:
 * 1. Send trigger measurement command (0xAC 0x33 0x00)
 * 2. Wait 80ms for measurement to complete
 * 3. Read 6 bytes of data
 * 4. Parse and convert raw data to physical values
 * 
 * Data format (6 bytes):
 * - Byte 0: Status
 * - Bytes 1-3: Humidity (20-bit)
 * - Bytes 3-5: Temperature (20-bit)
 * 
 * @param[out] temp Pointer to store temperature value in Celsius
 * @param[out] humi Pointer to store humidity value in percentage
 * 
 * @return true if measurement successful, false if I2C communication failed
 * 
 * @note Temperature formula: T = (raw * 200 / 2^20) - 50
 * @note Humidity formula: RH = (raw * 100 / 2^20)
 *****************************************************************************/
bool AHT20_Read(float *temp, float *humi) {
    I2C_TransferSeq_TypeDef seq;
    uint8_t trigger_cmd[] = {0xAC, 0x33, 0x00};  // Trigger measurement command
    uint8_t read_data[6];                         // Buffer for sensor data

    // 1. Send trigger measurement command
    seq.addr = AHT20_I2C_ADDRESS;
    seq.flags = I2C_FLAG_WRITE;
    seq.buf[0].data = trigger_cmd;
    seq.buf[0].len = 3;

    if (I2CSPM_Transfer(sl_i2cspm_sensor, &seq) != i2cTransferDone) {
        return false;
    }

    // 2. Wait for measurement to complete (~80ms)
    sl_sleeptimer_delay_millisecond(80);

    // 3. Read 6 bytes of data
    seq.flags = I2C_FLAG_READ;
    seq.buf[0].data = read_data;
    seq.buf[0].len = 6;

    if (I2CSPM_Transfer(sl_i2cspm_sensor, &seq) != i2cTransferDone) {
        return false;
    }

    // 4. Parse data according to AHT20 datasheet
    // Humidity: 20-bit value in bytes 1-3
    uint32_t raw_humi = ((uint32_t)read_data[1] << 12) | 
                        ((uint32_t)read_data[2] << 4) | 
                        (read_data[3] >> 4);
    
    // Temperature: 20-bit value in bytes 3-5
    uint32_t raw_temp = (((uint32_t)read_data[3] & 0x0F) << 16) | 
                        ((uint32_t)read_data[4] << 8) | 
                        read_data[5];

    // Convert to physical values
    *humi = ((float)raw_humi * 100.0f) / 1048576.0f;           // RH%
    *temp = (((float)raw_temp * 200.0f) / 1048576.0f) - 50.0f; // °C

    return true;
}
