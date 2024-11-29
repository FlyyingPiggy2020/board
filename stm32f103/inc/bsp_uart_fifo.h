/*
 * Copyright (c) 2024 by Lu Xianfan.
 * @FilePath     : bsp_uart_fifo.h
 * @Author       : lxf
 * @Date         : 2024-11-26 13:18:49
 * @LastEditors  : FlyyingPiggy2020 154562451@qq.com
 * @LastEditTime : 2024-11-27 17:07:08
 * @Brief        :
 */

#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_
/*---------- includes ----------*/
#include "bsp.h"

/*---------- macro ----------*/

/*---------- type define ----------*/

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

    void (*SendBefor)(COM_PORT_E com);                /* 开始发送之前的回调函数指针（主要用于RS485切换到发送模式）*/
    void (*SendOver)(COM_PORT_E com);                 /* 发送完毕的回调函数指针（主要用于RS485将发送模式切换为接收模式）*/
    void (*ReciveNew)(COM_PORT_E com, uint8_t _byte); /* 串口收到数据的回调函数指针 */
    void (*IdleCallback)(void);                       /* 空闲中断回调 */
    uint8_t Sending;                                  /* 正在发送中 */
} UART_T;

/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/
void bsp_InitUart(void);
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte);
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte);
int32_t comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
uint16_t comGetBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void comClearTxFifo(COM_PORT_E _ucPort);
void comClearRxFifo(COM_PORT_E _ucPort);
void comSetBaud(COM_PORT_E com, uint32_t _BaudRate);
void comSetUartParam(USART_TypeDef *Instance, uint32_t BaudRate, uint16_t WordLength, uint16_t StopBits, uint32_t Parity, uint32_t Mode);
uint16_t comSetBuf(COM_PORT_E com, uint8_t *_ucaBuf, uint16_t _usLen);

void RS485_SendBefor(COM_PORT_E com);
void RS485_SendOver(COM_PORT_E com);
void RS485_SetBaud(COM_PORT_E com, uint32_t _baud);
uint8_t comTxEmpty(COM_PORT_E com);
/*---------- end of file ----------*/
#endif
