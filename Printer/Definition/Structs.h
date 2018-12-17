/********************** (C) COPYRIGHT 2011 STMicroelectronics ********************
  * @file    structs.h
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-03-25
  * @brief   结构定义
*********************************************************************************/
#ifndef STRUCTS_H
#define STRUCTS_H

#include "de_type.h"
#include "AxisCtrl.h"															//电机控制

typedef struct
{
	int axesCount;
	enum _AXIS *axes;
	float *position;
}POSPARAM;

#endif	/* STRUCTS_H */

