/********************** (C) COPYRIGHT 2016 DediBot ******************************
  * @file    Position.c
  * @author  Dedibot
  * @version V
  * @date    2016-10-28
  * @brief   位置控制
*********************************************************************************/
#include "PositionMng.h"
#include "math.h"
#include "stdlib.h"
#include "DataParse.h"
#include "stdio.h"							//系统函数库
#include "string.h"							//系统函数库
#include "homing.h"
#include "planner.h"
#include "stepper.h"
#include "FDM_config.h"
#include "ErrorMng.h"




/*******************************************
	内部变量
********************************************/

/*******************************************
	外部变量
********************************************/

/*******************************************
	内部函数声明
********************************************/
static void PositionMng_CheckError(void);
static void PositionMng_SynParse(POSITIONMNG *this);
/*******************************************
	内部函数定义
********************************************/
void PositionMng_SynParse(POSITIONMNG *this)
{
	int i;
	
	for(i = 0; i < this->mainAxesCount; i++)
	{
		this->mainAxesPos[i] = this->localScalePos[this->mainAxesIndex[i]];
	}
	for(i = 0; i < this->extruderAxesCount; i++)
	{
		this->extruderAxesPos[i] = this->localScalePos[this->extruderAxesIndex[i]];
	}
	for(i = 0; i < this->freeAxesCount; i++)
	{
		this->freeAxesPos[i] = this->localScalePos[this->freeAxesIndex[i]];
	}
}
void PositionMng_SynOffset(POSITIONMNG *this)
{
	int i;
	
	for(i = 0; i < this->totalAxesCount; i++)
	{
		if(this->newPosFlag[i])
		{
			this->localScalePos[i] = this->localScalePos[i] + this->axesOffset[i];
		}
	}
}

/******************************************
功能：将字符串转换成浮点数,算出动作的目标位置
作成者：cao-dl
作成日期：2014/11/19
参数:无
返回值:无
*******************************************/
void PositionMng_Get_Coordinates(POSITIONMNG *this, char* gcode, float *outPos, float *outFeedrate)
{
	char *endptr;
	int i, cmpIdx, schIdx, schIdx2;
	int len = strlen(gcode);
	int size = len+1;
	char cmpStr[10];
	u8 index[AXIS_MAX] = {0};
	
	memset(this->newPosFlag, 0, sizeof(this->newPosFlag));
	
	for(schIdx = 0; schIdx < len; schIdx++)
	{
		for(i = 0; i < g_AxisCodesSize; i++)
		{
			if(gcode[schIdx] == g_AxisCodes[i])
			{
				cmpStr[0] = gcode[schIdx];
				schIdx++;
				for(schIdx2 = schIdx; schIdx2 < size; schIdx2++)
				{
					if(gcode[schIdx2] == '_')
					{
						memcpy(&cmpStr[1], &gcode[schIdx], schIdx2-schIdx);
						cmpStr[schIdx2-schIdx+1] = '\0';
						schIdx = schIdx2+1;
						break;
					}
					else if((gcode[schIdx2] < '0') || (gcode[schIdx2] > '9'))
					{
						cmpStr[1] = '\0';
						break;
					}
				}
				for(cmpIdx = 0; cmpIdx < this->totalAxesCount; cmpIdx++)
				{
					if(0 == strcmp(cmpStr, &this->stringsForParse[cmpIdx*PARSE_STRING_SIZE]))
					{
						this->newPosFlag[cmpIdx] = 1;
						outPos[cmpIdx] = (float)strtod(&gcode[schIdx],&endptr);
						// break;
					}
				}
				// while(	((gcode[schIdx] >= '0') && (gcode[schIdx] <= '9')) || 
						// (gcode[schIdx] == '.') || (gcode[schIdx] == '-') || (gcode[schIdx] == '+'))
				// {
					// schIdx++;
				// }
				break;
			}
		}
	}
	for(i = 0; i < this->totalAxesCount; i++)
	{
		if(this->newPosFlag[i] != 1)
		{
			outPos[i] = Planner_Get_AxisPosition(this->axes[i]);
		}
	}
	
	//确定出料速度
	DataParse_code_seen_chr(gcode, 'F', outFeedrate );
}
/******************************************
功能：检测打印中异常
作成者：chen-lj
作成日期：2016/11/10
参数：无
返回值：无
*******************************************/
void PositionMng_CheckError(void)
{
	// enum _AXIS axis;
	// float tem[] = MAXMOVELOCATION;
	// float axis_location;
	// for(axis = X_AXIS ;axis <= Z_AXIS ;axis++)
	// {
		// axis_location = Stepper_Get_Axis_Position_mm(axis);
		// if(axis_location > tem[axis])
		// {
			// ErrorMng_SetError(ERROR_POSITION_OUT);	
		// }
	// }
}

