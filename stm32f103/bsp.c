/*
*********************************************************************************************************
*
*   模块名称 : BSP模块(For STM32F407)
*   文件名称 : bsp.c
*   版    本 : V1.0
*   说    明 : 这是硬件底层驱动程序的主文件。每个c文件可以 #include "bsp.h"
*来包含所有的外设驱动模块。 bsp = Borad surport packet 板级支持包 修改记录 :
*       版本号  日期         作者       说明
*       V1.0    2018-07-29  Eric2013   正式发布
*
*   Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "stdbool.h"
#include "main.h"

/*
*********************************************************************************************************
*                                      函数声明
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*   函 数 名: bsp_Init
*   功能说明:
*初始化所有的硬件设备。该函数配置CPU寄存器和外设的寄存器并初始化一些全局变量。只需要调用一次
*   形    参：无
*   返 回 值: 无
*********************************************************************************************************
*/
void bsp_Init(void)
{
    bsp_InitTimer();
    bsp_InitUart(); /* 串口初始化 */
    #if (CONFIG_BSP_LED_NUM >= 1)
    bsp_InitLed();  /* 初始化LED */
    #endif
    bsp_InitI2C();  /* 初始化I2C */
    #if (CONFIG_BSP_HARD_KEY_NUM >= 1)
    bsp_InitKey();
    #endif
}

/*
*********************************************************************************************************
*   函 数 名: Error_Handler
*   形    参: file : 源代码文件名称。关键字 __FILE__ 表示源代码文件名。
*             line ：代码行号。关键字 __LINE__ 表示源代码行号
*   返 回 值: 无
*       bsp_Error_Handler(__FILE__, __LINE__);
*********************************************************************************************************
*/
void bsp_Error_Handler(char *file, uint32_t line)
{
    /*
        用户可以添加自己的代码报告源代码文件名和代码行号，比如将错误文件和行号打印到串口
        printf("Wrong parameters value: file %s on line %d\r\n", file, line)
    */

    /* 这是一个死循环，断言失败时程序会在此处死机，以便于用户查错 */
    if (line == 0) {
        return;
    }

    while (1) {
    }
}

/*
*********************************************************************************************************
*   函 数 名: bsp_Idle
*   功能说明: 空闲时执行的函数。一般主程序在for和while循环程序体中需要插入
*CPU_IDLE() 宏来调用本函数。
*            本函数缺省为空操作。用户可以添加喂狗、设置CPU进入休眠模式的功能。
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void bsp_Idle(void)
{
/* --- 喂狗 */
#if !DEBUG
    extern IWDG_HandleTypeDef hiwdg;
    HAL_IWDG_Refresh(&hiwdg);
#endif
}

/**
 * @brief 检查引脚字符串是否合法。例如：A.01
 * @param {char} *name
 * @return {*}
 */
static bool _check_pin_name_valid(const char *name)
{
    if (!(strlen(name) == 4 && name[1] == '.')) {
        return false;
    }

    if ((name[0] < 'A' || name[0] > 'D') || (name[2] < '0' || name[2] > '1') || (name[3] < '0' || name[3] > '9')) {
        return false;
    }

    uint8_t pinnumber = (name[2] - '0') * 10 + (name[3] - '0');
    if (pinnumber > 15) {
        return false;
    }
    return true;
}

static GPIO_TypeDef *gpio_table[] = {
    GPIOA,
    GPIOB,
    GPIOC,
    GPIOD,
};

/**
 * @brief 将字符串转换成GPIO,并且使能时钟
 * @param {char} *name
 * @param {UART_T} _uart
 * @return {*}
 */
bool _translate_pin_name(const char *name, GPIO_TypeDef **tx_port, uint32_t *tx_pin)
{
    if (!(_check_pin_name_valid(name))) {
        return false;
    }

    *tx_port = gpio_table[name[0] - 'A'];
    *tx_pin = (1 << ((uint8_t)((name[2] - '0') * 10 + (name[3] - '0'))));

    if (name[0] == 'A') {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    } else if (name[0] == 'B') {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    } else if (name[0] == 'C') {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    } else if (name[0] == 'D') {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }
    return true;
}
