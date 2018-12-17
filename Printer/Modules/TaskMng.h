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
    TIMER_ONE_COUNT,        //һ���Լ���
    TIMER_INTERVAL,         //�ظ�����
    TIMER_SET_COUNT,        //�涨����
    TIMER_TYPE_MAX
} T_TYPE;
//����
typedef enum TIMER_DELAY_EVENT
{
    TIMER_DELAY_PRINT,
    TIMER_DELAY_WARNING,	
	TIMER_DELAY_PRINT_TEMPREP,
	TIMER_DELAY_COUNT,
    TIMER_DELAY_MATERIAL,
	TIMER_DELAY_LINK,		//��������
	TIMER_DELAY_ACK_RETRY,//ack�����ط�
    TIMER_DELAY_EVENT_MAX
} T_DELAY_EVT;
//CB����
typedef void (*cbfun)(void);
typedef void (*ackcbfun)(void);
//��ʱ�����鱨
typedef struct
{
    bool        m_IsRegister;   //ע����
    cbfun       m_CbFunc;       //�ص�����
		ackcbfun		m_AckCbFunc;
    u32         m_TimeCnt;      //����ʱ��
    u32         m_TimeCnt_bak;  //����ʱ��(����)
    T_TYPE      m_Timer_Type;   //��������
	u32         m_count;      	//��������
} timerDelay;

/*******************************************
	����ӿ�
********************************************/

//�趨��ʱ����
extern void TaskMng_Set_Delay( T_DELAY_EVT , cbfun, u32, T_TYPE, u32 count);
//�趨��ʱ����
extern void TaskMng_AckSet_Delay( T_DELAY_EVT event, ackcbfun cbfunc, u32 timecnt, T_TYPE timetype , u32 count);
//ȡ����ʱ����
extern void TaskMng_Cancel_Delay( T_DELAY_EVT );
//ֹͣ��ʱ����
extern void TaskMng_Stop_Delay( T_DELAY_EVT );
//��ʱ����ʵʩ
extern void TaskMng_DelayAct_Do( void );
//��ʱ����
extern void TaskMng_Delay_Watch( void );
//��ʼ��
extern void TaskMng_Init( void );
#endif
