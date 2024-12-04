/*
 * Copyright (c) 2024 by Lu Xianfan.
 * @FilePath     : bsp_uart_fifo.c
 * @Author       : lxf
 * @Date         : 2024-11-26 15:36:00
 * @LastEditors  : FlyyingPiggy2020 154562451@qq.com
 * @LastEditTime : 2024-12-03 10:36:52
 * @Brief        : stm32f103串口驱动程序
 * 更新日志：
 * 2024-11-26   lxf     魔改自安富莱串口驱动程序，支持了通过字符串配置串口
 */

/*---------- includes ----------*/
#include "bsp.h"
#include <stdbool.h>

/*---------- macro ----------*/
/*---------- type define ----------*/
/*---------- variable prototype ----------*/
#if CONFIG_BSP_UART1_EN == 1
static UART_T g_tUart1;
static uint8_t g_TxBuf1[CONFIG_BSP_UART1_TX_BUF_SIZE]; /* 发送缓冲区 */
static uint8_t g_RxBuf1[CONFIG_BSP_UART1_RX_BUF_SIZE]; /* 接收缓冲区 */
#endif

#if CONFIG_BSP_UART2_EN == 1
UART_T g_tUart2;
uint8_t g_TxBuf2[CONFIG_BSP_UART2_TX_BUF_SIZE]; /* 发送缓冲区 */
uint8_t g_RxBuf2[CONFIG_BSP_UART2_RX_BUF_SIZE]; /* 接收缓冲区 */
#endif

#if CONFIG_BSP_UART3_EN == 1
static UART_T g_tUart3;
static uint8_t g_TxBuf3[CONFIG_BSP_UART3_TX_BUF_SIZE]; /* 发送缓冲区 */
static uint8_t g_RxBuf3[CONFIG_BSP_UART3_RX_BUF_SIZE]; /* 接收缓冲区 */
#endif

#if CONFIG_BSP_USART1_485_EN == 1
#endif

#if CONFIG_BSP_USART2_485_EN == 1
#endif

#if CONFIG_BSP_USART3_485_EN == 1
#endif

/*---------- function prototype ----------*/
static void UartVarInit(void);
static void InitHardUart(void);
static int32_t UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen);
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte);
static uint16_t UartGetBuf(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen);
static uint16_t UartSetBuf(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen);
static void UartIRQ(UART_T *_pUart);
void RS485_InitTXE(void);
void RS485_SendBefor(COM_PORT_E com);
void RS485_SendOver(COM_PORT_E com);
/*---------- variable ----------*/

/*---------- function ----------*/

/**
 * @brief 初始化串口硬件
 * @return {*}
 */
void bsp_InitUart(void)
{
    UartVarInit();   /* 必须先初始化全局变量,再配置硬件 */
    InitHardUart();  /* 配置串口的硬件参数(波特率等) */
    RS485_InitTXE(); /* 配置RS485芯片的发送使能硬件，配置为推挽输出 */
}

/**
 * @brief 将COM端口号转换为UART指针
 * @param {COM_PORT_E} _ucPort
 * @return {*}
 */
UART_T *ComToUart(COM_PORT_E _ucPort)
{
    if (_ucPort == COM1) {
#if CONFIG_BSP_UART1_EN == 1
        return &g_tUart1;
#else
        return 0;
#endif
    } else if (_ucPort == COM2) {
#if CONFIG_BSP_UART2_EN == 1
        return &g_tUart2;
#else
        return 0;
#endif
    } else if (_ucPort == COM3) {
#if CONFIG_BSP_UART3_EN == 1
        return &g_tUart3;
#else
        return 0;
#endif
    } else if (_ucPort == COM4) {
#if CONFIG_BSP_UART4_EN == 1
        return &g_tUart4;
#else
        return 0;
#endif
    } else if (_ucPort == COM5) {
#if CONFIG_BSP_UART5_EN == 1
        return &g_tUart5;
#else
        return 0;
#endif
    } else {
        bsp_Error_Handler(__FILE__, __LINE__);
        return 0;
    }
}

/**
 * @brief 将COM端口号转换为USART端口
 * @param {COM_PORT_E} _ucPort
 * @return {*}
 */
