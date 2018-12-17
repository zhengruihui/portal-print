#ifndef TASK_MNG_H
#define TASK_MNG_H
#include "de_type.h"


typedef struct{
	u32 time_2dot5_millis;
	u32 time_print_star;
	u32 Print_Total_time;
	u32 Print_Start_time;
}Sys_time;
extern Sys_time g_timer;

extern void TaskMng_Get_PrintTime( void );
typedef enum TIMER_TYPE
{
    TIMER_ONE_COUNT,        //一次性计数
    TIMER_INTERVAL,         //重复计数
    TIMER_SET_COUNT,        //规定次数
    TIMER_TYPE_MAX
} T_TYPE;
//动作
typedef enum TIMER_DELAY_EVENT
{
    TIMER_DELAY_PRINT,
    TIMER_DELAY_WARNING,	
	TIMER_DELAY_PRINT_TEMPREP,
	TIMER_DELAY_COUNT,
    TIMER_DELAY_MATERIAL,
	TIMER_DELAY_LINK,		//串口连接
	TIMER_DELAY_ACK_RETRY,//ack数据重发
    TIMER_DELAY_EVENT_MAX
} T_DELAY_EVT;
//CB函数
typedef void (*cbfun)(void);
typedef void (*ackcbfun)(void);
//延时动作情报
typedef struct
{
    bool        m_IsRegister;   //注册标记
    cbfun       m_CbFunc;       //回调函数
		ackcbfun		m_AckCbFunc;
    u32         m_TimeCnt;      //计数时间
    u32         m_TimeCnt_bak;  //计数时间(备份)
    T_TYPE      m_Timer_Type;   //计数类型
	u32         m_count;      	//计数次数
} timerDelay;

/*******************************************
	输出接口
********************************************/

//设定延时动作
extern void TaskMng_Set_Delay( T_DELAY_EVT , cbfun, u32, T_TYPE, u32 count);
//设定延时动作
extern void TaskMng_AckSet_Delay( T_DELAY_EVT event, ackcbfun cbfunc, u32 timecnt, T_TYPE timetype , u32 count);
//取消延时动作
extern void TaskMng_Cancel_Delay( T_DELAY_EVT );
//停止延时动作
extern void TaskMng_Stop_Delay( T_DELAY_EVT );
//延时动作实施
extern void TaskMng_DelayAct_Do( void );
//延时监视
extern void TaskMng_Delay_Watch( void );
//初始化
extern void TaskMng_Init( void );
#endif
