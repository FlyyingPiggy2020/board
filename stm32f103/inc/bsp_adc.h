/*
 * Copyright (c) 2024 by Lu Xianfan.
 * @FilePath     : bsp_adc.h
 * @Author       : lxf
 * @Date         : 2024-11-27 10:19:04
 * @LastEditors  : FlyyingPiggy2020 154562451@qq.com
 * @LastEditTime : 2024-12-04 09:46:29
 * @Brief        :
 */

#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__
/*---------- includes ----------*/
#include "stdint.h"

/*---------- macro ----------*/
#define AD_PD  0
#define I_TEST 1
/*---------- type define ----------*/
/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/
void bsp_adc_init(void);
uint16_t bsp_adc_getvalue(uint8_t ad_num);
void PowerDown_Check_Poll(void);
/*---------- end of file ----------*/
#endif // !__BSP_ADC_H__
