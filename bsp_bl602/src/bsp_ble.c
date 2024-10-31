/*
 * Copyright (c) 2024 by Lu Xianfan.
 * @FilePath     : bsp_ble.c
 * @Author       : lxf
 * @Date         : 2024-10-28 19:01:40
 * @LastEditors  : flyyingpiggy2020 154562451@qq.com
 * @LastEditTime : 2024-10-30 16:24:26
 * @Brief        : 博流蓝牙设备驱动
 */

/*---------- includes ----------*/
#include "bsp.h"
#include "ble_lib_api.h"
#include "bluetooth.h"
#include "hci_driver.h"
#include "hci_core.h"
#include "uuid.h"
#include "gatt.h"
#include "bl_sys.h"
/*---------- macro ----------*/
#define DEVICE_NAME     "BL602_LBS"
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

/** @brief LBS Service UUID. */
#define BT_UUID_LBS_VAL \
    BT_UUID_128_ENCODE(0x00001523, 0x1212, 0xefde, 0x1523, 0x785feabcd123)

/** @brief Button Characteristic UUID. */
#define BT_UUID_LBS_BUTTON_VAL \
    BT_UUID_128_ENCODE(0x00001524, 0x1212, 0xefde, 0x1523, 0x785feabcd123)

/** @brief LED Characteristic UUID. */
#define BT_UUID_LBS_LED_VAL \
    BT_UUID_128_ENCODE(0x00001525, 0x1212, 0xefde, 0x1523, 0x785feabcd123)

#define BT_UUID_LBS        BT_UUID_DECLARE_128(BT_UUID_LBS_VAL)
#define BT_UUID_LBS_BUTTON BT_UUID_DECLARE_128(BT_UUID_LBS_BUTTON_VAL)
#define BT_UUID_LBS_LED    BT_UUID_DECLARE_128(BT_UUID_LBS_LED_VAL)

/*---------- type define ----------*/
/** @brief Callback type for when an LED state change is received. */
typedef void (*led_cb_t)(const bool led_state);

/** @brief Callback type for when the button state is pulled. */
typedef bool (*button_cb_t)(void);

struct bt_lbs_cb {
    /** LED state change callback. */
    led_cb_t led_cb;
    /** Button read callback. */
    button_cb_t button_cb;
};
/*---------- variable prototype ----------*/

static bool notify_enabled;
/*---------- function prototype ----------*/
static void connected(struct bt_conn *conn, uint8_t err);
static void disconnected(struct bt_conn *conn, uint8_t reason);
static void app_led_cb(bool led_state);
static bool app_button_cb(void);
static void lbslc_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value);
static ssize_t write_led(struct bt_conn *conn,
                         const struct bt_gatt_attr *attr,
                         const void *buf,
                         uint16_t len,
                         uint16_t offset,
                         uint8_t flags);
/*---------- variable ----------*/
static struct bt_lbs_cb lbs_cb;
static struct bt_lbs_cb lbs_callbacs = {
    .led_cb = app_led_cb,
    .button_cb = app_button_cb,
};
static struct bt_conn_cb conn_callbacks = {
    .connected = connected,
    .disconnected = disconnected,
};
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static const struct bt_data sd[] = {
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, 0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x23, 0x15, 0x00, 0X00),
};

/* LED Button Service Declaration */
BT_GATT_SERVICE_DEFINE(lbs_svc,
                       BT_GATT_PRIMARY_SERVICE(BT_UUID_LBS),
                       BT_GATT_CHARACTERISTIC(BT_UUID_LBS_BUTTON,
                                              BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_READ,
                                              NULL,
                                              NULL,
                                              NULL),
                       BT_GATT_CCC(lbslc_ccc_cfg_changed,
                                   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
                       BT_GATT_CHARACTERISTIC(BT_UUID_LBS_LED,
                                              BT_GATT_CHRC_WRITE,
                                              BT_GATT_PERM_WRITE,
                                              NULL,
                                              write_led,
                                              NULL), );
/*---------- function ----------*/

int bt_lbs_init(struct bt_lbs_cb *callbacks)
{
    if (callbacks) {
        lbs_cb.led_cb = callbacks->led_cb;
        lbs_cb.button_cb = callbacks->button_cb;
    }

    return 0;
}

/**
 * @brief 蓝牙驱动初始化
 * @return {*}
 */
void bsp_InitBle(void)
{
    int err;
    // 博流设备必须调用bl_sys_init这个函数，蓝牙才可以正常工作
    bl_sys_init();

    // 这部分开始就是zephyr的蓝牙协议栈的api了
    ble_controller_init(configMAX_PRIORITIES - 1);
    hci_driver_init();
    err = bt_enable(NULL);
    if (err) {
        printk("Bluetooth init failed (err %d)\r\n", err);
        return;
    }
    printk("ble stack initialized\r\n");

    // // 这个是nordic connect sdk的api，zephyr里面没有，我把相关部分从ncs sdk中摘抄了出来
    // err = bt_lbs_init(&lbs_callbacs);
    // if (err) {
    // 	printk("Failed to init LBS (err:%d)\n", err);
    // 	return;
    // }
    // err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    // if (err) {
    // 	printk("Advertising failed to start (err %d)\n", err);
    // 	return;
    // }
    return;
}
static void app_led_cb(bool led_state)
{
    printf("led: %s\n", led_state ? "on" : "off");
}

static bool app_button_cb(void)
{
    // TODO: read button state
    return 0;
}
static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        printk("Connection failed (err %u)\n", err);
        return;
    }

    printk("Connected\n");
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    printk("Disconnected (reason %u)\n", reason);
}

static void lbslc_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    notify_enabled = (value == BT_GATT_CCC_NOTIFY);
}

static ssize_t write_led(struct bt_conn *conn,
                         const struct bt_gatt_attr *attr,
                         const void *buf,
                         uint16_t len,
                         uint16_t offset,
                         uint8_t flags)
{
    printf("Attribute write, handle: %u, conn: %p", attr->handle, (void *)conn);

    if (len != 1U) {
        printf("Write led: Incorrect data length");
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    if (offset != 0) {
        printf("Write led: Incorrect data offset");
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    if (lbs_cb.led_cb) {
        uint8_t val = *((uint8_t *)buf);

        if (val == 0x00 || val == 0x01) {
            lbs_cb.led_cb(val ? true : false);
        } else {
            printf("Write led: Incorrect value");
            return BT_GATT_ERR(BT_ATT_ERR_VALUE_NOT_ALLOWED);
        }
    }

    return len;
}

int bt_lbs_send_button_state(bool button_state)
{
    if (!notify_enabled) {
        return -1;
    }
    return bt_gatt_notify(NULL, &lbs_svc.attrs[2], &button_state, sizeof(button_state));
}
/*---------- end of file ----------*/
