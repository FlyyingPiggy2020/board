/*
 * Copyright (c) 2024 by Lu Xianfan.
 * @FilePath     : bsp_uart_fifo.c
 * @Author       : lxf
 * @Date         : 2024-10-21 16:14:42
 * @LastEditors  : flyyingpiggy2020 154562451@qq.com
 * @LastEditTime : 2024-11-08 16:42:55
 * @Brief        :
 * 1.uart0尽量不用：BL602拥有两个串口uart0,uart1，其中uart0用于log输出，不可用。
 * 2.串口阻塞：在RTOS中使用，我们希望底层的接口是阻塞的。这样子加了互斥之后才会起到作用。
 * 3.模拟空闲中断：利用软件定时器实现串口空闲，默认按照3.5字节去计算，最小值为2ms(因为软件定时器是毫秒级的)
 * 利用一个二值信号量通知任务去处理数据
 *
 */

/*---------- includes ----------*/
#include "bsp.h"

/*---------- macro ----------*/

#if UART1_FIFO_EN == 1
HOSAL_UART_DEV_DECL(uart_dev_int1, 0, USART1_TX_PIN, USART1_RX_PIN, UART1_BAUD);
#endif

#if UART2_FIFO_EN == 1
HOSAL_UART_DEV_DECL(uart_dev_int2, 1, USART2_TX_PIN, USART2_RX_PIN, UART2_BAUD);
#endif

/*---------- type define ----------*/
/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/
static void uart_idle_handler(TimerHandle_t xTimer);

/*---------- variable ----------*/
#if (UART1_FIFO_EN == 1)
static UART_T g_tUart1;
static uint8_t g_TxBuf1[UART1_TX_BUF_SIZE]; /* 发送缓冲区 */
static uint8_t g_RxBuf1[UART1_RX_BUF_SIZE]; /* 接收缓冲区 */
#endif

