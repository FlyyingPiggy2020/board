/*
 * Copyright (c) 2024 by Lu Xianfan.
 * @FilePath     : bsp_led.c
 * @Author       : lxf
 * @Date         : 2024-11-26 13:18:49
 * @LastEditors  : FlyyingPiggy2020 154562451@qq.com
 * @LastEditTime : 2024-11-29 11:08:54
 * @Brief        :
 */

/*---------- includes ----------*/
#include "bsp.h"
/*---------- macro ----------*/
#if (CONFIG_BSP_LED_NUM >= 1)
#define HARD_LED_NUM CONFIG_BSP_LED_NUM /* 实体灯个数 */

/*---------- type define ----------*/
/*---------- variable prototype ----------*/
/* 依次定义GPIO */
typedef struct {
    GPIO_TypeDef *gpio;
    uint32_t pin;
} L_GPIO_T;

static L_GPIO_T l_gpio_list[HARD_LED_NUM] = { 0 };
/*---------- function prototype ----------*/
/*---------- variable ----------*/
/*---------- function ----------*/

/**
 * @brief LED初始化
 * @return {*}
 */
void bsp_InitLed(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    uint8_t i;

    for (i = 0; i < HARD_LED_NUM; i++) {
        bsp_LedOff(i);
    }
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   /* 设置推挽输出 */
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;         /* 上下拉电阻不使能 */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; /* GPIO速度等级 */

    _translate_pin_name(CONFIG_BSP_LED1_IO, &l_gpio_list[0].gpio, &l_gpio_list[0].pin);

#if (CONFIG_BSP_LED_NUM >= 2)
    _translate_pin_name(CONFIG_BSP_LED2_IO, &l_gpio_list[1].gpio, &l_gpio_list[1].pin);
#endif
    for (i = 0; i < HARD_LED_NUM; i++) {
        GPIO_InitStruct.Pin = l_gpio_list[i].pin;
        HAL_GPIO_Init(l_gpio_list[i].gpio, &GPIO_InitStruct);
    }
}

/**
 * @brief 点亮指定的LED指示灯。
 * @param {uint8_t} _no
 * @return {*}
 */
void bsp_LedOn(uint8_t _no)
{
    if (_no < HARD_LED_NUM) {
        l_gpio_list[_no].gpio->BSRR = l_gpio_list[_no].pin << 16U;
    }
}

/**
 * @brief 熄灭指定的LED指示灯。
 * @param {uint8_t} _no
 * @return {*}
 */
void bsp_LedOff(uint8_t _no)
{
    if (_no < HARD_LED_NUM) {
        l_gpio_list[_no].gpio->BSRR = (uint32_t)l_gpio_list[_no].pin;
    }
}

/**
 * @brief 翻转指定的LED指示灯。
 * @param {uint8_t} _no
 * @return {*}
 */
void bsp_LedToggle(uint8_t _no)
{
    if (_no < HARD_LED_NUM) {
        l_gpio_list[_no].gpio->ODR ^= l_gpio_list[_no].pin;
    }
}

/**
 * @brief 判断LED指示灯是否已经点亮。
 * @param {uint8_t} _no
 * @return {*}
 */
uint8_t bsp_IsLedOn(uint8_t _no)
{
    if (_no < HARD_LED_NUM) {
        if ((l_gpio_list[_no].gpio->ODR & l_gpio_list[_no].pin) != 0) {
            return 1;
        }
        return 0;
    }

    return 0;
}
/*---------- end of file ----------*/
#endif
