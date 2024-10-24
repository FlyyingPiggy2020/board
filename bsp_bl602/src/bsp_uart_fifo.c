/*
 * Copyright (c) 2024 by Lu Xianfan.
 * @FilePath     : bsp_uart_fifo.c
 * @Author       : lxf
 * @Date         : 2024-10-21 16:14:42
 * @LastEditors  : flyyingpiggy2020 154562451@qq.com
 * @LastEditTime : 2024-10-22 17:25:03
 * @Brief        :
 * BL602拥有两个串口uart0,uart1，其中uart0用于log输出，不可用。
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
/*---------- variable ----------*/
/*---------- function ----------*/
#if UART1_FIFO_EN == 1
static int __uart1_tx_callback(void *p_arg)
{
    return 0;
}

static int __uart1_rx_callback(void *p_arg)
{
    int ret;
    uint8_t data_buf[32];
    hosal_uart_dev_t *p_dev = (hosal_uart_dev_t *)p_arg;
    ret = hosal_uart_receive(p_dev, data_buf, sizeof(data_buf));
    if (ret > 0) {
        hosal_uart_send(p_dev, data_buf, ret);
    }
    return ret;
}
#endif

#if UART2_FIFO_EN == 1
static int __uart2_tx_callback(void *p_arg)
{
    return 0;
}

static int __uart2_rx_callback(void *p_arg)
{
    int ret;
    uint8_t data_buf[32];
    hosal_uart_dev_t *p_dev = (hosal_uart_dev_t *)p_arg;
    ret = hosal_uart_receive(p_dev, data_buf, sizeof(data_buf));
    if (ret > 0) {
        hosal_uart_send(p_dev, data_buf, ret);
    }
    return ret;
}
#endif

void bsp_InitUart(void)
{
#if UART1_FIFO_EN == 1
    hosal_uart_init(&uart_dev_int1);
    hosal_uart_ioctl(&uart_dev_int1, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_INT);
    hosal_uart_callback_set(&uart_dev_int1, HOSAL_UART_RX_CALLBACK, __uart1_rx_callback, &uart_dev_int1);
    hosal_uart_callback_set(&uart_dev_int1, HOSAL_UART_TX_CALLBACK, __uart1_tx_callback, &uart_dev_int1);
    hosal_uart_ioctl(&uart_dev_int1, HOSAL_UART_TX_TRIGGER_ON, NULL);
#endif

#if UART2_FIFO_EN == 1
    printf("uart2 init\n");
    hosal_uart_init(&uart_dev_int2);
    hosal_uart_ioctl(&uart_dev_int2, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_INT);
    hosal_uart_callback_set(&uart_dev_int2, HOSAL_UART_RX_CALLBACK, __uart2_rx_callback, &uart_dev_int2);
    hosal_uart_callback_set(&uart_dev_int2, HOSAL_UART_TX_CALLBACK, __uart2_tx_callback, &uart_dev_int2);
    hosal_uart_ioctl(&uart_dev_int2, HOSAL_UART_TX_TRIGGER_ON, NULL);
#endif
    return;
}

int32_t comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen)
{
    hosal_uart_dev_t *p_dev = NULL;
    if (_ucPort == COM1) {
#if UART1_FIFO_EN == 1
        p_dev = &uart_dev_int1;
        return hosal_uart_send(p_dev, _ucaBuf, _usLen);
#endif
    } else {
#if UART2_FIFO_EN == 1
        p_dev = &uart_dev_int2;
        return hosal_uart_send(p_dev, _ucaBuf, _usLen);
#endif
    }
    return 0;
}

uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte)
{
    return 0;
}

uint16_t comGetBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen)
{
    return 0;
}
/*---------- end of file ----------*/
