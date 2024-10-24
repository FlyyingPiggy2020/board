/*
 * Copyright (c) 2024 by Lu Xianfan.
 * @FilePath     : bsp_config.example.h
 * @Author       : lxf
 * @Date         : 2024-10-17 15:31:09
 * @LastEditors  : flyyingpiggy2020 154562451@qq.com
 * @LastEditTime : 2024-10-22 14:04:10
 * @Brief        :
 */

#ifndef __BSP_CONFIG_H__
#define __BSP_CONFIG_H__
/*---------- includes ----------*/
/*---------- macro ----------*/

/**
 * @brief 串口设置：
 * bl602有两个串口，其中一个串口被用于日志打印。
 * 这里默认串口0用于日志打印，UART1_FIFO_EN一般为0
 * @return {*}
 */
#define UART1_FIFO_EN 0
#define USART1_TX_PIN 0
#define USART1_RX_PIN 1
#define UART1_BAUD    2000000

#define UART2_FIFO_EN 1
#define USART2_TX_PIN 3
#define USART2_RX_PIN 4
#define UART2_BAUD    115200

/*---------- type define ----------*/
/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/
/*---------- end of file ----------*/
#endif // !__BSP_CONFIG_H__
