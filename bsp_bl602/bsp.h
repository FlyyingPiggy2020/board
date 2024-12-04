/*
 * Copyright (c) 2024 by Lu Xianfan.
 * @FilePath     : bsp.h
 * @Author       : lxf
 * @Date         : 2024-10-21 16:11:43
 * @LastEditors  : flyyingpiggy2020 154562451@qq.com
 * @LastEditTime : 2024-10-30 14:48:00
 * @Brief        :
 */

#ifndef __BSP_H__
#define __BSP_H__
/*---------- includes ----------*/
#include "FreeRTOS.h"
#include <task.h>
#include <timers.h>
#include <semphr.h>

#include "bsp_config.h"
#include "bsp_uart_fifo.h"
#include "bsp_ble.h"
/*---------- macro ----------*/
/*---------- type define ----------*/
/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/

int bsp_Init(void);
/*---------- end of file ----------*/
#endif
