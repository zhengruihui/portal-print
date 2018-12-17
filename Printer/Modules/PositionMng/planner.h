/********************** (C) COPYRIGHT 2016 DediBot ******************************
  * @file    Planner.h
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-07-07
  * @brief   电机控制算法-速度曲线计算
*********************************************************************************/
#ifndef PLANNER_H
#define PLANNER_H
#include "block.h"
#include "structs.h"


typedef struct
{
	BLOCK_MNGER *block_Mnger;
	
	float m_Previous_Nominal_Speed;											//上一条指令的设定速度
}PLANNER;


/*******************************************
	外部参考变量
********************************************/
extern float g_Max_Feedrate[AXIS_MAX];
extern float g_Axis_Steps_per_Unit[AXIS_MAX];
extern float g_MinimumFeedrate;
extern float g_MinTravelFeedrate;
extern float g_Acceleration;
extern float g_Retract_Acceleration;
extern float g_Travel_Acceleration;
extern float g_Max_jerk[AXIS_MAX];
extern unsigned long g_Max_Acceleration_Units_per_Sqr_Second[AXIS_MAX];
extern unsigned long g_Axis_Steps_per_Sqr_Second[AXIS_MAX];

/*******************************************
	外部函数
********************************************/
//初始化
extern void Planner_Init(void);
//计算移动指令数据并加入缓冲区
extern void Planner_Buffer_Line(PLANNER *this, POSPARAM *mainAxes, POSPARAM *extruderAxes, float feed_rate);
//各轴加速度上限单位转换 mm/s^2→step/s^2
extern void Planner_Reset_Acceleration_Rates(void);

extern void Planner_Set_Position(PLANNER *this, POSPARAM *posParam);
extern float Planner_Get_AxisPosition(enum _AXIS axis);


#endif // PLANNER_H