/*******************************************
	外部函数
********************************************/
/******************************************
功能：初始化
作成者：chen-lj
作成日期：2016/10/28
参数：无
返回值：无
*******************************************/
void PositionMng_Init_1(POSITIONMNG *this)
{
	this->stage = STAGE_STANDARD;
	
	this->feedRate = 0;

	this->totalAxesCount = g_TotalAxesCount_1;
	this->stringsForParse_Origin = (char *)g_AxisCodes_1_Origin;
	this->stringsForParse = (char *)g_AxisCodes_1;
	this->axes = (enum _AXIS *)g_Axes_1;
	this->axesOffset = (long *)g_AxesOffset_1;
	memset(this->localScalePos, 0, sizeof(float)*g_TotalAxesCount_1);
	
	/* 移动轴 */
	this->mainAxesCount = g_MainAxesCount_1;
	this->mainAxesIndex = (u8 *)g_MainAxesIndex_1;
	this->mainAxes = (enum _AXIS *)g_MainAxes_1;
	memset(this->mainAxesPos, 0, sizeof(float)*g_MainAxesCount_1);
	
	this->mainPosParam.axesCount = g_MainAxesCount_1;
	this->mainPosParam.axes = this->mainAxes;
	this->mainPosParam.position = this->mainAxesPos;
	
	/* 挤料轴 */
	this->extruderAxesCount = g_ExtruderAxesCount_1;
	this->extruderAxesIndex = (u8 *)g_ExtruderAxesIndex_1;
	this->extruderAxes = (enum _AXIS *)g_ExtruderAxes_1;
	memset(this->extruderAxesPos, 0, sizeof(float)*g_ExtruderAxesCount_1);
	
	this->extruderPosParam.axesCount = g_ExtruderAxesCount_1;
	this->extruderPosParam.axes = this->extruderAxes;
	this->extruderPosParam.position = this->extruderAxesPos;
	
	/* 自由轴 */
	this->freeAxesCount = g_FreeAxesCount_1;
	this->freeAxesIndex = (u8 *)g_FreeAxesIndex_1;
	this->freeAxes = (enum _AXIS *)g_FreeAxes_1;
	memset(this->freeAxesPos, 0, sizeof(float)*g_FreeAxesCount_1);
	
	this->freePosParam.axesCount = 1;
	
	/* 线性插补运动模块 */
	this->mainPlanner = &g_MainPlanner_1;
	this->mainMoveModule = g_MainMoveModule_1;
	this->mainPlanner->block_Mnger = &this->mainMoveModule->block_Mnger;
	Stepper_EndStops_SetFlag(this->mainMoveModule, ENDSTOP_ENABLED);
	
	/* 自由轴运动模块 */
	this->freePlanner = &g_FreePlanner;
	this->freeMoveModule = g_FreeMoveModule;
	this->freePlanner->block_Mnger = &this->freeMoveModule->block_Mnger;

	/* 回原点模块 */
	this->homingAxesCount = g_HomingAxesCount_1;
	this->homingAxes = (enum _AXIS *)g_HomingAxes_1;
	this->subhomingAxesCount = g_subHomingAxesCount_1;
	this->subhomingAxes = (enum _AXIS *)g_subHomingAxes_1;
	this->setHomeGcode = (char **)g_SetHomeGcode_1;
	
	PositionMng_Clear(this);
}
/******************************************
功能：初始化
作成者：chen-lj
作成日期：2016/10/28
参数：无
返回值：无
*******************************************/
void PositionMng_Init_2(POSITIONMNG *this)
{
	this->stage = STAGE_STANDARD;
	
	this->feedRate = 0;

	this->totalAxesCount = g_TotalAxesCount_2;
	this->stringsForParse_Origin = (char *)g_AxisCodes_2_Origin;
	this->stringsForParse = (char *)g_AxisCodes_2;
	this->axes = (enum _AXIS *)g_Axes_2;
	this->axesOffset = (long *)g_AxesOffset_2;
	memset(this->localScalePos, 0, sizeof(float)*g_TotalAxesCount_2);
	
	/* 移动轴 */
	this->mainAxesCount = g_MainAxesCount_2;
	this->mainAxesIndex = (u8 *)g_MainAxesIndex_2;
	this->mainAxes = (enum _AXIS *)g_MainAxes_2;
	memset(this->mainAxesPos, 0, sizeof(float)*g_MainAxesCount_2);
	
	this->mainPosParam.axesCount = g_MainAxesCount_2;
	this->mainPosParam.axes = this->mainAxes;
	this->mainPosParam.position = this->mainAxesPos;
	
	/* 挤料轴 */
	this->extruderAxesCount = g_ExtruderAxesCount_2;
	this->extruderAxesIndex = (u8 *)g_ExtruderAxesIndex_2;
	this->extruderAxes = (enum _AXIS *)g_ExtruderAxes_2;
	memset(this->extruderAxesPos, 0, sizeof(float)*g_ExtruderAxesCount_2);
	
	this->extruderPosParam.axesCount = g_ExtruderAxesCount_2;
	this->extruderPosParam.axes = this->extruderAxes;
	this->extruderPosParam.position = this->extruderAxesPos;
	
	/* 自由轴 */
	this->freeAxesCount = g_FreeAxesCount_2;
	this->freeAxesIndex = (u8 *)g_FreeAxesIndex_2;
	this->freeAxes = (enum _AXIS *)g_FreeAxes_2;
	memset(this->freeAxesPos, 0, sizeof(float)*g_FreeAxesCount_2);
	
	this->freePosParam.axesCount = 1;
	
	/* 线性插补运动模块 */
	this->mainPlanner = &g_MainPlanner_2;
	this->mainMoveModule = g_MainMoveModule_2;
	this->mainPlanner->block_Mnger = &this->mainMoveModule->block_Mnger;
	Stepper_EndStops_SetFlag(this->mainMoveModule, ENDSTOP_ENABLED);
	
	/* 自由轴运动模块 */
	this->freePlanner = &g_FreePlanner;
	this->freeMoveModule = g_FreeMoveModule;
	this->freePlanner->block_Mnger = &this->freeMoveModule->block_Mnger;

	/* 回原点模块 */
	this->homingAxesCount = g_HomingAxesCount_2;
	this->homingAxes = (enum _AXIS *)g_HomingAxes_2;
	this->subhomingAxesCount = g_subHomingAxesCount_2;
	this->subhomingAxes = (enum _AXIS *)g_subHomingAxes_2;
	this->setHomeGcode = (char **)g_SetHomeGcode_2;
	
	PositionMng_Clear(this);
}
/******************************************
功能：初始化
作成者：chen-lj
作成日期：2016/10/28
参数：无
返回值：无
*******************************************/
void PositionMng_Init_3(POSITIONMNG *this)
{
	this->stage = STAGE_STANDARD;
	
	this->feedRate = 0;

	this->totalAxesCount = g_TotalAxesCount_3;
	this->stringsForParse_Origin = (char *)g_AxisCodes_3_Origin;
	this->stringsForParse = (char *)g_AxisCodes_3;
	this->axes = (enum _AXIS *)g_Axes_3;
	this->axesOffset = (long *)g_AxesOffset_3;
	memset(this->localScalePos, 0, sizeof(float)*g_TotalAxesCount_3);
	
	/* 移动轴 */
	this->mainAxesCount = g_MainAxesCount_3;
	this->mainAxesIndex = (u8 *)g_MainAxesIndex_3;
	this->mainAxes = (enum _AXIS *)g_MainAxes_3;
	memset(this->mainAxesPos, 0, sizeof(float)*g_MainAxesCount_3);
	
	this->mainPosParam.axesCount = g_MainAxesCount_3;
	this->mainPosParam.axes = this->mainAxes;
	this->mainPosParam.position = this->mainAxesPos;
	
	/* 挤料轴 */
	this->extruderAxesCount = g_ExtruderAxesCount_3;
	this->extruderAxesIndex = (u8 *)g_ExtruderAxesIndex_3;
	this->extruderAxes = (enum _AXIS *)g_ExtruderAxes_3;
	memset(this->extruderAxesPos, 0, sizeof(float)*g_ExtruderAxesCount_3);
	
	this->extruderPosParam.axesCount = g_ExtruderAxesCount_3;
	this->extruderPosParam.axes = this->extruderAxes;
	this->extruderPosParam.position = this->extruderAxesPos;
	
	/* 自由轴 */
	this->freeAxesCount = g_FreeAxesCount_3;
	this->freeAxesIndex = (u8 *)g_FreeAxesIndex_3;
	this->freeAxes = (enum _AXIS *)g_FreeAxes_3;
	memset(this->freeAxesPos, 0, sizeof(float)*g_FreeAxesCount_3);
	
	this->freePosParam.axesCount = 1;
	
	/* 线性插补运动模块 */
	this->mainPlanner = &g_MainPlanner_3;
	this->mainMoveModule = g_MainMoveModule_3;
	this->mainPlanner->block_Mnger = &this->mainMoveModule->block_Mnger;
	Stepper_EndStops_SetFlag(this->mainMoveModule, ENDSTOP_ENABLED);
	
	/* 自由轴运动模块 */
	this->freePlanner = &g_FreePlanner;
	this->freeMoveModule = g_FreeMoveModule;
	this->freePlanner->block_Mnger = &this->freeMoveModule->block_Mnger;

	/* 回原点模块 */
	this->homingAxesCount = g_HomingAxesCount_3;
	this->homingAxes = (enum _AXIS *)g_HomingAxes_3;
	this->subhomingAxesCount = g_subHomingAxesCount_3;
	this->subhomingAxes = (enum _AXIS *)g_subHomingAxes_3;
	this->setHomeGcode = (char **)g_SetHomeGcode_3;
	
	PositionMng_Clear(this);
}
/******************************************
功能：清除数据
作成者：chen-lj
作成日期：2016/10/28
参数：无
返回值：无
*******************************************/
void PositionMng_Clear(POSITIONMNG *this)
{
	int i;
	enum _AXIS axis;
	
	Stepper_QuickStop(this->mainMoveModule);
	for(i = 0; i < this->freeAxesCount; i++)
	{
		Stepper_QuickStop(&this->freeMoveModule[i]);
	}
	
	if(this->stage == STAGE_HOMING || this->stage == STAGE_SUBHOMING)
	{
		Homing_Exit(&this->homingModule);
	}
	this->stage = STAGE_STANDARD;
	
	memcpy(this->stringsForParse, this->stringsForParse_Origin, this->totalAxesCount*PARSE_STRING_SIZE);
	
	for(i = 0; i < this->totalAxesCount; i++)
	{
		axis = this->axes[i];
		this->localScalePos[i] = Stepper_Get_Axis_Position_mm(axis);
	}
	PositionMng_Set_Position(this, this->localScalePos, True);
		
	// for(i = 0; i < this->totalAxesCount; i++)
	// {
		// AxisCtrl_Disable(this->axes[i]);
	// }

	
//	Stepper_EndStops_SetFlag(ENDSTOP_DISABLED);
	// Stepper_EndStops_Hit_Clear(); 
}

