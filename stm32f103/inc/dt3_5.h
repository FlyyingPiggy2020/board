/*
 * Copyright (c) 2024 by Lu Xianfan.
 * @FilePath     : dt3_5.h
 * @Author       : lxf
 * @Date         : 2024-09-19 16:03:37
 * @LastEditors  : FlyyingPiggy2020 154562451@qq.com
 * @LastEditTime : 2024-09-19 16:04:07
 * @Brief        :
 */

#ifndef __DT35_H__

/*---------- includes ----------*/
/*---------- macro ----------*/
/*---------- type define ----------*/
/*---------- variable prototype ----------*/
/*---------- function prototype ----------*/

void bsp_InitHardTimer(void);
void bsp_StartHardTimer(unsigned char _CC, unsigned int _uiTimeOut, void *_pCallBack);
/*---------- end of file ----------*/
#endif // !__DT35_H__
