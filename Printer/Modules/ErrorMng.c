/********************** (C) COPYRIGHT 2015 DediBot ******************************
  * @file    ErrorMng.c
  * @author  Dedibot
  * @version V1.2.0
  * @date    2017-05-28
  * @brief   异常管理
*********************************************************************************/
/*******************************************
	包含关系
********************************************/
#include "ErrorMng.h"			//异常管理
#include "PrinterCtrl.h"
#include "ProtocolMng.h"
void Fun_ErrorAction_Level_0( ERROR_NAME errName );
void Fun_ErrorAction_Level_1( ERROR_NAME errName );
void Fun_ErrorAction_Level_2( ERROR_NAME errName );
void Fun_ErrorAction_Level_3( ERROR_NAME errName );
void Fun_ErrorAction_Level_4( ERROR_NAME errName );
void Fun_ErrorAction_Level_5( ERROR_NAME errName );

void ErrorMng_SetErrorIndicata(ERROR_NAME errName);
void ErrorMng_ResetErrorIndicata(ERROR_NAME errName);
/* 异常状态定义 */
typedef enum
{
	ERROR_STAGE_NONE,
	ERROR_STAGE_HAPPEN,
	
	ERROR_STAGE_MAX
}ERROR_STAGE;

/* 异常发生时的异常处理定义 */
typedef void (*FUN_ERROR_ACTION)(ERROR_NAME errName);



/*******************************************
    本地全部变量
********************************************/
static ERROR_PROPERTY m_ErrorProgerties[ERROR_MAX] = 
{
											//code编号				//level等级
/* ERROR_COM */								{0,						ERROE_LEVEL_2},
///* 	ERROR_COMTIMOUT,	//通信超时 */		{1,						ERROE_LEVEL_2},
/* ERROR_BREAK */		/*心跳中断	*/		{1,						ERROE_LEVEL_2},
///* ERROR_NOPRINTDATA */	   /*心跳中断	*/	{3,						ERROE_LEVEL_2},
/* ERROR_TEMOVERTOP1,	//温度过高 */		{2,						ERROE_LEVEL_1},
/* ERROR_TEMOVERTOP2,	//温度过高 */		{3,						ERROE_LEVEL_1},
/* ERROR_TEMOVERTOPBAD,	//温度过高 */		{4,						ERROE_LEVEL_1},
/* ERROR_TEMOUT1,		//加热超?*/			{5,						ERROE_LEVEL_2},
/* ERROR_TEMOUT2,		//加热超?*/			{6,						ERROE_LEVEL_2},
/* ERROR_TEMOUTBAD,		//加热超?*/			{7,						ERROE_LEVEL_2},
/* ERROR_TEMHEAD1,		//加热头异常 */		{8,					ERROE_LEVEL_1},
/* ERROR_TEMHEAD2,		//加热头异常 */		{9,					ERROE_LEVEL_1},
/* ERROR_TEMBADHEAD,	//加热头异常 */		{10,					ERROE_LEVEL_1},
/* ERROR_TEMSEN1,		//传感器异常*/		{11,					ERROE_LEVEL_1},
/* ERROR_TEMSEN2,		//传感器异常*/		{12,					ERROE_LEVEL_1},
/* ERROR_TEMSENBAD,		//传感器异常*/		{13,					ERROE_LEVEL_1},
/* ERROR_STARVING1,		//缺料异?*/			{14,					ERROE_LEVEL_2},
/* ERROR_STARVING2,		//缺料异?*/			{15,					ERROE_LEVEL_2},
///* ERROR_PUTTY,			//堵料*/			{18,					ERROE_LEVEL_2},
/*	ERROR_LIMIT_X,		//限位开关异常X*/	{16,					ERROE_LEVEL_1},
/*	ERROR_LIMIT_Y,		//限位开关异常Y*/	{17,					ERROE_LEVEL_1},
/*	ERROR_LIMIT_Z1,		//限位开关异常Z1*/	{18,					ERROE_LEVEL_1},
/*	ERROR_LIMIT_Z2,		//限位开关异常Z2*/	{19,					ERROE_LEVEL_1},
/*	ERROR_POSITION_OUT,	//位置超出*/		{20,					ERROE_LEVEL_0},
/*	ERROR_MOTOR_DRIVER1,//驱动器异常X*/		{21,					ERROE_LEVEL_1},
/*	ERROR_MOTOR_DRIVER2,//驱动器异常Y*/		{22,					ERROE_LEVEL_1}
};

