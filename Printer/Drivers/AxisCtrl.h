/********************** (C) COPYRIGHT 2011 STMicroelectronics ********************
  * @file    AxisCtrl.c
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-03-25
  * @brief   电机控制
*********************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef AXIS_CTRL_H
#define AXIS_CTRL_H
#include "de_type.h"



//轴定义
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

//轴定义
enum _AXIS {
	X1_AXIS,	//龙门架1 X轴
	Y1_AXIS,	//龙门架1 Y轴
	Z1_AXIS,	//龙门架1 Z轴
	E1_AXIS,	//龙门架1 E轴
	
	X2_AXIS,	//龙门架2 X轴
	Y2_AXIS,    //龙门架2 Y轴
	Z2_AXIS,    //龙门架2 Z轴
	E2_AXIS_1,  //龙门架2 E1轴
	E2_AXIS_2,  //龙门架2 E2轴
	E2_AXIS_3,  //龙门架2 E3轴
	E2_AXIS_4,  //龙门架2 E4轴

	
	X3_AXIS,	//龙门架3 X轴
	Y3_AXIS,  //龙门架3 Y1轴
	Z3_AXIS,    //龙门架3 Z轴
	E3_AXIS,  //龙门架3 E1轴
	
	W_AXIS,		//流水线
	
	AXIS_MAX
};

/*******************************************
	输出接口
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