USART_TypeDef *ComToUSARTx(COM_PORT_E _ucPort)
{
    if (_ucPort == COM1) {
#if CONFIG_BSP_UART1_EN == 1
        return USART1;
#else
        return 0;
#endif
    } else if (_ucPort == COM2) {
#if CONFIG_BSP_UART2_EN == 1
        return USART2;
#else
        return 0;
#endif
    } else if (_ucPort == COM3) {
#if CONFIG_BSP_UART3_EN == 1
        return USART3;
#else
        return 0;
#endif
    } else if (_ucPort == COM4) {
#if CONFIG_BSP_UART4_EN == 1
        return UART4;
#else
        return 0;
#endif
    } else if (_ucPort == COM5) {
#if CONFIG_BSP_UART5_EN == 1
        return UART5;
#else
        return 0;
#endif
    } else {
        /* 不做任何处理 */
        return 0;
    }
}

/**
 * @brief 向串口发送Buffer
 * @param {COM_PORT_E} _ucPort
 * @param {uint8_t} *_ucaBuf
 * @param {uint16_t} _usLen
 * @return {*}
 */
int32_t comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen)
{
    UART_T *pUart;

    pUart = ComToUart(_ucPort);
    if (pUart == 0) {
        return -1;
    }

    if (pUart->SendBefor != 0) {
        pUart->SendBefor(pUart->com); /* 如果是RS485通信，可以在这个函数中将RS485设置为发送模式 */
    }

    return UartSend(pUart, _ucaBuf, _usLen);
}

/**
 * @brief 向COM口发送一个字节
 * @param {COM_PORT_E} _ucPort
 * @param {uint8_t} _ucByte
 * @return {*}
 */
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte)
{
    comSendBuf(_ucPort, &_ucByte, 1);
}

/**
 * @brief 从接收缓冲区读取1字节，非阻塞。无论有无数据均立即返回。
 * @param {COM_PORT_E} _ucPort
 * @param {uint8_t} *_pByte
 * @return {*}
 */
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte)
{
    UART_T *pUart;

    pUart = ComToUart(_ucPort);
    if (pUart == 0) {
        return 0;
    }

    return UartGetChar(pUart, _pByte);
}

/**
 * @brief 从接收缓冲区放入多个字节，非阻塞。无论有无数据均立即返回。
 * @param {COM_PORT_E} _ucPort
 * @param {uint8_t} *_ucaBuf
 * @param {uint16_t} _usLen
 * @return {*}
 */
uint16_t comSetBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen)
{
    UART_T *pUart;

    pUart = ComToUart(_ucPort);
    if (pUart == 0) {
        return 0;
    }

    return UartSetBuf(pUart, _ucaBuf, _usLen);
}

/**
 * @brief 从接收缓冲区读取多个字节，非阻塞。无论有无数据均立即返回。
 * @param {COM_PORT_E} _ucPort
 * @param {uint8_t} *_ucaBuf
 * @param {uint16_t} _usLen
 * @return {*}
 */
uint16_t comGetBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen)
{
    UART_T *pUart;

    pUart = ComToUart(_ucPort);
    if (pUart == 0) {
        return 0;
    }

    return UartGetBuf(pUart, _ucaBuf, _usLen);
}

/**
 * @brief 清零串口发送缓冲区
 * @param {COM_PORT_E} _ucPort
 * @return {*}
 */
void comClearTxFifo(COM_PORT_E _ucPort)
{
    UART_T *pUart;

    pUart = ComToUart(_ucPort);
    if (pUart == 0) {
        return;
    }

    pUart->usTxWrite = 0;
    pUart->usTxRead = 0;
    pUart->usTxCount = 0;
}

/**
 * @brief 清零串口接收缓冲区
 * @param {COM_PORT_E} _ucPort
 * @return {*}
 */
void comClearRxFifo(COM_PORT_E _ucPort)
{
    UART_T *pUart;

    pUart = ComToUart(_ucPort);
    if (pUart == 0) {
        return;
    }

    pUart->usRxWrite = 0;
    pUart->usRxRead = 0;
    pUart->usRxCount = 0;
}

/**
 * @brief 设置串口的波特率. 本函数固定设置为无校验，收发都使能模式
 * @param {COM_PORT_E} _ucPort
 * @param {uint32_t} _BaudRate
 * @return {*}
 */
