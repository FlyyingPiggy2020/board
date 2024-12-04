/*
*********************************************************************************************************
*
*   模块名称 : 定时器模块
*   文件名称 : bsp_timer.h
*   版    本 : V1.3
*   说    明 : 头文件
*
*   Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

#include "stdint.h"
/*
    在此定义若干个软件定时器全局变量
    注意，必须增加__IO 即
   volatile，因为这个变量在中断和主程序中同时被访问，有可能造成编译器错误优化。
*/
#define TMR_ENABLE_US 1 /* 使能微秒 */
#define TMR_COUNT                                                            \
    16 /* 软件定时器的个数 （定时器ID范围 0 - (TMR_COUNT - 1)) \
        */

typedef enum {
    TMR_ID_SYS_RESET = 0,         /* led超时延时 */
    TMR_ID_MOBUS_REPEAT,          /* mobus超时延时 */
    TMR_ID_ZIG_THREAD,            /* zigbee线程延时 */
    TMR_ID_ZIG_TIMEOUT,           /* zigbee超时延时 */
    TMR_ID_UPG_TIMEOUT,           /* zigbee升级超时 */
    TMR_ID_CRYPOR_TIMEOUT,        /* 加密随机时间 */
    TMR_ID_UPLOAD_TIMEOUT,        /* 上报信息时间 */
    TMR_ID_RFMODE_TIMEOUT,        /* 搜索信息时间 */
    TMR_ID_PROTOCOL_TRANS_THREAD, /* 协议发送 */
    TMR_ID_LED_FLASH_THREAD,      /* LED闪烁 */
} TMR_CNT_ID;

/* 定时器宏定义 */
typedef enum {
    TIME_INTERVAL_1us = 0x00, // 1微妙
    TIME_INTERVAL_lms,        // 1毫秒
    TIME_INTERVAL_l00ms,      // 100毫秒
    TIME_INTERVAL_ls,         // 1秒
    TIME_INTERVAL_1min,       // 1分钟
} TIME_INTERVAL;

/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef enum {
    TMR_ONCE_MODE = 0, /* 一次工作模式 */
    TMR_AUTO_MODE = 1  /* 自动定时工作模式 */
} TMR_MODE_E;

/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef struct {
    volatile uint8_t Mode;     /* 计数器模式，1次性 */
    volatile uint8_t Flag;     /* 定时到达标志  */
    volatile uint32_t Count;   /* 计数器 */
    volatile uint32_t PreLoad; /* 计数器预装值 */
} SOFT_TMR;

/* 提供给其他C文件调用的函数 */
void bsp_InitTimer(void);
void bsp_DelayMS(uint32_t n);
void bsp_DelayUS(uint32_t n);
void bsp_StartTimer(uint8_t _id, uint32_t _period);
void bsp_StartAutoTimer(uint8_t _id, uint32_t _period);
void bsp_StopTimer(uint8_t _id);
uint8_t bsp_CheckTimer(uint8_t _id);
uint8_t bsp_CountTimer(uint8_t _id);
uint32_t bsp_RemainTimer(uint8_t _id);
int32_t bsp_GetRunTime(void);
int32_t bsp_CheckRunTime(int32_t _LastTime);
void bsp_SysTick_Handler(void);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE)
 * *********************************/
