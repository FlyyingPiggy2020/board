/*
*********************************************************************************************************
*
*   模块名称 : 按键驱动模块
*   文件名称 : bsp_key.h
*   版    本 : V1.0
*   说    明 : 头文件
*
*   Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "stdint.h"
#define KEY_FILTER_TIME CONFIG_BSP_KEYFILTER_TIME /* 单位10ms, 滤波时长 */
#define KEY_LONG_TIME   CONFIG_BSP_KEY_LONG_TIME  /* 单位10ms， 认为长按事件 */

/*
    每个按键对应1个全局的结构体变量。
*/
typedef struct {
    /* 下面是一个函数指针，指向判断按键手否按下的函数 */
    uint8_t (*IsKeyDownFunc)(void); /* 按键按下的判断函数,1表示按下 */

    uint8_t Count;       /* 滤波器计数器 */
    uint16_t LongCount;  /* 长按计数器 */
    uint16_t LongTime;   /* 按键按下持续时间, 0表示不检测长按 */
    uint8_t State;       /* 按键当前状态（按下还是弹起） */
    uint8_t RepeatSpeed; /* 连续按键周期 */
    uint8_t RepeatCount; /* 连续按键计数器 */
} KEY_T;

typedef enum {
    KEY_NONE = 0, /* 0 表示按键事件 */
#if (CONFIG_BSP_HARD_KEY_NUM >= 1)
    KEY_1_DOWN, /* 1键按下 */
    KEY_1_UP,   /* 1键弹起 */
    KEY_1_LONG, /* 1键长按 */
#endif
} KEY_ENUM;

/* 按键FIFO用到变量 */
#define KEY_FIFO_SIZE 10
typedef struct {
    uint8_t Buf[KEY_FIFO_SIZE]; /* 键值缓冲区 */
    uint8_t Read;               /* 缓冲区读指针1 */
    uint8_t Write;              /* 缓冲区写指针 */
    uint8_t Read2;              /* 缓冲区读指针2 */
} KEY_FIFO_T;

/* 供外部调用的函数声明 */
void bsp_InitKey(void);
void bsp_KeyScan10ms(void);
void bsp_PutKey(uint8_t _KeyCode);
uint8_t bsp_GetKey(void);
uint8_t bsp_GetKey2(void);
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t _RepeatSpeed);
void bsp_ClearKey(void);

#endif