static const FUN_ERROR_ACTION m_Fun_ErrorActionTbl[ERROR_LEVEL_MAX] = 
{
/* ERROE_LEVEL_0 */		Fun_ErrorAction_Level_0,	//打印停止
/* ERROE_LEVEL_1 */		Fun_ErrorAction_Level_1,	//打印暂停
/* ERROE_LEVEL_2 */		Fun_ErrorAction_Level_2,
/* ERROE_LEVEL_3 */		Fun_ErrorAction_Level_3,
/* ERROE_LEVEL_4 */		Fun_ErrorAction_Level_4,
/* ERROE_LEVEL_5 */		Fun_ErrorAction_Level_5
};

#define ERRORFLAG_NUM	(ERROR_MAX/(sizeof(u8)*8) + 1)
static u8	m_ErrorFlags[ERRORFLAG_NUM];

static ERROR_LEVEL m_HighestLevel;
static ERROR_STAGE m_Stage;
	
/*******************************************
	外部参考变量
********************************************/
/*******************************************
	内部函数声明
********************************************/
/*******************************************
	内部函数
*******************************************/
/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
void ErrorAction_FirstError(void)
{
	
}
/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
void Fun_ErrorAction_Level_0(ERROR_NAME errName)
{
	ErrorMng_SetErrorIndicata(errName);
	// if(g_printer.status == PROCESS_STATUS_PRINT)	//如果是打印状态，主动暂停打印
	{
		EventMng_SetEvent_Level(PRINTER_EVENT_STANDBY, NULL ,EVENT_LEVEL_0);
	}	
}
/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
void Fun_ErrorAction_Level_1(ERROR_NAME errName)
{
	ErrorMng_SetErrorIndicata(errName);
	// if(g_printer.status == PROCESS_STATUS_PRINT)	//如果是打印状态，主动暂停打印
	{
		// EventMng_SetEvent_Level(PRINTER_EVENT_PRINTPAUSE, NULL ,EVENT_LEVEL_0);
	}
}
/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
void Fun_ErrorAction_Level_2(ERROR_NAME errName)
{
	ErrorMng_SetErrorIndicata(errName);
}
/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
void Fun_ErrorAction_Level_3(ERROR_NAME errName)
{
	ErrorMng_SetErrorIndicata(errName);
}
/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
void Fun_ErrorAction_Level_4(ERROR_NAME errName)
{
	ErrorMng_SetErrorIndicata(errName);
}
/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
void Fun_ErrorAction_Level_5(ERROR_NAME errName)
{
	ErrorMng_SetErrorIndicata(errName);
}

void ErrorMng_SetStopPrint()
{
	// if(g_printer.status == PROCESS_STATUS_PRINT)
	{
		EventMng_SetEvent_Level(PRINTER_EVENT_STANDBY, PortEventCbFunc_Standby ,EVENT_LEVEL_0);	//回到待机
	}
}

