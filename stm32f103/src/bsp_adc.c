/*
 * Copyright (c) 2024 by Lu Xianfan.
 * @FilePath     : bsp_adc.c
 * @Author       : lxf
 * @Date         : 2024-11-27 10:18:55
 * @LastEditors  : FlyyingPiggy2020 154562451@qq.com
 * @LastEditTime : 2024-12-04 10:24:46
 * @Brief        :
 */

/*---------- includes ----------*/
#include "main.h"
#include "bsp.h"

/*---------- macro ----------*/
/*---------- type define ----------*/
typedef struct {
    char *gpio;
    uint32_t channel;
} channel_table_t;

/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/
/*---------- variable ----------*/
#if CONFIG_BSP_ADC_CH_MAX >= 1
static ADC_HandleTypeDef hadc;
static DMA_HandleTypeDef hdma_adc;
uint16_t AD_Values[CONFIG_BSP_ADC_FILTER_COUNT][CONFIG_BSP_ADC_CH_MAX];

const channel_table_t channel_table[] = {
    { "PA.00", ADC_CHANNEL_0 },
    { "PA.01", ADC_CHANNEL_1 },
    { "PA.02", ADC_CHANNEL_2 },
    { "PA.03", ADC_CHANNEL_3 },
    { "PA.04", ADC_CHANNEL_4 },
    { "PA.05", ADC_CHANNEL_5 },
    { "PA.06", ADC_CHANNEL_6 },
    { "PA.07", ADC_CHANNEL_7 },
    { "PB.00", ADC_CHANNEL_8 },
    { "PB.01", ADC_CHANNEL_9 },
    { "PB.02", ADC_CHANNEL_10 },
    { "PB.03", ADC_CHANNEL_11 },
    { "PB.04", ADC_CHANNEL_12 },
    { "PB.05", ADC_CHANNEL_13 },
    { "PB.06", ADC_CHANNEL_14 },
    { "PB.07", ADC_CHANNEL_15 },
};
#endif

/*---------- function ----------*/
#if CONFIG_BSP_ADC_CH_MAX >= 1
bool _translate_pin_name_to_adc_channel(const char *name, uint32_t *channel)
{
    uint32_t i;
    for (i = 0; i < sizeof(channel_table) / sizeof(channel_table[0]); i++) {
        if (strcmp(name, channel_table[i].gpio) == 0) {
            *channel = channel_table[i].channel;
            return true;
        }
    }
    return false;
}
#endif

void bsp_adc_init(void)
{
#if CONFIG_BSP_ADC_CH_MAX >= 1
    GPIO_TypeDef *io_port;
    uint32_t io_pin;
    /* 1.初始化DMA */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* 2.初始化GPIO */
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    __HAL_RCC_ADC1_CLK_ENABLE();

    _translate_pin_name(CONFIG_BSP_ADC1_CH1_IO, &io_port, &io_pin);
    GPIO_InitStruct.Pin = io_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(io_port, &GPIO_InitStruct);

    hdma_adc.Instance = DMA1_Channel1;
    hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc.Init.Mode = DMA_CIRCULAR;
    hdma_adc.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_adc) != HAL_OK) {
        Error_Handler();
    }
    __HAL_LINKDMA(&hadc, DMA_Handle, hdma_adc);

    ADC_ChannelConfTypeDef sConfig = { 0 };
    hadc.Instance = ADC1;
    hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc.Init.ContinuousConvMode = ENABLE;
    hadc.Init.DiscontinuousConvMode = DISABLE;
    hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc.Init.NbrOfConversion = 1;
    if (HAL_ADC_Init(&hadc) != HAL_OK) {
        Error_Handler();
    }

    uint32_t adc_channel = 0;
    /* ADC通道设置 */
    if (_translate_pin_name_to_adc_channel(CONFIG_BSP_ADC1_CH1_IO, &adc_channel) == true) {
        sConfig.Channel = adc_channel;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
        if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) {
            Error_Handler();
        }
    }
    HAL_ADCEx_Calibration_Start(&hadc);
    HAL_ADC_Start_DMA(&hadc, (uint32_t *)&AD_Values, CONFIG_BSP_ADC_FILTER_COUNT * CONFIG_BSP_ADC_CH_MAX);
#else
    return;
#endif
}

#if CONFIG_BSP_ADC_CH_MAX >= 1
/**
 * @brief This function handles DMA1 channel1 global interrupt.
 */
void DMA1_Channel1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_adc);
}
#endif

/**
 * @brief 求平均值
 * @param {uint8_t} ch
 * @return {*}
 */
uint16_t filter(uint8_t ch)
{
#if CONFIG_BSP_ADC_CH_MAX >= 1
    uint16_t sum = 0;
    uint8_t count;
    for (count = 0; count < CONFIG_BSP_ADC_FILTER_COUNT; count++) {
        sum += AD_Values[count][ch];
    }
    return sum / CONFIG_BSP_ADC_FILTER_COUNT;
#else
    return 0;
#endif
}

uint16_t filter_x(uint8_t ch)
{
#if CONFIG_BSP_ADC_CH_MAX >= 1
    static uint16_t max = 0;
    static uint16_t min = 0xffff;
    uint16_t sum = 0;
    uint8_t count;
    uint16_t tmp16;

    max = 0;
    min = 0xffff;

    for (count = 0; count < CONFIG_BSP_ADC_FILTER_COUNT; count++) {
        tmp16 = AD_Values[count][ch];
        sum += tmp16;

        if (tmp16 > max)
            max = tmp16;

        if (tmp16 < min)
            min = tmp16;
    }

    sum -= max;
    sum -= min;

    tmp16 = sum / (CONFIG_BSP_ADC_FILTER_COUNT - 2);

    return tmp16;
#else
    return 0;
#endif
}

uint16_t bsp_adc_getvalue(uint8_t ad_num)
{
#if CONFIG_BSP_ADC_CH_MAX >= 1
    switch (ad_num) {
        case AD_PD:
            return filter_x(ad_num);
        case I_TEST:
            return filter_x(ad_num);
        default:
            break;
    }
    return 0;
#else
    return 0;
#endif
}

/**
 * @brief 在1ms时间片中循环执行(嘀嗒中断)
 * @return {*}
 */
void PowerDown_Check_Poll(void)
{
#if CONFIG_BSP_USE_POWER_CHECK >= 1
    static uint16_t Time_PowerDown = 0;
    uint16_t tmp16 = bsp_adc_getvalue(AD_PD);
    if (tmp16 < 3200) {
        if (++Time_PowerDown > 10) {
        }
        if (Time_PowerDown > 40) {
            /* 1.关闭所有中断 */
            DISABLE_INT();
            /* 2.禁能电源引脚，使能WP引脚 */
            i2c_VCCDisable();
            i2c_WPDisable();
            /* 3.程序阻塞在这里 */
            while (1)
                ;
        }
    } else {
        Time_PowerDown = 0;
    }
#else
    return;
#endif
}
/*---------- end of file ----------*/
