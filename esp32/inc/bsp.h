/*
 * Copyright (c) 2024 by Moorgen Tech. Co, Ltd.
 * @FilePath     : bsp.h
 * @Author       : lxf
 * @Date         : 2024-03-08 08:35:50
 * @LastEditors  : flyyingpiggy2020 154562451@qq.com
 * @LastEditTime : 2024-08-07 13:18:54
 * @Brief        :
 */

#ifndef __BSP_H__
#define __BSP_H__
/*---------- includes ----------*/

#include "esp_log.h"

#if CONFIG_USE_BSP_USART_FIFO
#include "bsp_usart_fifo.h"
#endif
/*---------- macro ----------*/
/*---------- type define ----------*/
/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/

void bsp_init(void);
/*---------- end of file ----------*/
#endif
