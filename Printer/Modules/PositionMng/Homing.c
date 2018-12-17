/********************** (C) COPYRIGHT 2016 DediBot ******************************
  * @file    Position.c
  * @author  Dedibot
  * @version V
  * @date    2016-10-28
  * @brief   λ�ÿ���
*********************************************************************************/
#include "Homing.h"
#include "PositionMng.h"
#include "math.h"
#include "stepper.h"
#include "ErrorMng.h"
/* ��ƴ�ӡ�߶� */
#define PRINT_SIZE_X			1000	
#define PRINT_SIZE_Y			240	
#define PRINT_SIZE_Z			160	


/* �ع�ԭ����غ궨�� */
//60*60���ٶ��ڻع�ԭ���ʱ��û��ײ����
#define HOMING_MOVE_SPEED 		(30) 	// ��ԭ������ƶ����ٶ�
#define HOMING_BACK_SPEED 		(20) 	// ������λ���غ��������һ�ξ�����ٶ�
#define HOMING_CREEP_SPEED 		(5) 		// ���ٿ�����λ���ص��ٶ�
#define HOMING_CREEP_DIS		(5)			//5mm
#define HOMING_DEVIATION		(10)		//��ԭ���ƶ�ʱ�����ƶ�10mm


/*******************************************
	�ڲ�����
********************************************/			

/*******************************************
	�ⲿ����
********************************************/

/*******************************************
	�ڲ���������
********************************************/

/*******************************************
	�ڲ���������
********************************************/
/******************************************
���ܣ�
�����ߣ�chen-lj
�������ڣ�
����:	
����ֵ:	
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
	�ⲿ��������
********************************************/
/******************************************
���ܣ�
�����ߣ�chen-lj
�������ڣ�
����:	
����ֵ:	
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
���ܣ�
�����ߣ�chen-lj
�������ڣ�
����:	
����ֵ:	
*******************************************/
void Homing_Exit(HOMING_MODULE *this)
{
	Stepper_QuickStop(this->homingMoveModule);
	// Stepper_EndStops_SetFlag(this->homingMoveModule, ENDSTOP_DISABLED);
	this->stage = STAGE_STANDBY;
}


/******************************************
���ܣ�
�����ߣ�chen-lj
�������ڣ�
����:	
����ֵ:	
*******************************************/
bool Homing_IsFinished(HOMING_MODULE *this)
{
	return (bool)(this->stage == STAGE_FINISHED);
}

/******************************************
���ܣ�
�����ߣ�chen-lj
�������ڣ�
����:	
����ֵ:	
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
				//todo ��λ���ؼ��
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
���ܣ�
�����ߣ�chen-lj
�������ڣ�
����:	
����ֵ:	
*******************************************/
void Homing_SetPlanner(HOMING_MODULE *this, PLANNER *planner)
{
	this->homingPlanner = planner;
}

/******************************************
���ܣ�
�����ߣ�chen-lj
�������ڣ�
����:	
����ֵ:	
*******************************************/
void Homing_SetMoveModule(HOMING_MODULE *this, MOVE_MODULE *moveModule)
{
	this->homingMoveModule = moveModule;
}