void comSetBaud(COM_PORT_E _ucPort, uint32_t _BaudRate)
{
    USART_TypeDef *USARTx;

    USARTx = ComToUSARTx(_ucPort);
    if (USARTx == 0) {
        return;
    }

    comSetUartParam(USARTx, _BaudRate, UART_WORDLENGTH_8B, UART_STOPBITS_1, UART_PARITY_NONE, UART_MODE_TX_RX);
}

static bool _rs485_txe_init(const char *name)
{
    GPIO_InitTypeDef gpio_init;
    GPIO_TypeDef *io_port;
    uint32_t io_pin;

    if (!_translate_pin_name(name, &io_port, &io_pin)) {
        return false;
    }

    /* 配置引脚为推挽输出 */
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;   /* 推挽输出 */
    gpio_init.Pull = GPIO_NOPULL;           /* 上下拉电阻不使能 */
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH; /* GPIO速度等级 */
    gpio_init.Pin = io_pin;
    HAL_GPIO_Init(io_port, &gpio_init);
    return true;
}
/**
 * @brief 配置485发送使能
 * @return {*}
 */
void RS485_InitTXE(void)
{
#if CONFIG_BSP_USART1_485_EN == 1
    _rs485_txe_init(CONFIG_BSP_USART1_485_TXE_IO);
#endif
}

void RS485_TX_EN(COM_PORT_E com)
{
    if (com == COM3) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
    }
}

void RS485_RX_EN(COM_PORT_E com)
{
    if (com == COM3) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
    }
}
/**
 * @brief 修改串口的波特率。
 * @param {COM_PORT_E} _ucPort
 * @param {uint32_t} _baud
 * @return {*}
 */
void RS485_SetBaud(COM_PORT_E _ucPort, uint32_t _baud)
{
    comSetBaud(_ucPort, _baud);
}

/**
 * @brief 发送数据前的准备工作。对于RS485通信，请设置RS485芯片为发送状态
 * @param {COM_PORT_E} com
 * @return {*}
 */
void RS485_SendBefor(COM_PORT_E com)
{
    RS485_TX_EN(com); /* 切换RS485收发芯片为发送模式 */
}

/**
 * @brief 发送一串数据结束后的善后处理。对于RS485通信，请设置RS485芯片为接收状态
 * @param {COM_PORT_E} com
 * @return {*}
 */
void RS485_SendOver(COM_PORT_E com)
{
    RS485_RX_EN(com); /* 切换RS485收发芯片为接收模式 */
}

/*
*********************************************************************************************************
*   函 数 名: UartVarInit
*   功能说明: 初始化串口相关的变量
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
static void UartVarInit(void)
{
#if CONFIG_BSP_UART1_EN == 1
    g_tUart1.com = COM1;                                 /* 当前串口信息 */
    g_tUart1.uart = USART1;                              /* STM32 串口设备 */
    g_tUart1.pTxBuf = g_TxBuf1;                          /* 发送缓冲区指针 */
    g_tUart1.pRxBuf = g_RxBuf1;                          /* 接收缓冲区指针 */
    g_tUart1.usTxBufSize = CONFIG_BSP_UART1_TX_BUF_SIZE; /* 发送缓冲区大小 */
    g_tUart1.usRxBufSize = CONFIG_BSP_UART1_RX_BUF_SIZE; /* 接收缓冲区大小 */
    g_tUart1.usTxWrite = 0;                              /* 发送FIFO写索引 */
    g_tUart1.usTxRead = 0;                               /* 发送FIFO读索引 */
    g_tUart1.usRxWrite = 0;                              /* 接收FIFO写索引 */
    g_tUart1.usRxRead = 0;                               /* 接收FIFO读索引 */
    g_tUart1.usRxCount = 0;                              /* 接收到的新数据个数 */
    g_tUart1.usTxCount = 0;                              /* 待发送的数据个数 */
    g_tUart1.SendBefor = RS485_SendBefor;                /* 发送数据前的回调函数 */
    g_tUart1.SendOver = RS485_SendOver;                  /* 发送完毕后的回调函数 */
    g_tUart1.ReciveNew = 0;                              /* 接收到新数据后的回调函数 */
    g_tUart1.Sending = 0;                                /* 正在发送中标志 */
    g_tUart1.IdleCallback = 0;