void ErrorMng_SetPause()
{
	// if(g_printer.status == PROCESS_STATUS_PRINT)
	{
		// EventMng_SetEvent_Level(PRINTER_EVENT_PRINTPAUSE, PortEventCbFunc_PrintPause ,EVENT_LEVEL_0);
	}
}
void ErrorMng_SetErrorIndicata(ERROR_NAME errName)
{
	switch((u8)errName)
	{
		case ERROR_COM:
			ErrorMng_SetPause();
			break;
		case ERROR_COMTIMOUT:
			ErrorMng_SetPause();
			break;
//		case ERROR_BREAK:
//			ErrorMng_SetPause();
//			break;
//		case ERROR_NOPRINTDATA:
//			ErrorMng_SetPause();
//			break;
		case ERROR_TEMHEAD1:
		case ERROR_TEMHEAD2:
		case ERROR_TEMBADHEAD:
			ErrorMng_SetPause();
			break;
		case ERROR_TEMTIMEOUT1:
		case ERROR_TEMTIMEOUT2:
		case ERROR_TEMTIMEOUTBAD:
			ErrorMng_SetPause();
			// IndicateCtrl_Set(INDICATE_TIME_ALWAYS, INDICATE_TIME_NEVER, INDICATE_YELLOW, False);	//黄灯常亮
			
			break;
		case ERROR_TEMOVERTOP1:
		case ERROR_TEMOVERTOP2:
		case ERROR_TEMOVERTOPBAD:
			ErrorMng_SetPause();
			// IndicateCtrl_Set(INDICATE_TIME_ALWAYS, INDICATE_TIME_NEVER, INDICATE_BUZZER, False);
			// IndicateCtrl_Set(INDICATE_TIME_150MS, INDICATE_TIME_150MS, INDICATE_G_Y_R, False);			
			break;
		case ERROR_TEMSEN1:
		case ERROR_TEMSEN2:
		case ERROR_TEMSENBAD:
			ErrorMng_SetPause();
			break;
		case ERROR_STARVING1:
		case ERROR_STARVING2:
			// IndicateCtrl_Set(INDICATE_TIME_500MS, INDICATE_TIME_2S, INDICATE_BUZZER, False);
			// IndicateCtrl_Set(INDICATE_TIME_ALWAYS, INDICATE_TIME_NEVER, INDICATE_RED, False);				
			break;
//		case ERROR_PUTTY:
//			ErrorMng_SetPause();
			// break;
		case ERROR_LIMIT_X:
//			break;
		case ERROR_LIMIT_Y:
//			break;
		case ERROR_LIMIT_Z1:
//			break;
		case ERROR_LIMIT_Z2:
			ErrorMng_SetPause();
			break;
		case ERROR_POSITION_OUT:
			ErrorMng_SetStopPrint();
			break;		
		case ERROR_MOTOR_DRIVER1:
			break;
		case ERROR_MOTOR_DRIVER2:
			break;
		default:
			break;
	}
}
void ErrorMng_ResetErrorIndicata(ERROR_NAME errName)
{
	switch((u8)errName)
	{
		case ERROR_COM:
			break;
		case ERROR_COMTIMOUT:
			break;
//		case ERROR_BREAK:
//			break;
//		case ERROR_NOPRINTDATA:
//			break;
		case ERROR_TEMHEAD1:
		case ERROR_TEMHEAD2:
		case ERROR_TEMBADHEAD:
			break;
		case ERROR_TEMTIMEOUT1:
		case ERROR_TEMTIMEOUT2:
		case ERROR_TEMTIMEOUTBAD:
			// IndicateCtrl_Set(INDICATE_TIME_NEVER, INDICATE_TIME_ALWAYS, INDICATE_YELLOW, False);
			break;
		case ERROR_TEMOVERTOP1:
		case ERROR_TEMOVERTOP2:
		case ERROR_TEMOVERTOPBAD:
			// IndicateCtrl_Set(INDICATE_TIME_NEVER, INDICATE_TIME_ALWAYS, INDICATE_BUZZER, False);
			// IndicateCtrl_Set(INDICATE_TIME_NEVER, INDICATE_TIME_ALWAYS, INDICATE_G_Y_R, False);			
			break;
		case ERROR_TEMSEN1:
		case ERROR_TEMSEN2:
		case ERROR_TEMSENBAD:
			break;
		case ERROR_STARVING1:
		case ERROR_STARVING2:
			// IndicateCtrl_Set(INDICATE_TIME_NEVER, INDICATE_TIME_ALWAYS, INDICATE_BUZZER, False);
			// IndicateCtrl_Set(INDICATE_TIME_NEVER, INDICATE_TIME_ALWAYS, INDICATE_RED, False);				
			break;
//		case ERROR_PUTTY:
//			ErrorMng_SetPause();
//			break;
		case ERROR_LIMIT_X:
			break;
		case ERROR_LIMIT_Y:
			break;
		case ERROR_LIMIT_Z1:
			break;
		case ERROR_LIMIT_Z2:
			break;
		case ERROR_POSITION_OUT:
			break;		
		case ERROR_MOTOR_DRIVER1:
			break;
		case ERROR_MOTOR_DRIVER2:
			break;	
		default:
			break;
	}
}
/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
void SetErrorFlag(ERROR_NAME err)
{
	int mainId = err >> 3;
	int subId = err & 0x0FF;
	
	m_ErrorFlags[mainId] |= (1 << subId);
}
/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
u8* GetError(void)
{
	return m_ErrorFlags;
}
/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
void ClearErrorFlag(ERROR_NAME err)
{
	int mainId = err >> 3;
	int subId = err & 0x0FF;
	
	m_ErrorFlags[mainId] &= ~(1 << subId);
}
/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
u32 GetErrorFlags(ERROR_NAME *outErr)
{
	int subId;
	int mainId;
	u32 errCount = 0;
	
	for(mainId = 0; mainId < ERRORFLAG_NUM; mainId++)
	{
		if(m_ErrorFlags[mainId] != 0)
		{
			for(subId = 0; subId < 8; subId++)
			{
				if(((1<<subId) & m_ErrorFlags[mainId]) != 0)
				{
					outErr[errCount++] = (ERROR_NAME)((mainId << 3) | subId);
				}
			}
		}
	}
	return errCount;
}
/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
bool GetErrorFlag(ERROR_NAME *outErr)
{
	ERROR_NAME errs[ERRORFLAG_NUM];
	u32 count = GetErrorFlags(errs);
	int i;
	ERROR_LEVEL highLevel = ERROE_LEVEL_NORMAL;
	ERROR_NAME highErr;
	
	if(count > 0)
	{
		for(i = 0; i < count; i++)
		{
			if(m_ErrorProgerties[errs[i]].defaultLevel < highLevel)
			{
				highLevel = m_ErrorProgerties[errs[i]].defaultLevel;
				highErr = errs[i];
			}
		}
		*outErr = highErr;
		return True;
	}
	return False;
}
/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
bool IsErrorFlag(ERROR_NAME err)
{
	int mainId = err >> 3;
	int subId = err & 0x0FF;
	
	return (bool)((m_ErrorFlags[mainId] & (1 << subId)) != 0);
}

