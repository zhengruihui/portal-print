/********************** (C) COPYRIGHT 2011 STMicroelectronics ********************
  * @file    structs.h
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-03-25
  * @brief   �ṹ����
*********************************************************************************/
#ifndef STRUCTS_H
#define STRUCTS_H

#include "de_type.h"
#include "AxisCtrl.h"															//�������

typedef struct
{
	int axesCount;
	enum _AXIS *axes;
	float *position;
}POSPARAM;

#endif	/* STRUCTS_H */

