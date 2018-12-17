/********************** (C) COPYRIGHT 2011 STMicroelectronics ********************
  * @file    Globals.h
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-03-25
  * @brief   全局变量定义
*********************************************************************************/
#ifndef GLOBALS_H
#define GLOBALS_H
#include "de_type.h"
#include "structs.h"															//电机控制
#include "FanCtrl.h"															//风扇控制
#include "TempCtrl.h"															//温度控制
#include "block.h"
#include "stepper.h"
#include "planner.h"
#include "input.h"

#define PARSE_STRING_SIZE	(5)

typedef enum
{
	EXTRUDER_1,
	EXTRUDER_2,
	EXTRUDER_3,
	EXTRUDER_4,
	EXTRUDER_5,
	EXTRUDER_6,
	
	EXTRUDER_MAX
}EXTRUDER_NUM;

typedef struct
{
	/* 温度相关 */
	enum TEMPCTRL_ITEMS	heatItem;
	
	float targetTempNeedWait;
	float targetTempNotNeedWait;
	float currentTargetTemp;
	
	/* 风扇 */
	_FAN headCooler;
	_FAN modelCooler;
	
	/* 倾斜角度 */
	float angle;
	
	u8 status;
	
}EXTRUDER_STRU;

extern const enum _INPUT g_Axis_Limit_Tbl[AXIS_MAX];

extern EXTRUDER_STRU g_ExtruderStructs[EXTRUDER_MAX];

extern MOVE_MODULE * const g_FreeMoveModule;
extern PLANNER g_FreePlanner;

extern const char g_AxisCodes[];
extern const u8 g_AxisCodesSize;

extern const u8 g_ExtruderCount_1;
extern const char* g_ExtruderCodes_1[];
extern EXTRUDER_STRU g_Extruder_Stru_1[];
extern const char g_AxisCodes_1_Origin[][PARSE_STRING_SIZE];
extern 		 char g_AxisCodes_1[];
extern const u8 g_TotalAxesCount_1;
extern const enum _AXIS g_Axes_1[];
extern const long g_AxesOffset_1[];
extern const u8 g_MainAxesCount_1;
extern const enum _AXIS g_MainAxes_1[];
extern const u8 g_MainAxesIndex_1[];
extern const u8 g_ExtruderAxesCount_1;
extern const enum _AXIS g_ExtruderAxes_1[];
extern const u8 g_ExtruderAxesIndex_1[];
extern const u8 g_FreeAxesCount_1;
extern const enum _AXIS g_FreeAxes_1[];
extern const u8 g_FreeAxesIndex_1[];
extern MOVE_MODULE * const g_MainMoveModule_1;
extern PLANNER g_MainPlanner_1;
extern const u8 g_HomingAxesCount_1;
extern const u8 g_subHomingAxesCount_1;
extern const enum _AXIS g_HomingAxes_1[];
extern const enum _AXIS g_subHomingAxes_1[];
extern const char *g_SetHomeGcode_1[2];


extern const u8 g_ExtruderCount_2;
extern const char* g_ExtruderCodes_2[];
extern EXTRUDER_STRU g_Extruder_Stru_2[];
extern const char g_AxisCodes_2_Origin[][PARSE_STRING_SIZE];
extern 		 char g_AxisCodes_2[];
extern const u8 g_TotalAxesCount_2;
extern const enum _AXIS g_Axes_2[];
extern const long g_AxesOffset_2[];
extern const u8 g_MainAxesCount_2;
extern const enum _AXIS g_MainAxes_2[];
extern const u8 g_MainAxesIndex_2[];
extern const u8 g_ExtruderAxesCount_2;
extern const enum _AXIS g_ExtruderAxes_2[];
extern const u8 g_ExtruderAxesIndex_2[];
extern const u8 g_FreeAxesCount_2;
extern const enum _AXIS g_FreeAxes_2[];
extern const u8 g_FreeAxesIndex_2[];
extern MOVE_MODULE * const g_MainMoveModule_2;
extern PLANNER g_MainPlanner_2;
extern const u8 g_HomingAxesCount_2;
extern const u8 g_subHomingAxesCount_2;
extern const enum _AXIS g_HomingAxes_2[];
extern const enum _AXIS g_subHomingAxes_2[];
extern const char *g_SetHomeGcode_2[2];


extern const u8 g_ExtruderCount_3;
extern const char* g_ExtruderCodes_3[];
extern EXTRUDER_STRU g_Extruder_Stru_3[];
extern const char g_AxisCodes_3_Origin[][PARSE_STRING_SIZE];
extern 		 char g_AxisCodes_3[];
extern const u8 g_TotalAxesCount_3;
extern const enum _AXIS g_Axes_3[];
extern const long g_AxesOffset_3[];
extern const u8 g_MainAxesCount_3;
extern const enum _AXIS g_MainAxes_3[];
extern const u8 g_MainAxesIndex_3[];
extern const u8 g_ExtruderAxesCount_3;
extern const enum _AXIS g_ExtruderAxes_3[];
extern const u8 g_ExtruderAxesIndex_3[];
extern const u8 g_FreeAxesCount_3;
extern const enum _AXIS g_FreeAxes_3[];
extern const u8 g_FreeAxesIndex_3[];
extern MOVE_MODULE * const g_MainMoveModule_3;
extern PLANNER g_MainPlanner_3;
extern const u8 g_HomingAxesCount_3;
extern const u8 g_subHomingAxesCount_3;
extern const enum _AXIS g_HomingAxes_3[];
extern const enum _AXIS g_subHomingAxes_3[];
extern const char *g_SetHomeGcode_3[2];

#endif	/* GLOBALS_H */