/******************************************
功能：回归原点
作成者：chen-lj
作成日期：2016/10/28
参数：参数
返回值：无
*******************************************/
void PositionMng_MoveHome(POSITIONMNG *this)
{	
	Homing_Init(&this->homingModule, this->mainPlanner, this->mainMoveModule);
	Homing_Start(&this->homingModule, this->homingAxesCount, this->homingAxes);
	this->stage = STAGE_HOMING;
}

/******************************************
功能：XY轴回归原点
作成者：zheng-rh
作成日期：2017/10/12
参数：参数
返回值：无
*******************************************/
void PositionMng_MoveHome_XY(POSITIONMNG *this)
{	
	Homing_Init(&this->homingModule, this->mainPlanner, this->mainMoveModule);
	Homing_Start(&this->homingModule, this->subhomingAxesCount, this->subhomingAxes);
	this->stage = STAGE_SUBHOMING;
}


/******************************************
功能：用于第三个龙门架，Y轴单独回归原点
作成者：chen-lj
作成日期：2016/10/28
参数：参数
返回值：无
*******************************************/
void PositionMng_MoveHome_Y(POSITIONMNG *this, u8 select)
{	
	Homing_Init(&this->homingModule, this->freePlanner, this->freeMoveModule);
	Homing_Start(&this->homingModule, 1, &(this->mainAxes[select+1]));
	this->stage = STAGE_SUBHOMING;
}

