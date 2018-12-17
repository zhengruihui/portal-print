#include "TaskMng.h"
#include "string.h"

/*******************************************
    本地宏
********************************************/

/*******************************************
    本地全局变量
********************************************/
static timerDelay  m_timerDelay[TIMER_DELAY_EVENT_MAX];  //延时动作情报
static bool        m_EventOccur[TIMER_DELAY_EVENT_MAX];  //事件发生标志
/*******************************************
	外部参考变量
********************************************/

/*******************************************
	内部函数声明
********************************************/
static void TaskMng_Set_Event( T_DELAY_EVT ); //事件发生
/*******************************************
		外部函数
*******************************************/
u32 m_timer_millis; //Timer 定时计数

Sys_time g_timer;

void TaskMng_Time_Accumulation()
{
	g_timer.time_2dot5_millis++;
}

u32 TaskMng_GetSysTime()//单位2.5ms
{
	return g_timer.time_2dot5_millis;
}
/*******************************************
功能：监视时间到了以后，触发事件。
作成者：cao-dl
作成日期：2015/3/2
参数：
    event:事件
返回值：无
*******************************************/
void TaskMng_Set_Event( T_DELAY_EVT event)
{
    m_EventOccur[event] = True;
}

/*******************************************
功能：设定延时动作
    指定的时间到了以后，调用登录函数。
作成者：cao-dl
作成日期：2015/3/2
参数：
    event:事件
    cbfunc：回调函数
    timecnt:计数时间(2.5ms的倍数)
    timetype:类型
返回值：无
*******************************************/
void TaskMng_Set_Delay( T_DELAY_EVT event, cbfun cbfunc, u32 timecnt, T_TYPE timetype , u32 count)
{
    m_timerDelay[event].m_CbFunc = cbfunc;
    m_timerDelay[event].m_IsRegister = True;
    m_timerDelay[event].m_TimeCnt = timecnt;
    m_timerDelay[event].m_Timer_Type = timetype;
		m_timerDelay[event].m_count = count;
    //周期性动作
    if( timetype != TIMER_ONE_COUNT)
    {
        m_timerDelay[event].m_TimeCnt_bak = timecnt;
    }
}
/*******************************************
功能：设定延时动作
    指定的时间到了以后，调用登录函数。
作成者：cao-dl
作成日期：2015/3/2
参数：
    event:事件
    cbfunc：回调函数
    timecnt:计数时间(2.5ms的倍数)
    timetype:类型
返回值：无
*******************************************/
void TaskMng_AckSet_Delay( T_DELAY_EVT event, ackcbfun cbfunc, u32 timecnt, T_TYPE timetype , u32 count)
{
    m_timerDelay[event].m_AckCbFunc = cbfunc;
    m_timerDelay[event].m_IsRegister = True;
    m_timerDelay[event].m_TimeCnt = timecnt;
    m_timerDelay[event].m_Timer_Type = timetype;
		m_timerDelay[event].m_count = count;
    //周期性动作
    if( timetype != TIMER_ONE_COUNT)
    {
        m_timerDelay[event].m_TimeCnt_bak = timecnt;
    }
}
/******************************************
功能：取消延时动作
    如果该事件在指定的时间到之前，取消该动作。
作成者：cao-dl
作成日期：2015/3/2
参数：
    event:事件
返回值：无
*******************************************/
void TaskMng_Cancel_Delay( T_DELAY_EVT event )
{
    memset( &m_timerDelay[event], 0, sizeof(m_timerDelay[event]) );
}
/******************************************
功能：停止延时动作
作成者：cao-dl
作成日期：2015/3/2
参数：
    event:事件
返回值：无
*******************************************/
void TaskMng_Stop_Delay( T_DELAY_EVT event )
{
    memset( &m_timerDelay[event], 0, sizeof(m_timerDelay[event]) );
}

/******************************************
功能：延时动作实施
    该事件在指定的时间到了以后，执行该动作。
作成者：cao-dl
作成日期：2015/3/2
参数：无
返回值：无
*******************************************/
void TaskMng_DelayAct_Do( void )
{
	u8 i;
	//登录动作，进行循环扫描
	for( i = 0; i < TIMER_DELAY_EVENT_MAX; i ++ )
	{
		//事件发生
		if( m_EventOccur[i] )
		{
			//登录过，且回调函数不为空
			if( m_timerDelay[i].m_IsRegister && (m_timerDelay[i].m_CbFunc != NULL) )
			{
				( *m_timerDelay[i].m_CbFunc )();
				//一次性动作
				if( m_timerDelay[i].m_Timer_Type == TIMER_ONE_COUNT )
				{
					memset( &m_timerDelay[i], 0, sizeof( m_timerDelay[i] ) );
				}
				else if(m_timerDelay[i].m_Timer_Type == TIMER_SET_COUNT)				//规定次数的动作
				{
					m_timerDelay[i].m_count--;
					if(m_timerDelay[i].m_count==0)
					{
						memset( &m_timerDelay[i], 0, sizeof( m_timerDelay[i] ) );
					}
				}
			}
			m_EventOccur[i] = False;
			//防止一次性过多事件发生
			break;
		}
	}
}
/******************************************
功能：延时监视
    登录后动作，时间进行减算，时间到了后触发动作事件。
    (注：2.5ms周期)
作成者：cao-dl
作成日期：2015/3/2
参数：无
返回值：无
*******************************************/
void TaskMng_Delay_Watch( void )
{
    u8 i;
    //登录动作，进行循环扫描
    for( i = 0; i < TIMER_DELAY_EVENT_MAX; i ++ )
    {
        //计数时间不为空
        if( m_timerDelay[i].m_TimeCnt > 0 )
        {
            m_timerDelay[i].m_TimeCnt--;
            //时间到了以后，触发事件
            if( m_timerDelay[i].m_TimeCnt == 0 )
            {
                //事件发生
                TaskMng_Set_Event( (T_DELAY_EVT)i );
                //周期性的动作，继续。。。
                if( m_timerDelay[i].m_Timer_Type != TIMER_ONE_COUNT)
                {
                    m_timerDelay[i].m_TimeCnt = m_timerDelay[i].m_TimeCnt_bak;
                }
            }
        }
    }
}
/******************************************
功能：初始化
作成者：cao-dl
作成日期：2015/3/2
参数：无
返回值：无
*******************************************/
void TaskMng_Init( void )
{
    //初始化
    memset( m_timerDelay, 0, sizeof(m_timerDelay) );
    memset( m_EventOccur, False, sizeof(m_EventOccur) );
}





