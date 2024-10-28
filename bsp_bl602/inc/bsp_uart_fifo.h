/*
 * Copyright (c) 2024 by Lu Xianfan.
 * @FilePath     : bsp_uart_fifo.h
 * @Author       : lxf
 * @Date         : 2024-10-21 16:06:54
 * @LastEditors  : flyyingpiggy2020 154562451@qq.com
 * @LastEditTime : 2024-10-28 14:45:17
 * @Brief        :
 */

#ifndef __BSP_UART_FIFO_H__
#define __BSP_UART_FIFO_H__
/*---------- includes ----------*/
#include <bl_uart.h>
#include <hosal_uart.h>
#include "FreeRTOS.h"
#include <task.h>
#include <timers.h>
#include <semphr.h>
/*---------- macro ----------*/

// BL602有两个端口，端口0和端口1.
// 端口 0 是为命令行界面保留的，因此我们应该始终使用 UART 端口 1。
#if (UART1_FIFO_EN == 0)
#define UART1_FIFO_EN     0
#define USART1_TX_PIN     0
#define USART1_RX_PIN     1
#define UART1_BAUD        115200
#define UART1_TX_BUF_SIZE 1 * 1024
#define UART1_RX_BUF_SIZE 1 * 1024
#endif

#if (UART2_FIFO_EN == 0)
#define UART2_FIFO_EN     1
#define USART2_TX_PIN     3
#define USART2_RX_PIN     4
#define UART2_BAUD        115200
#define UART2_TX_BUF_SIZE 1 * 1024
#define UART2_RX_BUF_SIZE 1 * 1024
#endif

/*---------- type define ----------*/
typedef enum {
    COM1 = 0,
    COM2 = 1,
    COM_NUM,
} COM_PORT_E;

typedef struct {
    COM_PORT_E com; /* 当前串口信息 */
    hosal_uart_dev_t *uart;
    uint8_t *pTxBuf;             /* 发送缓冲区 */
    uint8_t *pRxBuf;             /* 接收缓冲区 */
    uint16_t usTxBufSize;        /* 发送缓冲区大小 */
    uint16_t usRxBufSize;        /* 接收缓冲区大小 */
    volatile uint16_t usTxWrite; /* 发送缓冲区写指针 */
    volatile uint16_t usTxRead;  /* 发送缓冲区读指针 */
    volatile uint16_t usTxCount; /* 等待发送的数据个数 */

    volatile uint16_t usRxWrite; /* 接收缓冲区写指针 */
    volatile uint16_t usRxRead;  /* 接收缓冲区读指针 */
    volatile uint16_t usRxCount; /* 还未读取的新数据个数 */

    void (*SendBefor)(COM_PORT_E com);
    void (*SendOver)(COM_PORT_E com);
    TimerHandle_t rto;      /* 接收超时定时器(模拟idle中断) */
    SemaphoreHandle_t idle; /* idle空闲中断 */
    // void (*ReciveNew)(COM_PORT_E com, uint8_t _byte);
    // void (*IdleCallback)(void);
    // uint8_t Sending;                  /* 正在发送中 */
} UART_T;

/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/

void bsp_InitUart(void);
int32_t comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
UART_T *ComToUart(COM_PORT_E _ucPort);
uint16_t comGetBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
/*---------- end of file ----------*/
#endif
