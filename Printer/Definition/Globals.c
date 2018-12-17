/********************** (C) COPYRIGHT 2011 STMicroelectronics ********************
  * @file    Globals.c
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-03-25
  * @brief   ȫ�ֱ�������
*********************************************************************************/
#include "Globals.h"														//ȫ�ֱ�������
#include "PrintDataMng.h"
#include "PortalFrame.h"

/*******************************************
    ȫ�ֱ���
********************************************/


EXTRUDER_STRU g_ExtruderStructs[EXTRUDER_MAX];

/* ������λ���صĶ�Ӧ */
const enum _INPUT g_Axis_Limit_Tbl[AXIS_MAX] =
{
/* X1_AXIS */		INPUT_LIMIT1,
/* Y1_AXIS */		INPUT_LIMIT2,
/* Z1_AXIS */		INPUT_LIMIT3,
/* E1_AXIS */		INPUT_MAX,
	
/* X2_AXIS */		INPUT_LIMIT12,
/* Y2_AXIS */		INPUT_LIMIT6,
/* Z2_AXIS */		INPUT_LIMIT7,
/* E2_AXIS_1 */		INPUT_MAX, 
/* E2_AXIS_2 */		INPUT_MAX,
/* E2_AXIS_3 */		INPUT_MAX,
/* E2_AXIS_4 */		INPUT_MAX,
	
/* X3_AXIS */		INPUT_LIMIT9,
/* Y3_AXIS */		INPUT_LIMIT10,
/* Z3_AXIS */		INPUT_MAX,
/* E3_AXIS */		INPUT_MAX,


/* W_AXIS */		INPUT_MAX
};


//�����Ͷ���
const char g_AxisCodes[] =
{
	'X',
	'Y',
	'Z',
	'E',
	'W'
};
const u8 g_AxisCodesSize = sizeof(g_AxisCodes);


MOVE_MODULE * const g_FreeMoveModule = &g_MoveModules[MOVE_MODULE_4];
PLANNER g_FreePlanner;



/************ ���ż�1 ************/
//��ͷ�豸
const u8 g_ExtruderCount_1 = 1;
const char* g_ExtruderCodes_1[] =
{
	"S"
};
EXTRUDER_STRU g_Extruder_Stru_1[] = 
{
	{TEMPCTRL_EXTRUDER1,	0, 0, 0, FAN_1, FAN_2, 90}
};

// λ�ÿ���
const char g_AxisCodes_1_Origin[][PARSE_STRING_SIZE] =
{
	"X",
	"Y",
	"Z",
	"E",
	"W"
};
char g_AxisCodes_1[sizeof(g_AxisCodes_1_Origin)];

const u8 g_TotalAxesCount_1 = 5;
const enum _AXIS g_Axes_1[] =
{
	X1_AXIS,
	Y1_AXIS,
	Z1_AXIS,
	E1_AXIS,
	W_AXIS
};
const long g_AxesOffset_1[] =
{ 
	0,
	0,  
	0,
	0,
	0
};

const u8 g_MainAxesCount_1 = 3;
const enum _AXIS g_MainAxes_1[] =
{
	X1_AXIS,
	Y1_AXIS,
	Z1_AXIS
};
const u8 g_MainAxesIndex_1[] =
{ 
	0,
	1,
	2
};

const u8 g_ExtruderAxesCount_1 = 1;
const enum _AXIS g_ExtruderAxes_1[] =
{
	E1_AXIS
};
const u8 g_ExtruderAxesIndex_1[] =
{
	3
};

const u8 g_FreeAxesCount_1 = 1;
const enum _AXIS g_FreeAxes_1[] =
{
	W_AXIS
};
const u8 g_FreeAxesIndex_1[] =
{
	4
};
MOVE_MODULE * const g_MainMoveModule_1 = &g_MoveModules[MOVE_MODULE_1];
PLANNER g_MainPlanner_1;

const u8 g_HomingAxesCount_1 = 1;
const u8 g_subHomingAxesCount_1 = 1;
const enum _AXIS g_HomingAxes_1[] =
{
	X1_AXIS,
};
const enum _AXIS g_subHomingAxes_1[] =
{
	X1_AXIS,
};
//��ʱû�кõķ�������ԭ�����꣬��gcode��ʵ��
const char *g_SetHomeGcode_1[2] = 
{
	"G92 X0",
	"G92 X0",
};
 


/************ ���ż�2 ************/
//��ͷ�豸
const u8 g_ExtruderCount_2 = 4;
const char* g_ExtruderCodes_2[] =
{
	"S",
	"S",
	"S",
	"S"
};
EXTRUDER_STRU g_Extruder_Stru_2[] = 
{
	{TEMPCTRL_EXTRUDER2_1,	0, 0, 0, FAN_1, FAN_2, 90},
	{TEMPCTRL_EXTRUDER2_2,	0, 0, 0, FAN_3, FAN_4, 90},
	{TEMPCTRL_EXTRUDER2_3,	0, 0, 0, FAN_5, FAN_6, 90},
	{TEMPCTRL_EXTRUDER2_4,	0, 0, 0, FAN_7, FAN_8, 90}
};

