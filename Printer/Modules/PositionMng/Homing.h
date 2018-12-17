/********************** (C) COPYRIGHT 2016 DediBot ******************************
  * @file    Homing.h
  * @author  Dedibot
  * @version V
  * @date    2017-05-24
  * @brief   回归原点
*********************************************************************************/
#ifndef HOMING_H
#define HOMING_H
#include "de_type.h"
#include "string.h"
#include "planner.h"
#include "stepper.h"

typedef enum
{
	STAGE_STANDBY,
	STAGE_START,
	STAGE_MOVING,
	STAGE_BACKWARD,
	STAGE_CREEPING,
	STAGE_FINISHED
}HOMING_STAGE;

typedef struct
{
	HOMING_STAGE stage;
	
	int homingAxesCount;
	enum _AXIS *homingAxes;
	
	float homeMaxDis[AXIS_MAX];
	float homeBackDest[AXIS_MAX];
	float homeCreepDest[AXIS_MAX];
	float homeBasePos[AXIS_MAX];
	
	PLANNER *homingPlanner;
	MOVE_MODULE *homingMoveModule;
	
	u8	 m_LimitState[AXIS_MAX];
}HOMING_MODULE;



extern void Homing_Init(HOMING_MODULE *this, PLANNER *planner, MOVE_MODULE *moveModule);
extern void Homing_SetPlanner(HOMING_MODULE *this, PLANNER *planner);
extern void Homing_SetMoveModule(HOMING_MODULE *this, MOVE_MODULE *moveModule);
extern void Homing_Start(HOMING_MODULE *this, int axesCount, enum _AXIS *axes);
extern void Homing_Exit(HOMING_MODULE *this);
extern bool Homing_IsFinished(HOMING_MODULE *this);
extern void Homing_Run(HOMING_MODULE *this);

#endif/* HOMING_H */
