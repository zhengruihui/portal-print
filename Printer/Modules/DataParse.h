/********************** (C) COPYRIGHT 2015 DediBot ******************************
  * @file    DataParse.h
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-03-28
  * @brief   异常管理
*********************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DATA_PARSE_H
#define DATA_PARSE_H
#include "de_type.h"
#include "PortalFrame.h"
#include "Print.h"

//要检查的字符是否在目标字符串里
extern bool DataParse_code_seen_chr( char *gcode, const char code, float *data );
extern bool DataParse_code_seen_str( char *gcode, const char* code, float *data );

//受信数据解析(G,M)
extern bool DataParse_G_M_Ctrl( PORTALFRAME *this, char*  );

#endif  //DATA_PARSE_H
