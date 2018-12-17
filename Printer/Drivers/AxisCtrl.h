/********************** (C) COPYRIGHT 2011 STMicroelectronics ********************
  * @file    AxisCtrl.c
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-03-25
  * @brief   �������
*********************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef AXIS_CTRL_H
#define AXIS_CTRL_H
#include "de_type.h"



//�ᶨ��
enum _DRIVER {

	STEPPER_M1,
	STEPPER_M2,
	STEPPER_M3,
	STEPPER_M4,
	STEPPER_M5,
	STEPPER_M6,
	STEPPER7,
	STEPPER8,
	STEPPER9,
	STEPPER10,
	STEPPER11,
	STEPPER12,
	STEPPER13,
	STEPPER14,
	STEPPER15,
	
	DRIVER_MAX
};

//�ᶨ��
enum _AXIS {
	X1_AXIS,	//���ż�1 X��
	Y1_AXIS,	//���ż�1 Y��
	Z1_AXIS,	//���ż�1 Z��
	E1_AXIS,	//���ż�1 E��
	
	X2_AXIS,	//���ż�2 X��
	Y2_AXIS,    //���ż�2 Y��
	Z2_AXIS,    //���ż�2 Z��
	E2_AXIS_1,  //���ż�2 E1��
	E2_AXIS_2,  //���ż�2 E2��
	E2_AXIS_3,  //���ż�2 E3��
	E2_AXIS_4,  //���ż�2 E4��

	
	X3_AXIS,	//���ż�3 X��
	Y3_AXIS,  //���ż�3 Y1��
	Z3_AXIS,    //���ż�3 Z��
	E3_AXIS,  //���ż�3 E1��
	
	W_AXIS,		//��ˮ��
	
	AXIS_MAX
};

/*******************************************
	����ӿ�
********************************************/
extern void AxisCtrl_Init( void );
extern void AxisCtrl_All_Stop( void );

extern void AxisCtrl_Enable(enum _AXIS axis);
extern void AxisCtrl_Disable(enum _AXIS axis);
extern void AxisCtrl_SetDirFront(enum _AXIS axis);
extern void AxisCtrl_SetDirBack(enum _AXIS axis);
extern void AxisCtrl_Move(enum _AXIS axis);
extern void AxisCtrl_Stop(enum _AXIS axis);

#endif  //AXIS_CTRL_H
