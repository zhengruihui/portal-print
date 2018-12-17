#include "TaskMng.h"
#include "string.h"

/*******************************************
    ���غ�
********************************************/

/*******************************************
    ����ȫ�ֱ���
********************************************/
static timerDelay  m_timerDelay[TIMER_DELAY_EVENT_MAX];  //��ʱ�����鱨
static bool        m_EventOccur[TIMER_DELAY_EVENT_MAX];  //�¼�������־
/*******************************************
	�ⲿ�ο�����
********************************************/

/*******************************************
	�ڲ���������
********************************************/
static void TaskMng_Set_Event( T_DELAY_EVT ); //�¼�����
/*******************************************
		�ⲿ����
*******************************************/
u32 m_timer_millis; //Timer ��ʱ����

Sys_time g_timer;

void TaskMng_Time_Accumulation()
{
	g_timer.time_2dot5_millis++;
}

u32 TaskMng_GetSysTime()//��λ2.5ms
{
	return g_timer.time_2dot5_millis;
}
/*******************************************
���ܣ�����ʱ�䵽���Ժ󣬴����¼���
�����ߣ�cao-dl
�������ڣ�2015/3/2
������
    event:�¼�
����ֵ����
*******************************************/
void TaskMng_Set_Event( T_DELAY_EVT event)
{
    m_EventOccur[event] = True;
}

/*******************************************
���ܣ��趨��ʱ����
    ָ����ʱ�䵽���Ժ󣬵��õ�¼������
�����ߣ�cao-dl
�������ڣ�2015/3/2
������
    event:�¼�
    cbfunc���ص�����
    timecnt:����ʱ��(2.5ms�ı���)
    timetype:����
����ֵ����
*******************************************/
void TaskMng_Set_Delay( T_DELAY_EVT event, cbfun cbfunc, u32 timecnt, T_TYPE timetype , u32 count)
{
    m_timerDelay[event].m_CbFunc = cbfunc;
    m_timerDelay[event].m_IsRegister = True;
    m_timerDelay[event].m_TimeCnt = timecnt;
    m_timerDelay[event].m_Timer_Type = timetype;
		m_timerDelay[event].m_count = count;
    //�����Զ���
    if( timetype != TIMER_ONE_COUNT)
    {
        m_timerDelay[event].m_TimeCnt_bak = timecnt;
    }
}
/*******************************************
���ܣ��趨��ʱ����
    ָ����ʱ�䵽���Ժ󣬵��õ�¼������
�����ߣ�cao-dl
�������ڣ�2015/3/2
������
    event:�¼�
    cbfunc���ص�����
    timecnt:����ʱ��(2.5ms�ı���)
    timetype:����
����ֵ����
*******************************************/
void TaskMng_AckSet_Delay( T_DELAY_EVT event, ackcbfun cbfunc, u32 timecnt, T_TYPE timetype , u32 count)
{
    m_timerDelay[event].m_AckCbFunc = cbfunc;
    m_timerDelay[event].m_IsRegister = True;
    m_timerDelay[event].m_TimeCnt = timecnt;
    m_timerDelay[event].m_Timer_Type = timetype;
		m_timerDelay[event].m_count = count;
    //�����Զ���
    if( timetype != TIMER_ONE_COUNT)
    {
        m_timerDelay[event].m_TimeCnt_bak = timecnt;
    }
}
/******************************************
���ܣ�ȡ����ʱ����
    ������¼���ָ����ʱ�䵽֮ǰ��ȡ���ö�����
�����ߣ�cao-dl
�������ڣ�2015/3/2
������
    event:�¼�
����ֵ����
*******************************************/
void TaskMng_Cancel_Delay( T_DELAY_EVT event )
{
    memset( &m_timerDelay[event], 0, sizeof(m_timerDelay[event]) );
}
/******************************************
���ܣ�ֹͣ��ʱ����
�����ߣ�cao-dl
�������ڣ�2015/3/2
������
    event:�¼�
����ֵ����
*******************************************/
void TaskMng_Stop_Delay( T_DELAY_EVT event )
{
    memset( &m_timerDelay[event], 0, sizeof(m_timerDelay[event]) );
}

/******************************************
���ܣ���ʱ����ʵʩ
    ���¼���ָ����ʱ�䵽���Ժ�ִ�иö�����
�����ߣ�cao-dl
�������ڣ�2015/3/2
��������
����ֵ����
*******************************************/
void TaskMng_DelayAct_Do( void )
{
	u8 i;
	//��¼����������ѭ��ɨ��
	for( i = 0; i < TIMER_DELAY_EVENT_MAX; i ++ )
	{
		//�¼�����
		if( m_EventOccur[i] )
		{
			//��¼�����һص�������Ϊ��
			if( m_timerDelay[i].m_IsRegister && (m_timerDelay[i].m_CbFunc != NULL) )
			{
				( *m_timerDelay[i].m_CbFunc )();
				//һ���Զ���
				if( m_timerDelay[i].m_Timer_Type == TIMER_ONE_COUNT )
				{
					memset( &m_timerDelay[i], 0, sizeof( m_timerDelay[i] ) );
				}
				else if(m_timerDelay[i].m_Timer_Type == TIMER_SET_COUNT)				//�涨�����Ķ���
				{
					m_timerDelay[i].m_count--;
					if(m_timerDelay[i].m_count==0)
					{
						memset( &m_timerDelay[i], 0, sizeof( m_timerDelay[i] ) );
					}
				}
			}
			m_EventOccur[i] = False;
			//��ֹһ���Թ����¼�����
			break;
		}
	}
}
/******************************************
���ܣ���ʱ����
    ��¼������ʱ����м��㣬ʱ�䵽�˺󴥷������¼���
    (ע��2.5ms����)
�����ߣ�cao-dl
�������ڣ�2015/3/2
��������
����ֵ����
*******************************************/
void TaskMng_Delay_Watch( void )
{
    u8 i;
    //��¼����������ѭ��ɨ��
    for( i = 0; i < TIMER_DELAY_EVENT_MAX; i ++ )
    {
        //����ʱ�䲻Ϊ��
        if( m_timerDelay[i].m_TimeCnt > 0 )
        {
            m_timerDelay[i].m_TimeCnt--;
            //ʱ�䵽���Ժ󣬴����¼�
            if( m_timerDelay[i].m_TimeCnt == 0 )
            {
                //�¼�����
                TaskMng_Set_Event( (T_DELAY_EVT)i );
                //�����ԵĶ���������������
                if( m_timerDelay[i].m_Timer_Type != TIMER_ONE_COUNT)
                {
                    m_timerDelay[i].m_TimeCnt = m_timerDelay[i].m_TimeCnt_bak;
                }
            }
        }
    }
}
/******************************************
���ܣ���ʼ��
�����ߣ�cao-dl
�������ڣ�2015/3/2
��������
����ֵ����
*******************************************/
void TaskMng_Init( void )
{
    //��ʼ��
    memset( m_timerDelay, 0, sizeof(m_timerDelay) );
    memset( m_EventOccur, False, sizeof(m_EventOccur) );
}





