/********************** (C) COPYRIGHT 2015 DediBot ******************************
  * @file    DataParse.h
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-03-28
  * @brief   �쳣����
*********************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DATA_PARSE_H
#define DATA_PARSE_H
#include "de_type.h"
#include "PortalFrame.h"
#include "Print.h"

//Ҫ�����ַ��Ƿ���Ŀ���ַ�����
extern bool DataParse_code_seen_chr( char *gcode, const char code, float *data );
extern bool DataParse_code_seen_str( char *gcode, const char* code, float *data );

//�������ݽ���(G,M)
extern bool DataParse_G_M_Ctrl( PORTALFRAME *this, char*  );

#endif  //DATA_PARSE_H