/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
void ErrorMng_Clear(void)
{
	memset(m_ErrorFlags, 0, sizeof(m_ErrorFlags));
	m_HighestLevel = ERROE_LEVEL_NORMAL;
	m_Stage = ERROR_STAGE_NONE;
}

/******************************************
功能：	
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
void ErrorMng_FandNextErr(void)
{
}


/*******************************************
	外部函数
*******************************************/
/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
void ErrorMng_Init(void)
{
	ErrorMng_Clear();
}
//ERROR_NAME ErrorMng_GetTempError(u8 temp ,)
//{
//	
//}
/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
void ErrorMng_SetError(ERROR_NAME errName)
{
	ERROR_PROPERTY *property = &m_ErrorProgerties[errName];
	
	SetErrorFlag(errName);

	if(m_Stage == ERROR_STAGE_NONE)
	{
		ErrorAction_FirstError();
		m_Stage = ERROR_STAGE_HAPPEN;
	}
	//存在更高等级的异常，只登记不进行异常处理
	if(m_HighestLevel >= property->defaultLevel)
	{
		m_HighestLevel = property->defaultLevel;
		
		//相应等级下的处理函数
		m_Fun_ErrorActionTbl[m_HighestLevel](errName);
	}
}

/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
void ErrorMng_ResetError(ERROR_NAME errName)
{
//	ERROR_PROPERTY *property = &m_ErrorProgerties[errName];
	ERROR_NAME error;
	
	ClearErrorFlag(errName);
	if(False == GetErrorFlag(&error))
	{
		m_Stage = ERROR_STAGE_NONE;
	}
	else
	{
		m_HighestLevel = m_ErrorProgerties[error].defaultLevel;
	}	
	if(m_ErrorProgerties[error].defaultLevel <= m_HighestLevel)//清除的异常等级是最高的，清除异常指示
	{
		ErrorMng_ResetErrorIndicata(errName);				
	}
}

/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
void ErrorMng_ResetAllError(void)
{
	ErrorMng_Clear();
}

/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
bool ErrorMng_IsError(void)
{
	return (bool)(ERROR_STAGE_HAPPEN == m_Stage);
}

/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
bool ErrorMng_IsErrorLevel(ERROR_LEVEL level)
{
	ERROR_NAME err;
	
	if(False != GetErrorFlag(&err))
	{
		if(m_ErrorProgerties[err].defaultLevel >= level)
		{
			return True;
		}
	}
	return False;
}

ERROR_LEVEL Get_HighestLevel()
{
	return m_HighestLevel;
}

bool Get_HighestLevelError(ERROR_NAME *outErr)
{
	bool ret;
	ERROR_NAME highest;
	if(GetErrorFlag(&highest) == True)
	{
		ret = True;
	}
	else
	{
		ret = False;
	}
	return ret;
}

/******************************************
功能：		
作成者：	
作成日期：	2017-05-28
参数：		无
返回值：	无
*******************************************/
u32 ErrorMng_GetAllErrors(u8 *outErrCodes)
{
	ERROR_NAME errs[ERRORFLAG_NUM];
	u32 count = GetErrorFlags(errs);
	int i;
	
	if(count > 0)
	{
		for(i = 0; i < count; i++)
		{
			outErrCodes[i] = m_ErrorProgerties[errs[i]].code;
		}
	}
	return count;
}