#if (UART2_FIFO_EN == 1)
UART_T g_tUart2;
uint8_t g_TxBuf2[UART2_TX_BUF_SIZE]; /* 发送缓冲区 */
uint8_t g_RxBuf2[UART2_RX_BUF_SIZE]; /* 接收缓冲区 */
#endif
/*---------- function ----------*/
#if (UART1_FIFO_EN == 1)
static int __uart1_rx_callback(void *p_arg)
{
    int ret;
    uint8_t ch;
    UART_T *_pUart = (UART_T *)p_arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    ret = hosal_uart_receive(_pUart->uart, &ch, 1);
    if (ret > 0) {
        if (_pUart->usRxCount < _pUart->usRxBufSize) {
            _pUart->pRxBuf[_pUart->usRxWrite] = ch;
            if (++_pUart->usRxWrite >= _pUart->usRxBufSize) {
                _pUart->usRxWrite = 0;
            }
            _pUart->usRxCount++;
        }
        xTimerResetFromISR(_pUart->rto, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    return ret;
}
#endif

#if (UART2_FIFO_EN == 1)
static int __uart2_rx_callback(void *p_arg)
{
    int ret;
    uint8_t ch;
    UART_T *_pUart = (UART_T *)p_arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    ret = hosal_uart_receive(_pUart->uart, &ch, 1);
    if (ret > 0) {
        if (_pUart->usRxCount < _pUart->usRxBufSize) {
            _pUart->pRxBuf[_pUart->usRxWrite] = ch;
            if (++_pUart->usRxWrite >= _pUart->usRxBufSize) {
                _pUart->usRxWrite = 0;
            }
            _pUart->usRxCount++;
        }
        xTimerResetFromISR(_pUart->rto, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    return ret;
}
#endif

/**
 * @brief 计算3.5字节超时
 * @param {uint32_t} baud_rate
 * @return {*}
 */
int calc_uart_timeout(uint32_t baud_rate)
{
    double timeout = 3.5 * 10.0 * 1000 / baud_rate; // 计算超时，得到浮点型结果
    return (int)((timeout < 2) ? 2 : timeout);      // 将结果转换为整数并返回，若小于2毫秒则返回2毫秒
}

/**
 * @brief 串口初始化。注意该函数不能被重复调用
 * @return {*}
 */
void bsp_InitUart(void)
{
#if UART1_FIFO_EN == 1
    g_tUart1.com = COM1;
    g_tUart1.uart = &uart_dev_int1;
    g_tUart1.pTxBuf = g_TxBuf1;               /* 发送缓冲区指针 */
    g_tUart1.pRxBuf = g_RxBuf1;               /* 接收缓冲区指针 */
    g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE; /* 发送缓冲区大小 */
    g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE; /* 接收缓冲区大小 */
    g_tUart1.usTxWrite = 0;                   /* 发送FIFO写索引 */
    g_tUart1.usTxRead = 0;                    /* 发送FIFO读索引 */
    g_tUart1.usRxWrite = 0;                   /* 接收FIFO写索引 */
    g_tUart1.usRxRead = 0;                    /* 接收FIFO读索引 */
    g_tUart1.usRxCount = 0;                   /* 接收到的新数据个数 */
    g_tUart1.usTxCount = 0;                   /* 待发送的数据个数 */
    g_tUart1.rto = xTimerCreate("rto_u1", pdMS_TO_TICKS(calc_uart_timeout(UART1_BAUD)), pdFALSE, (void *const)COM1, uart_idle_handler);
    g_tUart1.idle = xSemaphoreCreateBinary();
    g_tUart1.mutex = xSemaphoreCreateMutex();
    hosal_uart_init(&uart_dev_int1);
    hosal_uart_ioctl(&uart_dev_int1, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_INT);
    hosal_uart_callback_set(&uart_dev_int1, HOSAL_UART_RX_CALLBACK, __uart1_rx_callback, &g_tUart1);
    printf("uart1 init success:\r\nbaud_rate=%d\r\ntimeout=%d(ms)\r\nrxio:%d\r\ntxio:%d\r\n",
           UART1_BAUD,
           calc_uart_timeout(UART1_BAUD),
           USART1_RX_PIN,
           USART1_TX_PIN);
#endif

#if UART2_FIFO_EN == 1
    g_tUart2.com = COM2;
    g_tUart2.uart = &uart_dev_int2;
    g_tUart2.pTxBuf = g_TxBuf2;               /* 发送缓冲区指针 */
    g_tUart2.pRxBuf = g_RxBuf2;               /* 接收缓冲区指针 */
    g_tUart2.usTxBufSize = UART2_TX_BUF_SIZE; /* 发送缓冲区大小 */
    g_tUart2.usRxBufSize = UART2_RX_BUF_SIZE; /* 接收缓冲区大小 */
    g_tUart2.usTxWrite = 0;                   /* 发送FIFO写索引 */
    g_tUart2.usTxRead = 0;                    /* 发送FIFO读索引 */
    g_tUart2.usRxWrite = 0;                   /* 接收FIFO写索引 */
    g_tUart2.usRxRead = 0;                    /* 接收FIFO读索引 */
    g_tUart2.usRxCount = 0;                   /* 接收到的新数据个数 */
    g_tUart2.usTxCount = 0;                   /* 待发送的数据个数 */
    g_tUart2.rto = xTimerCreate("rto_u2", pdMS_TO_TICKS(calc_uart_timeout(UART2_BAUD)), pdFALSE, (void *const)COM2, uart_idle_handler);
    g_tUart2.idle = xSemaphoreCreateBinary();
    g_tUart2.mutex = xSemaphoreCreateMutex();
    hosal_uart_init(&uart_dev_int2);
    hosal_uart_ioctl(&uart_dev_int2, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_INT);
    hosal_uart_callback_set(&uart_dev_int2, HOSAL_UART_RX_CALLBACK, __uart2_rx_callback, &g_tUart2);
    printf("uart2 init success:\r\nbaud_rate=%d\r\ntimeout=%d(ms)\r\nrxio:%d\r\ntxio:%d\r\n",
           UART2_BAUD,
           calc_uart_timeout(UART2_BAUD),
           USART2_RX_PIN,
           USART2_TX_PIN);
#endif
    return;
}

/**
 * @brief 将COM端口号转换为UART指针
 * @param {COM_PORT_E} _ucPort 端口号(COM1 - COM2)
 * @return {*}
 */
UART_T *ComToUart(COM_PORT_E _ucPort)
{
    if (_ucPort == COM1) {
#if UART1_FIFO_EN == 1
        return &g_tUart1;
#else
        return 0;
#endif
    } else if (_ucPort == COM2) {
#if UART2_FIFO_EN == 1
        return &g_tUart2;
#else
        return 0;
#endif
    } else {
        return 0;
    }
}

/**
 * @brief 串口发送
 * @param {COM_PORT_E} _ucPort
 * @param {uint8_t} *_ucaBuf
 * @param {uint16_t} _usLen
 * @return {*}
 */
int32_t comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen)
{
    UART_T *pUart;
    int ret = 0;
    pUart = ComToUart(_ucPort);

    if (pUart == 0) {
        return -1;
    }
    xSemaphoreTake(pUart->mutex, portMAX_DELAY);
    if (pUart->SendBefor != NULL) {
        pUart->SendBefor(pUart->com);
    }
    ret = hosal_uart_send(pUart->uart, _ucaBuf, _usLen);
    hosal_uart_ioctl(pUart->uart, HOSAL_UART_FLUSH, NULL); // 阻塞发送
    if (pUart->SendOver != NULL) {
        pUart->SendOver(pUart->com);
    }
    xSemaphoreGive(pUart->mutex);
    return ret;
}

static uint16_t UartGetBuf(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
    uint16_t usCount, usPos = 0;
    uint8_t id = _pUart->uart->config.uart_id;
    /* usRxWrite 变量在中断函数中被改写，主程序读取该变量时，必须进行临界区保护 */
    bl_uart_int_rx_disable(id);
    if (_pUart->usRxCount == 0 && _pUart->usRxRead != _pUart->usRxWrite) {
        _pUart->usRxRead = _pUart->usRxWrite = 0;
    }
    usCount = _pUart->usRxCount;
    bl_uart_int_rx_enable(id);

    /* 如果读和写索引相同，则返回0 */
    if (usCount == 0) /* 已经没有数据 */
    {
        return 0;
    } else {
        /* 改写FIFO读索引 */
        bl_uart_int_rx_disable(id);
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
        bl_uart_int_rx_enable(id);
        return usPos;
    }
}

/**
 * @brief 串口接收
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

static void uart_idle_handler(TimerHandle_t xTimer)
{
    UART_T *pUart;

    pUart = ComToUart((COM_PORT_E)pvTimerGetTimerID(xTimer));

    if (pUart == 0) {
        return;
    }
    // 释放二值信号量，代表串口接收到数据
    xSemaphoreGive(pUart->idle);
}
/*---------- end of file ----------*/
