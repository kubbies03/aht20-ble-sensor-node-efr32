/*******************************************************************************
 * @file app.h
 * @brief Application configuration for AHT20 sensor node
 *
 * This version is simplified for:
 *  - AHT20 sensor reading
 *  - LCD display
 *  - BLE advertising
 *
 * NO UART, NO COMMAND SET PERIOD
 ******************************************************************************/

#ifndef APP_H
#define APP_H

// -----------------------------------------------------------------------------
// Device Configuration
// -----------------------------------------------------------------------------

// Tên device hiển thị trên LCD & BLE (giống style project MAX30102)
#define DEVICE_ID        "AHT20_1"

// -----------------------------------------------------------------------------
// Timing Configuration
// -----------------------------------------------------------------------------

// Period đo cảm biến cố định = 1000 ms
#define SENSOR_READ_PERIOD_MS     1000

// BLE advertising interval = 1000 ms = 1600 * 0.625
#define BLE_ADV_INTERVAL_MIN      1600
#define BLE_ADV_INTERVAL_MAX      1600

// -----------------------------------------------------------------------------
// Function Prototypes
// -----------------------------------------------------------------------------

void app_init(void);
void app_process_action(void);

#endif // APP_H
