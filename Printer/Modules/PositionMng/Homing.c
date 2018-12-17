/********************** (C) COPYRIGHT 2016 DediBot ******************************
  * @file    Position.c
  * @author  Dedibot
  * @version V
  * @date    2016-10-28
  * @brief   位置控制
*********************************************************************************/
#include "Homing.h"
#include "PositionMng.h"
#include "math.h"
#include "stepper.h"
#include "ErrorMng.h"
/* 设计打印高度 */
#define PRINT_SIZE_X			1000	
#define PRINT_SIZE_Y			240	
#define PRINT_SIZE_Z			160	


/* 回归原点相关宏定义 */
//60*60的速度在回归原点的时候，没有撞击声
#define HOMING_MOVE_SPEED 		(30) 	// 朝原点快速移动的速度
#define HOMING_BACK_SPEED 		(20) 	// 遇到限位开关后往后回退一段距离的速度
#define HOMING_CREEP_SPEED 		(5) 		// 慢速靠向限位开关的速度
#define HOMING_CREEP_DIS		(5)			//5mm
#define HOMING_DEVIATION		(10)		//朝原点移动时，多移动10mm


/*******************************************
	内部变量
********************************************/			

/*******************************************
	外部变量
********************************************/

/*******************************************
	内部函数声明
********************************************/

/*******************************************
	内部函数定义
********************************************/
/******************************************
功能：
作成者：chen-lj
作成日期：
参数:	
返回值:	
*******************************************/
void Set_HomingPos(HOMING_MODULE *this)
{
	int i = 0;
	
	for(i = 0; i < this->homingAxesCount; i++)
	{
		this->homeMaxDis[i] = PRINT_SIZE_X+HOMING_DEVIATION;
		this->homeBasePos[i] = 0;
		this->homeBackDest[i] = HOMING_CREEP_DIS;
		this->homeCreepDest[i] = -2*HOMING_CREEP_DIS;
	}
}
// void Get_LimitState()
// {
	// m_LimitState[X_AXIS] = IO_GetInputPortState( INPUT_X_LIMIT );
	// m_LimitState[Y_AXIS] = IO_GetInputPortState( INPUT_Y_LIMIT );
	// m_LimitState[Z_AXIS] = IO_GetInputPortState( INPUT_Z_LIMIT );
// //	m_LimitState[E_AXIS] = IO_GetInputPortState( X_MAX_PIN );
// }

// void Limit_Check(u8 rightstate)
// {
	// u8 i;
	// Get_LimitState();
	// for(i = 0 ;i < Z_AXIS ;i++)
	// {
		// if(m_LimitState[i] != rightstate)
		// {
			// ErrorMng_SetError((ERROR_LIMIT_X ));
		// }
	// }	
// }
/*******************************************
	外部函数定义
********************************************/
/******************************************
功能：
作成者：chen-lj
作成日期：
参数:	
返回值:	
*******************************************/
void Homing_Start(HOMING_MODULE *this, int axesCount, enum _AXIS *axes)
{
	this->homingAxesCount = axesCount;
	this->homingAxes = axes;
	Set_HomingPos(this);
	// Stepper_EndStops_SetFlag(this->homingMoveModule, ENDSTOP_ENABLED);

	this->stage = STAGE_START;
}

/******************************************
功能：
作成者：chen-lj
作成日期：
参数:	
返回值:	
*******************************************/
void Homing_Exit(HOMING_MODULE *this)
{
	Stepper_QuickStop(this->homingMoveModule);
	// Stepper_EndStops_SetFlag(this->homingMoveModule, ENDSTOP_DISABLED);
	this->stage = STAGE_STANDBY;
}


/******************************************
功能：
作成者：chen-lj
作成日期：
参数:	
返回值:	
*******************************************/
bool Homing_IsFinished(HOMING_MODULE *this)
{
	return (bool)(this->stage == STAGE_FINISHED);
}

/******************************************
功能：
作成者：chen-lj
作成日期：
参数:	
返回值:	
*******************************************/
void Homing_Run(HOMING_MODULE *this)
{
	POSPARAM posParam;
	switch(this->stage)
	{
		case STAGE_START:
			if(False != Stepper_Is_MoveFinished(this->homingMoveModule))
			{
				Stepper_QuickStop(this->homingMoveModule);
				
				posParam.axesCount = this->homingAxesCount;
				posParam.axes = this->homingAxes;
				posParam.position = this->homeMaxDis;
				Planner_Set_Position(this->homingPlanner, &posParam);
				Stepper_Set_Position(this->homingMoveModule, &posParam);

				posParam.axesCount = this->homingAxesCount;
				posParam.axes = this->homingAxes;
				posParam.position = this->homeBasePos;
				Planner_Buffer_Line(this->homingPlanner, &posParam, NULL, HOMING_MOVE_SPEED);
				this->stage = STAGE_MOVING;
			}
			break;
		case STAGE_MOVING:
			if(False != Stepper_Is_MoveFinished(this->homingMoveModule))
			{
				//todo 限位开关检测
				//Limit_Check(HIGH);
				posParam.axesCount = this->homingAxesCount;
				posParam.axes = this->homingAxes;
				posParam.position = this->homeBackDest;
				Planner_Buffer_Line(this->homingPlanner, &posParam, NULL, HOMING_BACK_SPEED);
				this->stage = STAGE_BACKWARD;
			}
			break;
		case STAGE_BACKWARD:
			if(False != Stepper_Is_MoveFinished(this->homingMoveModule))
			{
				//Limit_Check(LOW);				
				posParam.axesCount = this->homingAxesCount;
				posParam.axes = this->homingAxes;
				posParam.position = this->homeCreepDest;
				Planner_Buffer_Line(this->homingPlanner, &posParam, NULL, HOMING_CREEP_SPEED);
				this->stage = STAGE_CREEPING;
			}
			break;
		case STAGE_CREEPING:
			if(False != Stepper_Is_MoveFinished(this->homingMoveModule))
			{
				//Limit_Check(HIGH);	
				this->stage = STAGE_FINISHED;
			}
			break;
		case STAGE_STANDBY:
		case STAGE_FINISHED:
		default:
			break;
	}
}

void Homing_Init(HOMING_MODULE *this, PLANNER *planner, MOVE_MODULE *moveModule)
{
	this->homingPlanner = planner;
	this->homingMoveModule = moveModule;
}

/******************************************
功能：
作成者：chen-lj
作成日期：
参数:	
返回值:	
*******************************************/
void Homing_SetPlanner(HOMING_MODULE *this, PLANNER *planner)
{
	this->homingPlanner = planner;
}

/******************************************
功能：
作成者：chen-lj
作成日期：
参数:	
返回值:	
*******************************************/
void Homing_SetMoveModule(HOMING_MODULE *this, MOVE_MODULE *moveModule)
{
	this->homingMoveModule = moveModule;
}







