/*
 * Copyright (c) 2024 by Lu Xianfan.
 * @FilePath     : bsp.c
 * @Author       : lxf
 * @Date         : 2024-10-22 14:01:20
 * @LastEditors  : flyyingpiggy2020 154562451@qq.com
 * @LastEditTime : 2024-10-26 11:20:11
 * @Brief        :
 */

/*---------- includes ----------*/

#include "bsp.h"
/*---------- macro ----------*/
/*---------- type define ----------*/
/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/
/*---------- variable ----------*/
/*---------- function ----------*/
int bsp_Init(void)
{
    bsp_InitUart(); /* 串口初始化 */
    return 0;
}

#if defined(CONF_FPSDK_ENABLE_EXPORT)
#include "export.h"
INIT_BOARD_EXPORT(bsp_Init);
#endif


/*---------- end of file ----------*/
