/*
*********************************************************************************************************
*
*   模块名称 : LED指示灯驱动模块
*   文件名称 : bsp_led.c
*   版    本 : V1.0
*   说    明 : 驱动LED指示灯
*
*   ????? :
*       ?汾??  ????        ????     ???
*       V1.0    2019-04-24 armfly  ???????
*
*   Copyright (C), 2019-2030, ?????????? www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
    安富莱STM32-V5 开发板LED口线分配：
        LD1     : PB13          (低电平点亮，高电平熄灭)
*/

#define HARD_LED_NUM LED_IDX /* 实体灯个数 */

/* LED口对应的RCC时钟 */
#define LED_GPIO_CLK_ALLENABLE()                                               \
  {                                                                            \
    __HAL_RCC_GPIOA_CLK_ENABLE();                                              \
    __HAL_RCC_GPIOB_CLK_ENABLE();                                              \
  };

/* 依次定义GPIO */
typedef struct {
  GPIO_TypeDef *gpio;
  uint16_t pin;
} L_GPIO_T;

/* GPIO和PIN定义 */
static const L_GPIO_T l_gpio_list[HARD_LED_NUM] = {
    {GPIOB, GPIO_PIN_14}, /* Led1 */
};

/*
*********************************************************************************************************
*   函 数 名: bsp_InitLed
*   功能说明: 配置LED指示灯相关的GPIO,  该函数被 bsp_Init() 调用。
*   形    参:  无
*   返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitLed(void) {
  GPIO_InitTypeDef GPIO_InitStruct;
  uint8_t i;

  /* 打开GPIO时钟 */
  LED_GPIO_CLK_ALLENABLE();

  /*
      配置所有的LED指示灯GPIO为推挽输出模式
      由于将GPIO设置为输出时，GPIO输出寄存器的值缺省是0，因此会驱动LED点亮.
      这是我不希望的，因此在改变GPIO为输出前，先关闭LED指示灯
  */
  for (i = 0; i < HARD_LED_NUM; i++) {
    bsp_LedOff(i);
  }
  /* 配置LED */
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   /* 设置推挽输出 */
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;         /* 上下拉电阻不使能 */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; /* GPIO速度等级 */

  for (i = 0; i < HARD_LED_NUM; i++) {
    GPIO_InitStruct.Pin = l_gpio_list[i].pin;
    HAL_GPIO_Init(l_gpio_list[i].gpio, &GPIO_InitStruct);
  }
}

/*
*********************************************************************************************************
*   函 数 名: bsp_LedOn
*   功能说明: 点亮指定的LED指示灯。
*   形    参:  _no : 指示灯序号，范围 1 - 4
*   返 回 值: 无
*********************************************************************************************************
*/
void bsp_LedOn(uint8_t _no) {
  if (_no < HARD_LED_NUM) {
    l_gpio_list[_no].gpio->BSRR = l_gpio_list[_no].pin << 16U;
  }
}

/*
*********************************************************************************************************
*   函 数 名: bsp_LedOff
*   功能说明: 熄灭指定的LED指示灯。
*   形    参:  _no : 指示灯序号，范围 1 - 4
*   返 回 值: 无
*********************************************************************************************************
*/
void bsp_LedOff(uint8_t _no) {
  if (_no < HARD_LED_NUM) {
    l_gpio_list[_no].gpio->BSRR = (uint32_t)l_gpio_list[_no].pin;
  }
}

/*
*********************************************************************************************************
*   函 数 名: bsp_LedToggle
*   功能说明: 翻转指定的LED指示灯。
*   形    参:  _no : 指示灯序号，范围 1 - 4
*   返 回 值: 按键代码
*********************************************************************************************************
*/
void bsp_LedToggle(uint8_t _no) {
  if (_no < HARD_LED_NUM) {
    l_gpio_list[_no].gpio->ODR ^= l_gpio_list[_no].pin;
  }
}

/*
*********************************************************************************************************
*   函 数 名: bsp_IsLedOn
*   功能说明: 判断LED指示灯是否已经点亮。
*   形    参:  _no : 指示灯序号，范围 1 - 4
*   返 回 值: 1表示已经点亮，0表示未点亮
*********************************************************************************************************
*/
uint8_t bsp_IsLedOn(uint8_t _no) {
  if (_no < HARD_LED_NUM) {
    if ((l_gpio_list[_no].gpio->ODR & l_gpio_list[_no].pin) != 0) {
      return 1;
    }
    return 0;
  }

  return 0;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE)
 * *********************************/
