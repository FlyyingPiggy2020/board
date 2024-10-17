/*
*********************************************************************************************************
*
*   模块名称 : 串口中断+FIFO驱动模块
*   文件名称 : bsp_uart_fifo.h
*   说    明 : 头文件
*
*   Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_

#include "bsp.h"

/*
    如果需要更改串口对应的管脚，请自行修改 bsp_uart_fifo.c文件中的 static void
   InitHardUart(void)函数
*/

/* 定义使能的串口, 0 表示不使能（不增加代码大小）， 1表示使能 */
/*
    安富莱STM32-V5 串口分配：
    【串口1】 RS232 芯片第1路。
        PA10/USART1_RX
        PA9/USART1_TX

    【串口2】 PA2 管脚用于以太网； RX管脚用于接收GPS信号
        PA2/USART2_TX/ETH_MDIO (用于以太网，不做串口发送用)
        PA3/USART2_RX   ;接GPS模块输出

    【串口3】 RS485 通信 - TTL 跳线 和 排针
        PB10/USART3_TX
        PB11/USART3_RX
        PB2-BOOT1/RS485_TXEN

    【串口4】 --- 不做串口用。用于SD卡
    【串口5】 --- 不做串口用。用于SD卡
        PC12/UART5_TX
        PD2/UART5_RX

    【串口6】--- GPRS模块 （硬件流控）
        PG14/USART6_TX
        PC7/USART6_RX
        PG8/USART6_RTS
        PG15/USART6_CTS
*/
#ifndef UART1_FIFO_EN
#define UART1_FIFO_EN 1
#endif

#define UART2_FIFO_EN 1

#define UART3_FIFO_EN 0
#define UART4_FIFO_EN 0
#define UART5_FIFO_EN 0
#define UART6_FIFO_EN 0

/* PB2 控制RS485芯片的发送使能 */
#define RS485_TXEN_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define RS485_TXEN_GPIO_PORT GPIOA
#define RS485_TXEN_PIN GPIO_PIN_1

#define RS485_RX_EN()                                                          \
  RS485_TXEN_GPIO_PORT->BSRR = (uint32_t)RS485_TXEN_PIN << 16U
#define RS485_TX_EN() RS485_TXEN_GPIO_PORT->BSRR = RS485_TXEN_PIN

/* 定义端口号 */
typedef enum {
  COM1 = 0,   /* USART1 */
  COM2 = 1,   /* USART2 */
  COM3 = 2,   /* USART3 */
  COM4 = 3,   /* UART4 */
  COM5 = 4,   /* UART5 */
  COM6 = 5,   /* USART6 */
  COM_NUM,    /* COM_NUM */
  COMx = COM2 /* PRINTF */
} COM_PORT_E;

/* 定义串口波特率和FIFO缓冲区大小，分为发送缓冲区和接收缓冲区, 支持全双工 */
#if UART1_FIFO_EN == 1
#define UART1_BAUD 115200
#define UART1_TX_BUF_SIZE 1 * 1024
#define UART1_RX_BUF_SIZE 4 * 1024
#endif

#if UART2_FIFO_EN == 1
#define UART2_BAUD 9600
#define UART2_TX_BUF_SIZE 1 * 1024
#define UART2_RX_BUF_SIZE 1 * 1024
#endif

#if UART3_FIFO_EN == 1
#define UART3_BAUD 115200
#define UART3_TX_BUF_SIZE 1 * 1024
#define UART3_RX_BUF_SIZE 1 * 1024
#endif

#if UART4_FIFO_EN == 1
#define UART4_BAUD 115200
#define UART4_TX_BUF_SIZE 1 * 1024
#define UART4_RX_BUF_SIZE 1 * 1024
#endif

#if UART5_FIFO_EN == 1
#define UART5_BAUD 115200
#define UART5_TX_BUF_SIZE 1 * 1024
#define UART5_RX_BUF_SIZE 1 * 1024
#endif

#if UART6_FIFO_EN == 1
#define UART6_BAUD 115200
#define UART6_TX_BUF_SIZE 1 * 1024
#define UART6_RX_BUF_SIZE 1 * 1024
#endif

/* 串口设备结构体 */
typedef struct {
  COM_PORT_E com;          /* 当前串口信息 */
  USART_TypeDef *uart;     /* STM32内部串口设备指针 */
  uint8_t *pTxBuf;         /* 发送缓冲区 */
  uint8_t *pRxBuf;         /* 接收缓冲区 */
  uint16_t usTxBufSize;    /* 发送缓冲区大小 */
  uint16_t usRxBufSize;    /* 接收缓冲区大小 */
  __IO uint16_t usTxWrite; /* 发送缓冲区写指针 */
  __IO uint16_t usTxRead;  /* 发送缓冲区读指针 */
  __IO uint16_t usTxCount; /* 等待发送的数据个数 */

  __IO uint16_t usRxWrite; /* 接收缓冲区写指针 */
  __IO uint16_t usRxRead;  /* 接收缓冲区读指针 */
  __IO uint16_t usRxCount; /* 还未读取的新数据个数 */

  void (*SendBefor)(COM_PORT_E com); /* 开始发送之前的回调函数指针（主要用于RS485切换到发送模式）
                                      */
  void (*SendOver)(COM_PORT_E com); /* 发送完毕的回调函数指针（主要用于RS485将发送模式切换为接收模式）
                                     */
  void (*ReciveNew)(COM_PORT_E com,
                    uint8_t _byte); /* 串口收到数据的回调函数指针 */
  void (*IdleCallback)(void);       /* 空闲中断回调 */
  uint8_t Sending;                  /* 正在发送中 */
} UART_T;

void bsp_InitUart(void);
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte);
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte);
int32_t comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
uint16_t comGetBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void comClearTxFifo(COM_PORT_E _ucPort);
void comClearRxFifo(COM_PORT_E _ucPort);
void comSetBaud(COM_PORT_E com, uint32_t _BaudRate);
void comSetUartParam(USART_TypeDef *Instance, uint32_t BaudRate,
                     uint16_t WordLength, uint16_t StopBits, uint32_t Parity,
                     uint32_t Mode);
uint16_t comSetBuf(COM_PORT_E com, uint8_t *_ucaBuf, uint16_t _usLen);

void RS485_SendBefor(COM_PORT_E com);
void RS485_SendOver(COM_PORT_E com);
void RS485_SendBuf(COM_PORT_E com, uint8_t *_ucaBuf, uint16_t _usLen);
void RS485_SendStr(COM_PORT_E com, char *_pBuf);
void RS485_SetBaud(COM_PORT_E com, uint32_t _baud);
uint8_t comTxEmpty(COM_PORT_E com);
UART_T *comTxToUart(USART_TypeDef *_uart);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE)
 * *********************************/