/******************************************
功能：用于第三个龙门架，Y轴单独回归原点
作成者：chen-lj
作成日期：2016/10/28
参数：参数
返回值：无
*******************************************/
void PositionMng_Stop_Y(POSITIONMNG *this, u8 select)
{	
	this->stringsForParse[(select+1)*PARSE_STRING_SIZE] = '?';					//停止Y轴
	// AxisCtrl_Disable(this->axes[select+1]);
	Stepper_StopAxis(this->mainMoveModule, this->axes[select+1]);
	this->stringsForParse[(select+4)*PARSE_STRING_SIZE] = '?';					//停止E轴
	// AxisCtrl_Disable(this->axes[select+4]);
	Stepper_StopAxis(this->mainMoveModule, this->axes[select+4]);
}

/******************************************
功能：Delta坐标移动扫描
作成者：pang-w
作成日期：2016/11/3
参数:无
返回值:无
*******************************************/
void PositionMng_Pro(POSITIONMNG *this)
{
	switch(this->stage)
	{
		case STAGE_SUBHOMING:
			Homing_Run(&this->homingModule);
			if(False != Homing_IsFinished(&this->homingModule))
			{
				Homing_Exit(&this->homingModule);
				PositionMng_Set_Position_Gcode(this, this->setHomeGcode[1], True);
				this->stage = STAGE_STANDARD;
			}
			break;
		case STAGE_HOMING:
			Homing_Run(&this->homingModule);
			if(False != Homing_IsFinished(&this->homingModule))
			{
				Homing_Exit(&this->homingModule);
				PositionMng_Set_Position_Gcode(this, this->setHomeGcode[0], True);
				this->stage = STAGE_STANDARD;
			}
			break;
		case STAGE_STANDARD:
			PositionMng_CheckError();
			break;
		default:
			break;
	}
}
/******************************************
功能：是否能够移动
作成者：pang-w
作成日期：2017/02/08
参数:无
返回值:无
*******************************************/
bool PositionMng_CanMove(POSITIONMNG *this)
{
	if(this->stage != STAGE_HOMING && this->stage != STAGE_SUBHOMING)
	{
		if(!Stepper_Is_Full(this->mainMoveModule))
		{
			return True;
		}
	}
	return False;
}
/******************************************
功能：是否移动结束，planner缓冲区也移动结束
作成者：chen-lj
作成日期：2017/02/13
参数:无
返回值:无
*******************************************/
bool PositionMng_IsMoveFinished(POSITIONMNG *this)
{
	return (bool)((Stepper_Is_MoveFinished(this->mainMoveModule))? True: False);
}

