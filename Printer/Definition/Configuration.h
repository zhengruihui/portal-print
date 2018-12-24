#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "de_type.h"
#include "Calculate.h"

#define MAXMOVELOCATION	{600 ,600 ,600}			//?????

/************??????**************/
#define HEAT_HEAD_COUNT 2						//????
#define	HEAD_HEAD_TEMPRANGE						//????????

#define ONELINE_GCODE_MAXSIZE 			80		//??GCO????


/*****************************���������ÿ�ʼ***********************************************
���㷽��
���תһȦ��������p
���תһȦ�ƶ����룺s
ÿ������ľ��� = s / p

����/����/����
���ֳ���: k
�ݼ�ࣺl
���תһȦ��������p
ÿ������ľ��� = k * l / p

˿��
���תһȦ��������p
˿�����Ƽ�ࣺl
ÿ������ľ��� = l / p

*****************************????????******************** **************************/
#define PLUSE_PER_CIRCLE_X			(2500.0)
#define PLUSE_PER_CIRCLE_Y			(3200.0)
#define PLUSE_PER_CIRCLE_Z			(3200.0)
#define PLUSE_PER_CIRCLE_E			(3200.0)
#define PLUSE_PER_CIRCLE_W			(10000.0) 


#define DIS_PER_CIRCLE_X			(5.0)
#define DIS_PER_CIRCLE_Y			(32)
#define DIS_PER_CIRCLE_Z			(60)
#define DIS_PER_CIRCLE_E			(32) 
#define DIS_PER_CIRCLE_W			(191.268)

#define AXIS_STEPS_PER_UNIT_X		(PLUSE_PER_CIRCLE_X/DIS_PER_CIRCLE_X)
#define AXIS_STEPS_PER_UNIT_Y		(PLUSE_PER_CIRCLE_Y/DIS_PER_CIRCLE_Y)
#define AXIS_STEPS_PER_UNIT_Z		(PLUSE_PER_CIRCLE_Z/DIS_PER_CIRCLE_Z)
#define AXIS_STEPS_PER_UNIT_E		(PLUSE_PER_CIRCLE_E/DIS_PER_CIRCLE_E)
#define AXIS_STEPS_PER_UNIT_W		(PLUSE_PER_CIRCLE_W/DIS_PER_CIRCLE_W)


#define AXIS_STEPS_PER_UNIT    		{	AXIS_STEPS_PER_UNIT_X, AXIS_STEPS_PER_UNIT_Y, AXIS_STEPS_PER_UNIT_Z, AXIS_STEPS_PER_UNIT_E, \
											\
										AXIS_STEPS_PER_UNIT_X, AXIS_STEPS_PER_UNIT_Y, AXIS_STEPS_PER_UNIT_Z, AXIS_STEPS_PER_UNIT_E, \
										AXIS_STEPS_PER_UNIT_E, AXIS_STEPS_PER_UNIT_E, AXIS_STEPS_PER_UNIT_E, 	 \
											\
										AXIS_STEPS_PER_UNIT_X, AXIS_STEPS_PER_UNIT_Y, AXIS_STEPS_PER_UNIT_Z, 5*AXIS_STEPS_PER_UNIT_E, \
											\
										AXIS_STEPS_PER_UNIT_W \
									} 
/* ����������ٶ� mm/s */
#define AXIS_MAX_FEEDRATE			{	100, 100, 100, 100, \
										100, 100, 100, 100, 100, 100, 100,	 \
										140, 140, 140, 140, \
										140 \
									}
/* ��������޼��ٶ�	mm/s^2 */
#define AXIS_MAX_ACCELERATION		{	100, 200, 100, 200, \
										100, 100, 100, 100, 100, 100, 100, \
										1000, 1000, 1000, 1000, \
										1000 \
									}	
/* ��������޼��ٶ�	mm/s^2 */
#define AXIS_MAX_JERK				{	5.0, 5.0, 5.0, 5.0, \
										5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 	  \
										5.0, 5.0, 5.0, 5.0, \
										5.0 \
									}


#define MOTOR_MOMENT_X					1		
#define MOTOR_MOMENT_Y					1		
#define MOTOR_MOMENT_Z					1		
#define MOTOR_MOMENT_E					1		
#define MOTOR_MOMENT_E1_				1		

#define AXIS_RESISTANCE_X				10		
#define AXIS_RESISTANCE_Y				10		
#define AXIS_RESISTANCE_Z				10		
#define AXIS_RESISTANCE_E				10		
#define AXIS_RESISTANCE_E1_				10		

#define	_G								9.8
#define axis_angular_acc(axis)			(MOTOR_MOMENT_##axis/((AXIS_RESISTANCE_##axis/_G)*square(MOTOR_##axis##_PERIMETER_MM)))//????
#define axis_linear_acc(axis)			axis_angular_acc(axis)/(MOTOR_##axis##_PERIMETER_MM/PI/2)								//????


#define MOTOR_ANGULAR_ACC_X				
/*-------------------------------------------------------------*///#define PIDTEMP 1
#endif	  //CONFIGURATION_H