#endif

#if CONFIG_BSP_UART2_EN == 1
    g_tUart2.com = COM2;                                 /* 当前串口信息 */
    g_tUart2.uart = USART2;                              /* STM32 串口设备 */
    g_tUart2.pTxBuf = g_TxBuf2;                          /* 发送缓冲区指针 */
    g_tUart2.pRxBuf = g_RxBuf2;                          /* 接收缓冲区指针 */
    g_tUart2.usTxBufSize = CONFIG_BSP_UART2_TX_BUF_SIZE; /* 发送缓冲区大小 */
    g_tUart2.usRxBufSize = CONFIG_BSP_UART2_RX_BUF_SIZE; /* 接收缓冲区大小 */
    g_tUart2.usTxWrite = 0;                              /* 发送FIFO写索引 */
    g_tUart2.usTxRead = 0;                               /* 发送FIFO读索引 */
    g_tUart2.usRxWrite = 0;                              /* 接收FIFO写索引 */
    g_tUart2.usRxRead = 0;                               /* 接收FIFO读索引 */
    g_tUart2.usRxCount = 0;                              /* 接收到的新数据个数 */
    g_tUart2.usTxCount = 0;                              /* 待发送的数据个数 */
    g_tUart2.SendBefor = RS485_SendBefor;                /* 发送数据前的回调函数 */
    g_tUart2.SendOver = RS485_SendOver;                  /* 发送完毕后的回调函数 */
    g_tUart2.ReciveNew = 0;                              /* 接收到新数据后的回调函数 */
    g_tUart2.Sending = 0;                                /* 正在发送中标志 */
    g_tUart2.IdleCallback = 0;
#endif

#if CONFIG_BSP_UART3_EN == 1
    g_tUart3.com = COM3;                                 /* 当前串口信息 */
    g_tUart3.uart = USART3;                              /* STM32 串口设备 */
    g_tUart3.pTxBuf = g_TxBuf3;                          /* 发送缓冲区指针 */
    g_tUart3.pRxBuf = g_RxBuf3;                          /* 接收缓冲区指针 */
    g_tUart3.usTxBufSize = CONFIG_BSP_UART3_TX_BUF_SIZE; /* 发送缓冲区大小 */
    g_tUart3.usRxBufSize = CONFIG_BSP_UART3_RX_BUF_SIZE; /* 接收缓冲区大小 */
    g_tUart3.usTxWrite = 0;                              /* 发送FIFO写索引 */
    g_tUart3.usTxRead = 0;                               /* 发送FIFO读索引 */
    g_tUart3.usRxWrite = 0;                              /* 接收FIFO写索引 */
    g_tUart3.usRxRead = 0;                               /* 接收FIFO读索引 */
    g_tUart3.usRxCount = 0;                              /* 接收到的新数据个数 */
    g_tUart3.usTxCount = 0;                              /* 待发送的数据个数 */
    g_tUart3.SendBefor = RS485_SendBefor;                /* 发送数据前的回调函数 */
    g_tUart3.SendOver = RS485_SendOver;                  /* 发送完毕后的回调函数 */
    g_tUart3.ReciveNew = 0;                              /* 接收到新数据后的回调函数 */
    g_tUart3.Sending = 0;                                /* 正在发送中标志 */
#endif
}

/**
 * @brief 配置串口的硬件参数（波特率，数据位，停止位，起始位，校验位，中断使能）
 * @param {USART_TypeDef} *Instance
 * @param {uint32_t} BaudRate
 * @param {uint16_t} WordLength
 * @param {uint16_t} StopBits
 * @param {uint32_t} Parity
 * @param {uint32_t} Mode
 * @return {*}
 */
void comSetUartParam(USART_TypeDef *Instance, uint32_t BaudRate, uint16_t WordLength, uint16_t StopBits, uint32_t Parity, uint32_t Mode)
{
    UART_HandleTypeDef UartHandle;

    UartHandle.Instance = Instance;

    UartHandle.Init.BaudRate = BaudRate;
    UartHandle.Init.WordLength = WordLength;
    UartHandle.Init.StopBits = StopBits;
    UartHandle.Init.Parity = Parity;
    UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode = Mode;
    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&UartHandle) != HAL_OK) {
        bsp_Error_Handler(__FILE__, __LINE__);
    }
}

