/*******************************************************************************
 * @file app.c
 * @brief AHT20 Sensor Node – LCD + BLE Advertising
 *
 * CHỨC NĂNG:
 *  - Đọc cảm biến AHT20 mỗi 1 giây
 *  - Hiển thị LCD theo style project MAX30102
 *  - Quảng bá BLE (Manufacturer Data + Name)
 *
 * KHÔNG BAO GỒM:
 *  - UART
 *  - SET PERIOD command
 *  - Ping từ PC
 ******************************************************************************/

#include "em_common.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "app.h"
#include "app_log.h"

#include "dmd.h"
#include "glib.h"
#include "stdio.h"
#include "string.h"

#include "sl_sleeptimer.h"
#include "aht20.h"

// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------

static uint8_t advertising_set_handle = 0xff;

static GLIB_Context_t glibContext;

static float current_temp = 0.0f;
static float current_humi = 0.0f;

static sl_sleeptimer_timer_handle_t measure_timer;
static volatile bool need_measure = false;

// -----------------------------------------------------------------------------
// Forward declarations
// -----------------------------------------------------------------------------

static void update_advertisement_packet(void);
static void perform_measurement(void);

void my_timer_callback(sl_sleeptimer_timer_handle_t *handle, void *data);

// -----------------------------------------------------------------------------
// Timer Callback
// -----------------------------------------------------------------------------

void my_timer_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
    (void)handle;
    (void)data;
    need_measure = true;
}

// -----------------------------------------------------------------------------
// BLE Advertisement Packet Update
// -----------------------------------------------------------------------------

static void update_advertisement_packet(void)
{
    sl_status_t sc;
    uint8_t adv_data[31];
    uint8_t idx = 0;

    // FLAGS
    adv_data[idx++] = 2;
    adv_data[idx++] = 0x01;
    adv_data[idx++] = 0x06;

    // Manufacturer Data
    adv_data[idx++] = 0x0B;      // Total length
    adv_data[idx++] = 0xFF;      // Type: Manufacturer
    adv_data[idx++] = 0xFF;      // Company ID LSB (demo)
    adv_data[idx++] = 0x02;      // Company ID MSB (demo)

    memcpy(&adv_data[idx], &current_temp, sizeof(float));
    idx += 4;
    memcpy(&adv_data[idx], &current_humi, sizeof(float));
    idx += 4;

    // Complete Local Name
    uint8_t name_len = strlen(DEVICE_ID);
    adv_data[idx++] = name_len + 1;
    adv_data[idx++] = 0x09;
    memcpy(&adv_data[idx], DEVICE_ID, name_len);
    idx += name_len;

    sc = sl_bt_legacy_advertiser_set_data(advertising_set_handle,
                                          sl_bt_advertiser_advertising_data_packet,
                                          idx,
                                          adv_data);

    (void)sc;
}

// -----------------------------------------------------------------------------
// Perform One Measurement + LCD Update + BLE Update
// -----------------------------------------------------------------------------

static void perform_measurement(void)
{
    char lcd_buf[32];
    bool ok = AHT20_Read(&current_temp, &current_humi);

    GLIB_clear(&glibContext);

    if (ok)
    {
        int t_i  = (int)current_temp;
        int t_f  = (int)((current_temp - t_i) * 10);

        int h_i  = (int)current_humi;
        int h_f  = (int)((current_humi - h_i) * 10);

        // ---------------------------------------------------------
        // 3 dòng đầu tiên (theo yêu cầu)
        // ---------------------------------------------------------
        GLIB_drawStringOnLine(&glibContext,
                              "22200131",
                              1, GLIB_ALIGN_CENTER,
                              0, 0, true);

        GLIB_drawStringOnLine(&glibContext,
                              "22200133",
                              2, GLIB_ALIGN_CENTER,
                              0, 0, true);

        GLIB_drawStringOnLine(&glibContext,
                              "22200166",
                              3, GLIB_ALIGN_CENTER,
                              0, 0, true);

        // ---------------------------------------------------------
        // NAME: AHT20_1
        // ---------------------------------------------------------
        sprintf(lcd_buf, "NAME: %s", DEVICE_ID);
        GLIB_drawStringOnLine(&glibContext,
                              lcd_buf,
                              5, GLIB_ALIGN_CENTER,
                              0, 0, true);

        // ---------------------------------------------------------
        // Temperature
        // ---------------------------------------------------------
        sprintf(lcd_buf, "T: %d.%d C", t_i, t_f);
        GLIB_drawStringOnLine(&glibContext,
                              lcd_buf,
                              7, GLIB_ALIGN_LEFT,
                              5, 0, true);

        // ---------------------------------------------------------
        // Humidity
        // ---------------------------------------------------------
        sprintf(lcd_buf, "H: %d.%d %%", h_i, h_f);
        GLIB_drawStringOnLine(&glibContext,
                              lcd_buf,
                              8, GLIB_ALIGN_LEFT,
                              5, 0, true);

        // ---------------------------------------------------------
        // PERIOD (const = 1000 ms)
        // ---------------------------------------------------------
        sprintf(lcd_buf, "PERIOD: %d ms", SENSOR_READ_PERIOD_MS);
        GLIB_drawStringOnLine(&glibContext,
                              lcd_buf,
                              9, GLIB_ALIGN_LEFT,
                              5, 0, true);
    }
    else
    {
        // Nếu đọc lỗi → chỉ hiển thị lỗi ở dòng 4
        GLIB_drawStringOnLine(&glibContext,
                              "SENSOR ERROR",
                              5,
                              GLIB_ALIGN_CENTER,
                              0, 0, true);
    }

    DMD_updateDisplay();

    // Update BLE
    update_advertisement_packet();
}


// -----------------------------------------------------------------------------
// app_init()
// -----------------------------------------------------------------------------

void app_init(void)
{
    // LCD init
    DMD_init(0);
    GLIB_contextInit(&glibContext);
    glibContext.backgroundColor = White;
    glibContext.foregroundColor = Black;
    GLIB_clear(&glibContext);
    DMD_updateDisplay();

    // AHT20 init
    AHT20_Init();

    // Start 1-second measurement timer
    sl_sleeptimer_start_periodic_timer_ms(&measure_timer,
                                          SENSOR_READ_PERIOD_MS,
                                          my_timer_callback,
                                          NULL,
                                          0,
                                          0);

    app_log("AHT20 init done. PERIOD = %d ms\r\n",
            SENSOR_READ_PERIOD_MS);
}

// -----------------------------------------------------------------------------
// app_process_action()
// -----------------------------------------------------------------------------

void app_process_action(void)
{
    if (need_measure)
    {
        need_measure = false;
        perform_measurement();
    }
}

// -----------------------------------------------------------------------------
// BLE Event Handler
// -----------------------------------------------------------------------------

void sl_bt_on_event(sl_bt_msg_t *evt)
{
    sl_status_t sc;

    switch (SL_BT_MSG_ID(evt->header))
    {
        case sl_bt_evt_system_boot_id:

            sc = sl_bt_advertiser_create_set(&advertising_set_handle);
            app_assert_status(sc);

            sl_bt_advertiser_set_timing(advertising_set_handle,
                                        BLE_ADV_INTERVAL_MIN,
                                        BLE_ADV_INTERVAL_MAX,
                                        0, 0);

            sl_bt_legacy_advertiser_start(advertising_set_handle,
                                          sl_bt_advertiser_connectable_scannable);
            break;

        case sl_bt_evt_connection_closed_id:
            sl_bt_legacy_advertiser_start(advertising_set_handle,
                                          sl_bt_advertiser_connectable_scannable);
            break;

        default:
            break;
    }
}
