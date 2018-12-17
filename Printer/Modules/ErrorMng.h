/********************** (C) COPYRIGHT 2015 DediBot ******************************
  * @file    ErrorMng.h
  * @author  Dedibot
  * @version V1.2.0
  * @date    2017-05-25
  * @brief   异常管理
*********************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ERRORMNG_H
#define ERRORMNG_H
#include "de_type.h"
/* 异常定义 */
typedef enum
{
	ERROR_COM,			//通信异常
//	ERROR_BREAK,		//心跳中断
	ERROR_COMTIMOUT,	//通信超时
//	ERROR_NOPRINTDATA,	//长时间没有打印数据
	ERROR_TEMOVERTOP1,	//喷头1温度过高
	ERROR_TEMOVERTOP2,	//喷头2温度过高
	ERROR_TEMOVERTOPBAD,//热床温度过高
	ERROR_TEMTIMEOUT1,	//喷头1加热超时
	ERROR_TEMTIMEOUT2,	//喷头2加热超时	
	ERROR_TEMTIMEOUTBAD,//热床加热超时	
	ERROR_TEMHEAD1,		//加热1头异常
	ERROR_TEMHEAD2,		//加热2头异常
	ERROR_TEMBADHEAD,	//热床异常
	ERROR_TEMSEN1,		//喷头传感器1异常
	ERROR_TEMSEN2,		//喷头传感器2异常
	ERROR_TEMSENBAD,	//热床传感器异常
	ERROR_STARVING1,	//缺料异常
	ERROR_STARVING2,	//缺料异常	
//	ERROR_PUTTY,		//堵料
	ERROR_LIMIT_X,		//限位开关异常X
	ERROR_LIMIT_Y,		//限位开关异常Y
	ERROR_LIMIT_Z1,		//限位开关异常Z1
	ERROR_LIMIT_Z2,		//限位开关异常Z2
	ERROR_POSITION_OUT,	//位置超出
	ERROR_MOTOR_DRIVER1,//驱动器异常X
	ERROR_MOTOR_DRIVER2,//驱动器异常Y
	ERROR_MAX
}ERROR_NAME;

/* 异常等级定义 */
typedef enum
{
	ERROE_LEVEL_0,	//停止打印，不可清除
	ERROE_LEVEL_1,	//暂停打印，不可清除
	ERROE_LEVEL_2,	//暂停打印，自动清除
	ERROE_LEVEL_3,	//自动清除
	ERROE_LEVEL_4,
	ERROE_LEVEL_5,
	ERROE_LEVEL_NORMAL = ERROE_LEVEL_5,
	
	ERROR_LEVEL_MAX
}ERROR_LEVEL;

/* 异常属性结构定义 */
typedef struct
{
	// ERROR_NAME	name;
	u32			code;
	ERROR_LEVEL	defaultLevel;
}ERROR_PROPERTY;

// typedef struct 
// {
	// ERROR_NAME	name;
	// ERROR_LEVEL	level;
// }ERROR;
extern void ErrorMng_SetError(ERROR_NAME errName);
extern void ErrorMng_ResetAllError(void);
extern bool ErrorMng_IsError(void);
extern bool IsErrorFlag(ERROR_NAME err);
extern void ErrorMng_ResetError(ERROR_NAME errName);
extern bool ErrorMng_IsErrorLevel(ERROR_LEVEL level);
extern ERROR_LEVEL Get_HighestLevel(void);
extern void ErrorMng_Init(void);
extern u8* GetError(void);
extern bool Get_HighestLevelError( ERROR_NAME *outErr );
#endif  //ERRORMNG_H