/*
*********************************************************************************************************
*   函 数 名: InitHardUart
*   功能说明:
*配置串口的硬件参数（波特率，数据位，停止位，起始位，校验位，中断使能）适合于STM32-H7开发板
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
static void InitHardUart(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_TypeDef *tx_port, *rx_port;
    uint32_t tx_pin, rx_pin;
#if CONFIG_BSP_UART1_EN == 1 /* 串口1 */
    do {
        /* 将字符串转换成GPIO */
        if (!_translate_pin_name(CONFIG_BSP_USART1_TX_IO, &tx_port, &tx_pin) || !_translate_pin_name(CONFIG_BSP_USART1_RX_IO, &rx_port, &rx_pin)) {
            break;
        }

        /* 使能 USARTx 时钟 */
        __HAL_RCC_USART1_CLK_ENABLE();

        /* 配置TX引脚 */
        GPIO_InitStruct.Pin = tx_pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(tx_port, &GPIO_InitStruct);

        /* 配置RX引脚 */
        GPIO_InitStruct.Pin = rx_pin;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        HAL_GPIO_Init(rx_port, &GPIO_InitStruct);

        /* 配置NVIC the NVIC for UART */
        HAL_NVIC_SetPriority(USART1_IRQn, 0, 2);
        HAL_NVIC_EnableIRQ(USART1_IRQn);

        /* 配置波特率、奇偶校验 */
        comSetUartParam(USART1, CONFIG_BSP_UART1_BAUD, UART_WORDLENGTH_8B, UART_STOPBITS_1, UART_PARITY_NONE, UART_MODE_TX_RX);

        CLEAR_BIT(USART1->SR, USART_SR_TC);   /* 清除TC发送完成标志 */
        CLEAR_BIT(USART1->SR, USART_SR_RXNE); /* 清除RXNE接收标志 */
        // USART_CR1_PEIE | USART_CR1_RXNEIE
        SET_BIT(USART1->CR1, USART_CR1_RXNEIE); /* 使能PE. RX接受中断 */
    } while (0);
#endif

#if CONFIG_BSP_UART2_EN == 1 /* 串口2 */
    do {
        /* 将字符串转换成GPIO */
        if (!_translate_pin_name(CONFIG_BSP_USART2_TX_IO, &tx_port, &tx_pin) || !_translate_pin_name(CONFIG_BSP_USART2_RX_IO, &rx_port, &rx_pin)) {
            break;
        }
        /* 使能 USARTx 时钟 */
        __HAL_RCC_USART2_CLK_ENABLE();

        /* 配置TX引脚 */
        GPIO_InitStruct.Pin = tx_pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(tx_port, &GPIO_InitStruct);

        /* 配置RX引脚 */
        GPIO_InitStruct.Pin = rx_pin;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        HAL_GPIO_Init(rx_port, &GPIO_InitStruct);

        /* 配置NVIC the NVIC for UART */
        HAL_NVIC_SetPriority(USART2_IRQn, 0, 2);
        HAL_NVIC_EnableIRQ(USART2_IRQn);

        /* 配置波特率、奇偶校验 */
        comSetUartParam(USART2, CONFIG_BSP_UART2_BAUD, UART_WORDLENGTH_8B, UART_STOPBITS_1, UART_PARITY_NONE, UART_MODE_TX_RX); // UART_MODE_TX_RX

        CLEAR_BIT(USART2->SR, USART_SR_TC);     /* 清除TC发送完成标志 */
        CLEAR_BIT(USART2->SR, USART_SR_RXNE);   /* 清除RXNE接收标志 */
        SET_BIT(USART2->CR1, USART_CR1_RXNEIE); /* 使能PE. RX接受中断 */
        SET_BIT(USART2->CR1, USART_CR1_IDLEIE);
    } while (0);

#endif