/******************************************
功能：回归原点完成
作成者：chen-lj
作成日期：2016/11/08
参数：无
返回值：无
*******************************************/
bool PositionMng_IsHomingFinished( POSITIONMNG *this ) 
{
	return (bool)(this->stage != STAGE_HOMING && this->stage != STAGE_SUBHOMING);
}

/******************************************
功能：gcode并移动
作成者：chen-lj
作成日期：2017/02/07
参数：参数
返回值：无
*******************************************/
void PositionMng_PrepareMove_Position(POSITIONMNG *this, float *dest, float feedrate)
{
	int i;
	
	memcpy(this->localScalePos, dest, this->totalAxesCount * sizeof(float));
	PositionMng_SynOffset(this);
	PositionMng_SynParse(this);
	if(feedrate > 0)
	{
		this->feedRate = feedrate;
	}
	
	Planner_Buffer_Line(this->mainPlanner, &this->mainPosParam, &this->extruderPosParam, this->feedRate/60);
	
	for(i = 0; i < this->freeAxesCount; i++)
	{
		this->freePosParam.axes = &this->freeAxes[i];
		this->freePosParam.position = &this->freeAxesPos[i];
		Planner_Buffer_Line(this->freePlanner, &this->freePosParam, NULL, this->feedRate/60);
	}
}