// λ�ÿ���
const char g_AxisCodes_2_Origin[][PARSE_STRING_SIZE] =
{
	"X",
	"Y",
	"Z",
	"E",
	"E",
	"E",
	"E",
	"W"
};
char g_AxisCodes_2[sizeof(g_AxisCodes_2_Origin)];

const u8 g_TotalAxesCount_2 = 8;
const enum _AXIS g_Axes_2[] =
{	
	X2_AXIS,	//���ż�2 X��
	Y2_AXIS,    //���ż�2 Y��
	Z2_AXIS,    //���ż�2 Z��
	E2_AXIS_1,  //���ż�2 E1��
	E2_AXIS_2,  //���ż�2 E2��
	E2_AXIS_3,  //���ż�2 E3��
	E2_AXIS_4,  //���ż�2 E4��
	W_AXIS
};
const long g_AxesOffset_2[] =
{ 
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

const u8 g_MainAxesCount_2 = 4;
const enum _AXIS g_MainAxes_2[] =
{
	X2_AXIS,	//���ż�2 X��
	Y2_AXIS,    //���ż�2 Y��
	Z2_AXIS,     //���ż�2 Z��
	W_AXIS,
};
const u8 g_MainAxesIndex_2[] =
{
	0,
	1,
	2,
	7
	
};

const u8 g_ExtruderAxesCount_2 = 4;
const enum _AXIS g_ExtruderAxes_2[] =
{
	E2_AXIS_1,  //���ż�2 E1��
	E2_AXIS_2,  //���ż�2 E2��
	E2_AXIS_3,  //���ż�2 E3��
	E2_AXIS_4,   //���ż�2 E4��
};
const u8 g_ExtruderAxesIndex_2[] =
{
	3,
	4,
	5,
	6,
};

const u8 g_FreeAxesCount_2 = 0;
const enum _AXIS g_FreeAxes_2[] =
{
	W_AXIS
};
const u8 g_FreeAxesIndex_2[] =
{
	7,
};

MOVE_MODULE * const g_MainMoveModule_2 = &g_MoveModules[MOVE_MODULE_2];
PLANNER g_MainPlanner_2;

const u8 g_HomingAxesCount_2 = 3;
const u8 g_subHomingAxesCount_2 = 2;
const enum _AXIS g_HomingAxes_2[] =
{
	X2_AXIS,
	Y2_AXIS,
	Z2_AXIS,
};
const enum _AXIS g_subHomingAxes_2[] =
{
	X2_AXIS,
	Y2_AXIS,
};
const char *g_SetHomeGcode_2[2] = 
{
	"G92 X0 Y0 Z0",
	"G92 X0 Y0",
};






/************ ���ż�3 ************/
//��ͷ�豸
const u8 g_ExtruderCount_3 = 1;
const char* g_ExtruderCodes_3[] =
{
	"S",
};
EXTRUDER_STRU g_Extruder_Stru_3[] = 
{
	{TEMPCTRL_EXTRUDER3_1,	0, 0, 0, FAN_5, FAN_6, 90},
};

// λ�ÿ���
const char g_AxisCodes_3_Origin[][PARSE_STRING_SIZE] =
{
	"X",
	"Y",
	"Z",
	"E",
	"W"
};
char g_AxisCodes_3[sizeof(g_AxisCodes_3_Origin)];

const u8 g_TotalAxesCount_3 = 5;
const enum _AXIS g_Axes_3[] =
{
	X3_AXIS,	//���ż�3 X��
	Y3_AXIS,  //���ż�3 Y1��
	Z3_AXIS,    //���ż�3 Z��
	E3_AXIS,  //���ż�3 E1��
	W_AXIS
};
const long g_AxesOffset_3[] =
{
	-3.0,
	-11.71,	
	0,		//���⽹��
	0,
	0,
};

const u8 g_MainAxesCount_3 = 4;
const enum _AXIS g_MainAxes_3[] =
{
	X3_AXIS,	//���ż�3 X��
	Y3_AXIS,  //���ż�3 Y1��
	Z3_AXIS,     //���ż�3 Z��
	W_AXIS
};
const u8 g_MainAxesIndex_3[] =
{
	0,
	1,
	2,
	4
};

const u8 g_ExtruderAxesCount_3 = 1;
const enum _AXIS g_ExtruderAxes_3[] =
{
	E3_AXIS,  //���ż�3 E1��
};
const u8 g_ExtruderAxesIndex_3[] =
{
	3,
};

const u8 g_FreeAxesCount_3 = 0;
const enum _AXIS g_FreeAxes_3[] =
{
	E3_AXIS
};
const u8 g_FreeAxesIndex_3[] =
{
	3
};
MOVE_MODULE * const g_MainMoveModule_3 = &g_MoveModules[MOVE_MODULE_3];
PLANNER g_MainPlanner_3;

const u8 g_HomingAxesCount_3 = 2;
const u8 g_subHomingAxesCount_3 = 2;
const enum _AXIS g_HomingAxes_3[] =
{
	X3_AXIS,
	Y3_AXIS,
};
const enum _AXIS g_subHomingAxes_3[] =
{
	X3_AXIS,
	Y3_AXIS,
};

const char *g_SetHomeGcode_3[2] = 
{
	"G92 X0 Y0 W0",
	"G92 X0 Y0 W0",
};