#if CONFIG_BSP_UART3_EN == 1 /* 串口3 */
    do {
        if (!_translate_pin_name(CONFIG_BSP_USART3_TX_IO, &tx_port, &tx_pin) || !_translate_pin_name(CONFIG_BSP_USART3_RX_IO, &rx_port, &rx_pin)) {
            break;
        } /* 使能 USARTx 时钟 */
        __HAL_RCC_USART3_CLK_ENABLE();

        /* 配置TX引脚 */
        GPIO_InitStruct.Pin = tx_pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(tx_port, &GPIO_InitStruct);

        /* 配置RX引脚 */
        GPIO_InitStruct.Pin = rx_pin;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        HAL_GPIO_Init(rx_port, &GPIO_InitStruct);

        /* 配置NVIC the NVIC for UART */
        HAL_NVIC_SetPriority(USART3_IRQn, 0, 1);
        HAL_NVIC_EnableIRQ(USART3_IRQn);

        /* 配置波特率、奇偶校验 */
        comSetUartParam(USART3, CONFIG_BSP_UART3_BAUD, UART_WORDLENGTH_8B, UART_STOPBITS_1, UART_PARITY_NONE, UART_MODE_TX_RX);

        CLEAR_BIT(USART3->SR, USART_SR_TC);     /* 清除TC发送完成标志 */
        CLEAR_BIT(USART3->SR, USART_SR_RXNE);   /* 清除RXNE接收标志 */
        SET_BIT(USART3->CR1, USART_CR1_RXNEIE); /* 使能PE. RX接受中断 */
    } while (0);

#endif
}

/*
*********************************************************************************************************
*   函 数 名: UartSend
*   功能说明:
*填写数据到UART发送缓冲区,并启动发送中断。中断处理函数发送完毕后，自动关闭发送中断
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
static int32_t UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
    uint16_t i;

    for (i = 0; i < _usLen; i++) {
        /* 如果发送缓冲区已经满了，则等待缓冲区空 */
        while (1) {
            __IO uint16_t usCount;

            DISABLE_INT();
            usCount = _pUart->usTxCount;
            ENABLE_INT();

            if (usCount < _pUart->usTxBufSize) {
                break;
            } else if (usCount == _pUart->usTxBufSize) /* 数据已填满缓冲区 */
            {
                if ((_pUart->uart->CR1 & USART_CR1_TXEIE) == 0) {
                    SET_BIT(_pUart->uart->CR1, USART_CR1_TXEIE);
                }
            }
        }

        /* 将新数据填入发送缓冲区 */
        _pUart->pTxBuf[_pUart->usTxWrite] = _ucaBuf[i];

        DISABLE_INT();
        if (++_pUart->usTxWrite >= _pUart->usTxBufSize) {
            _pUart->usTxWrite = 0;
        }
        _pUart->usTxCount++;
        ENABLE_INT();
    }

    SET_BIT(_pUart->uart->CR1, USART_CR1_TXEIE); /* 使能发送中断（缓冲区空） */

    return i;
}

/*
*********************************************************************************************************
*   函 数 名: UartGetChar
*   功能说明: 从串口接收缓冲区读取1字节数据 （用于主程序调用）
*   形    参: _pUart : 串口设备
*             _pByte : 存放读取数据的指针
*   返 回 值: 0 表示无数据  1表示读取到数据
*********************************************************************************************************
*/
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte)
{
    uint16_t usCount;

    /* usRxWrite 变量在中断函数中被改写，主程序读取该变量时，必须进行临界区保护 */
    DISABLE_INT();
    usCount = _pUart->usRxCount;
    ENABLE_INT();

    /* 如果读和写索引相同，则返回0 */
    // if (_pUart->usRxRead == usRxWrite)
    if (usCount == 0) /* 已经没有数据 */
    {
        return 0;
    } else {
        *_pByte = _pUart->pRxBuf[_pUart->usRxRead]; /* 从串口接收FIFO取1个数据 */

        /* 改写FIFO读索引 */
        DISABLE_INT();
        if (++_pUart->usRxRead >= _pUart->usRxBufSize) {
            _pUart->usRxRead = 0;
        }
        _pUart->usRxCount--;
        ENABLE_INT();
        return 1;
    }
}

