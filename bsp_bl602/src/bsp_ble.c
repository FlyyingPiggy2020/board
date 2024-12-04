/*
 * Copyright (c) 2024 by Lu Xianfan.
 * @FilePath     : bsp_ble.c
 * @Author       : lxf
 * @Date         : 2024-10-28 19:01:40
 * @LastEditors  : flyyingpiggy2020 154562451@qq.com
 * @LastEditTime : 2024-11-11 17:40:25
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
/*---------- type define ----------*/
/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/

/*---------- function ----------*/

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
    return;
}
/*---------- end of file ----------*/
