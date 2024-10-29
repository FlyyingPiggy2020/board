/*
 * Copyright (c) 2024 by Lu Xianfan.
 * @FilePath     : bsp_ble.c
 * @Author       : lxf
 * @Date         : 2024-10-28 19:01:40
 * @LastEditors  : flyyingpiggy2020 154562451@qq.com
 * @LastEditTime : 2024-10-29 09:29:10
 * @Brief        : 博流蓝牙设备驱动
 * 
 */


/*---------- includes ----------*/
#include "bsp.h"
#include "ble_lib_api.h"
#include "bluetooth.h"
#include "hci_driver.h"
#include "hci_core.h"
#include "bl_sys.h"
/*---------- macro ----------*/

#define DEVICE_NAME "BL602_MG_TEST"
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

#define BT_LE_ADV_NCONN_IDENTITY BT_LE_ADV_PARAM(BT_LE_ADV_OPT_USE_IDENTITY, \
						 BT_GAP_ADV_FAST_INT_MIN_2, \
						 BT_GAP_ADV_FAST_INT_MAX_2)
/*---------- type define ----------*/
/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/

void bt_enable_cb(int err);
/*---------- variable ----------*/

/*
 * Set Advertisement data. Based on the Eddystone specification:
 * https://github.com/google/eddystone/blob/master/protocol-specification.md
 * https://github.com/google/eddystone/tree/master/eddystone-url
 */
static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0xaa, 0xfe),
	BT_DATA_BYTES(BT_DATA_SVC_DATA16,
		      0xaa, 0xfe, /* Eddystone UUID */
		      0x10, /* Eddystone-URL frame type */
		      0x00, /* Calibrated Tx power at 0m */
		      0x00, /* URL Scheme Prefix http://www. */
		      'z', 'e', 'p', 'h', 'y', 'r',
		      'p', 'r', 'o', 'j', 'e', 'c', 't',
		      0x08) /* .org */
};

/* Set Scan Response data */
static const struct bt_data sd[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

/*---------- function ----------*/
void bsp_InitBle(void)
{
    /* ble controller 初始化*/
	// 博流设备必须调用bl_sys_init这个函数，蓝牙才可以正常工作
	bl_sys_init();
    ble_controller_init(configMAX_PRIORITIES - 1);
    hci_driver_init();
    bt_enable(bt_enable_cb);
}

/**
 * @brief 蓝牙初始化成功的回调函数
 * @param {int} err
 * @return {*}
 */
void bt_enable_cb(int err)
{
	char addr_s[BT_ADDR_LE_STR_LEN];
	bt_addr_le_t addr = {0};
	size_t count = 1;

	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");

	/* Start advertising */
	err = bt_le_adv_start(BT_LE_ADV_NCONN_IDENTITY, ad, ARRAY_SIZE(ad),
			      sd, ARRAY_SIZE(sd));
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}


	/* For connectable advertising you would use
	 * bt_le_oob_get_local().  For non-connectable non-identity
	 * advertising an non-resolvable private address is used;
	 * there is no API to retrieve that.
	 */

	bt_id_get(&addr, &count);
	bt_addr_le_to_str(&addr, addr_s, sizeof(addr_s));

	printk("Beacon started, advertising as %s\n", addr_s);
}
/*---------- end of file ----------*/