/******************************************
功能：解析gcode并移动
作成者：chen-lj
作成日期：2017/02/07
参数：参数
返回值：无
*******************************************/
void PositionMng_PrepareMove_Gcode( POSITIONMNG *this, char *gcode) 
{
	float pos[AXIS_MAX];
	float feedrate;
	//获取目标移动距离
	PositionMng_Get_Coordinates( this, gcode, pos, &feedrate ); // For X Y Z E F

	PositionMng_PrepareMove_Position(this, pos, feedrate);
}

/******************************************
功能：解析gcode并设置当前位置
作成者：chen-lj
作成日期：2016/10/28
参数：参数
返回值：无
*******************************************/
void PositionMng_Set_Position(POSITIONMNG *this, float *pos, bool isAllSet) 
{
	int i;
	
	memcpy(this->localScalePos, pos, this->totalAxesCount * sizeof(float));
	PositionMng_SynParse(this);
	
	Planner_Set_Position(this->mainPlanner, &this->mainPosParam);
	Planner_Set_Position(this->mainPlanner, &this->extruderPosParam);
	if(isAllSet)
	{
		Stepper_Set_Position(this->mainMoveModule, &this->mainPosParam);
		Stepper_Set_Position(this->mainMoveModule, &this->extruderPosParam);
	}
	
	for(i = 0; i < this->freeAxesCount; i++)
	{
		this->freePosParam.axes = &this->freeAxes[i];
		this->freePosParam.position = &this->freeAxesPos[i];
		Planner_Set_Position(&this->freePlanner[i], &this->freePosParam);
		Stepper_Set_Position(&this->freeMoveModule[i], &this->freePosParam);
	}
}

/******************************************
功能：解析gcode并设置当前位置
作成者：chen-lj
作成日期：2016/10/28
参数：参数
返回值：无
*******************************************/
void PositionMng_Set_Position_Gcode( POSITIONMNG *this, char *gcode, bool isAllSet) 
{
	float pos[AXIS_MAX];
	float feedrate;
	//获取目标移动距离
	PositionMng_Get_Coordinates( this, gcode, pos, &feedrate ); // For X Y Z E F
	
	PositionMng_Set_Position(this, pos, isAllSet);
}

/******************************************
功能：
作成者：chen-lj
作成日期：
参数：参数
返回值：无
*******************************************/
void PositionMng_SetNormalMode(POSITIONMNG *this) 
{
	this->freePlanner = &g_FreePlanner;
	this->freeMoveModule = g_FreeMoveModule;
}

/******************************************
功能：
作成者：chen-lj
作成日期：
参数：参数
返回值：无
*******************************************/
void PositionMng_SetInfiniteMode(POSITIONMNG *this) 
{
	this->freePlanner = this->mainPlanner;
	this->freeMoveModule = this->mainMoveModule;
}