/*
*********************************************************************************************************
*   函 数 名: UartGetBuf
*   功能说明: 从串口接收缓冲区读取多个字节数据 （用于主程序调用）
*   形    参: _pUart : 串口设备
*             _pByte : 存放读取数据的指针
*   返 回 值: 0 表示无数据  1表示读取到数据
*********************************************************************************************************
*/
static uint16_t UartGetBuf(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
    uint16_t usCount, usPos = 0;

    /* usRxWrite 变量在中断函数中被改写，主程序读取该变量时，必须进行临界区保护 */
    DISABLE_INT();
    if (_pUart->usRxCount == 0 && _pUart->usRxRead != _pUart->usRxWrite) {
        _pUart->usRxRead = _pUart->usRxWrite = 0;
    }
    usCount = _pUart->usRxCount;
    ENABLE_INT();

    /* 如果读和写索引相同，则返回0 */
    // if (_pUart->usRxRead == usRxWrite)
    if (usCount == 0) /* 已经没有数据 */
    {
        return 0;
    } else {
        /* 改写FIFO读索引 */
        DISABLE_INT();
        while (usCount > 0 && usPos < _usLen) {
            *_ucaBuf++ =
                _pUart->pRxBuf[_pUart->usRxRead]; /* 从串口接收FIFO取1个数据 */
            if (++_pUart->usRxRead >= _pUart->usRxBufSize) {
                _pUart->usRxRead = 0;
            }
            _pUart->usRxCount--;
            usCount--;
            usPos++;
        }
        ENABLE_INT();
        return usPos;
    }
}

/*
*********************************************************************************************************
*   函 数 名: UartGetChar
*   功能说明: 从串口接收缓冲区放入n字节数据 （用于主程序调用）
*   形    参: _pUart : 串口设备
*             _pByte : 存放读取数据的指针
*   返 回 值: 0 表示无数据  1表示读取到数据
*********************************************************************************************************
*/
static uint16_t UartSetBuf(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
    uint16_t usCount = 0;

    DISABLE_INT();
    while (usCount < _usLen) {
        _pUart->pRxBuf[_pUart->usRxWrite] = _ucaBuf[usCount++];
        if (++_pUart->usRxWrite >= _pUart->usRxBufSize) {
            _pUart->usRxWrite = 0;
        }
        if (_pUart->usRxCount < _pUart->usRxBufSize) {
            _pUart->usRxCount++;
        }
    }
    ENABLE_INT();

    return usCount;
}

/*
*********************************************************************************************************
*   函 数 名: UartTxEmpty
*   功能说明: 判断发送缓冲区是否为空。
*   形    参:  _pUart : 串口设备
*   返 回 值: 1为空。0为不空。
*********************************************************************************************************
*/
uint8_t comTxEmpty(COM_PORT_E _ucPort)
{
    UART_T *pUart;
    uint8_t Sending;

    pUart = ComToUart(_ucPort);
    if (pUart == 0) {
        return 0;
    }

    Sending = pUart->Sending;

    if (Sending != 0) {
        return 0;
    }
    return 1;
}

