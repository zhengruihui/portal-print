/********************** (C) COPYRIGHT 2017 DediBot ******************************
  * @file    ProtocolData.c
  * @author  Dedibot
  * @version V
  * @date    2017-05-24
  * @brief   通信数据保存
*********************************************************************************/
#include "configuration.h"


char g_ControlBuf[ONELINE_GCODE_MAXSIZE+1];

/* 龙门架选择 */
u8 g_PortalFrameSelect;

/* 打印文件编号选择 */
u8 g_GcodeFileSelect;

/* 第三龙门架Y轴选择 */
u8 g_PortalFrame_Y_Operation;