/*
*********************************************************************************************************
*   函 数 名: UartIRQ
*   功能说明: 供中断服务程序调用，通用串口中断处理函数
*   形    参: _pUart : 串口设备
*   返 回 值: 无
*********************************************************************************************************
*/
static void UartIRQ(UART_T *_pUart)
{
    uint32_t isrflags = READ_REG(_pUart->uart->SR);
    uint32_t cr1its = READ_REG(_pUart->uart->CR1);
    uint32_t cr3its = READ_REG(_pUart->uart->CR3);

    /* 处理接收中断  */
    if ((isrflags & USART_SR_RXNE) != RESET) {
        /* 从串口接收数据寄存器读取数据存放到接收FIFO */
        uint8_t ch;

        ch = READ_REG(_pUart->uart->DR);
        if (_pUart->usRxCount < _pUart->usRxBufSize) {
            _pUart->pRxBuf[_pUart->usRxWrite] = ch;
            if (++_pUart->usRxWrite >= _pUart->usRxBufSize) {
                _pUart->usRxWrite = 0;
            }
            _pUart->usRxCount++;
        }

        /* 回调函数,通知应用程序收到新数据,一般是发送1个消息或者设置一个标记 */
        // if (_pUart->usRxWrite == _pUart->usRxRead)
        // if (_pUart->usRxCount == 1)
        {
            if (_pUart->ReciveNew) {
                _pUart->ReciveNew(_pUart->com,
                                  ch); /* 比如，交给MODBUS解码程序处理字节流 */
            }
        }
    }

    /* 处理发送缓冲区空中断 */
    if (((isrflags & USART_SR_TXE) != RESET) && (cr1its & USART_CR1_TXEIE) != RESET) {
        // if (_pUart->usTxRead == _pUart->usTxWrite)
        if (_pUart->usTxCount == 0) {
            /* 发送缓冲区的数据已取完时， 禁止发送缓冲区空中断
             * （注意：此时最后1个数据还未真正发送完毕）*/
            // USART_ITConfig(_pUart->uart, USART_IT_TXE, DISABLE);
            CLEAR_BIT(_pUart->uart->CR1, USART_CR1_TXEIE);

            /* 使能数据发送完毕中断 */
            // USART_ITConfig(_pUart->uart, USART_IT_TC, ENABLE);
            SET_BIT(_pUart->uart->CR1, USART_CR1_TCIE);
        } else {
            _pUart->Sending = 1;

            /* 从发送FIFO取1个字节写入串口发送数据寄存器 */
            // USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
            _pUart->uart->DR = _pUart->pTxBuf[_pUart->usTxRead];
            if (++_pUart->usTxRead >= _pUart->usTxBufSize) {
                _pUart->usTxRead = 0;
            }
            _pUart->usTxCount--;
        }
    }
    /* 数据bit位全部发送完毕的中断 */
    if (((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET)) {
        // if (_pUart->usTxRead == _pUart->usTxWrite)
        if (_pUart->usTxCount == 0) {
            /* 如果发送FIFO的数据全部发送完毕，禁止数据发送完毕中断 */
            // USART_ITConfig(_pUart->uart, USART_IT_TC, DISABLE);
            CLEAR_BIT(_pUart->uart->CR1, USART_CR1_TCIE);

            /* 回调函数,
             * 一般用来处理RS485通信，将RS485芯片设置为接收模式，避免抢占总线 */
            if (_pUart->SendOver) {
                _pUart->SendOver(_pUart->com);
            }

            _pUart->Sending = 0;
        } else {
            /* 正常情况下，不会进入此分支 */

            /* 如果发送FIFO的数据还未完毕，则从发送FIFO取1个数据写入发送数据寄存器 */
            // USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
            _pUart->uart->DR = _pUart->pTxBuf[_pUart->usTxRead];
            if (++_pUart->usTxRead >= _pUart->usTxBufSize) {
                _pUart->usTxRead = 0;
            }
            _pUart->usTxCount--;
        }
    }
    if ((isrflags & USART_SR_IDLE) != RESET) {
        __attribute__((unused)) uint8_t ch;
        if (_pUart->IdleCallback != 0) {
            _pUart->IdleCallback();
        }
        ch = READ_REG(_pUart->uart->DR);
        CLEAR_BIT(_pUart->uart->SR, USART_SR_IDLE);
    }
}

/*
*********************************************************************************************************
*   函 数 名: USART1_IRQHandler  USART2_IRQHandler USART3_IRQHandler
*UART4_IRQHandler UART5_IRQHandler等 功能说明: USART中断服务程序 形    参: 无 返
*回 值: 无
*********************************************************************************************************
*/
#if CONFIG_BSP_UART1_EN == 1
void USART1_IRQHandler(void)
{
    UartIRQ(&g_tUart1);
}
#endif

#if CONFIG_BSP_UART2_EN == 1
void USART2_IRQHandler(void)
{
    UartIRQ(&g_tUart2);
}
#endif

#if CONFIG_BSP_UART3_EN == 1
void USART3_IRQHandler(void)
{
    UartIRQ(&g_tUart3);
}
#endif

#if (PRINTF_MODE == PRINTF_UART) // 串口调试
/*
*********************************************************************************************************
*   函 数 名: fputc
*   功能说明: 重定义putc函数，这样可以使用printf函数从串口1打印输出
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
int fputc(int ch, FILE *f)
{
    comSendChar(COMx, ch);
    return ch;
}

/*
*********************************************************************************************************
*   函 数 名: fgetc
*   功能说明: 重定义getc函数，这样可以使用getchar函数从串口1输入数据
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
int fgetc(FILE *f)
{
    uint8_t ucData;

    if (comGetChar(COMx, &ucData) > 0) {
        return ucData;
    }
    return -1;
}
#endif
/*---------- end of file ----------*/
