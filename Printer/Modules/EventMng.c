/********************** (C) COPYRIGHT 2017 DediBot ******************************
  * @file    EventMng.c
  * @author  Dedibot
  * @version V1.2.0
  * @date    2017-05-25
  * @brief   �¼�����
*********************************************************************************/

/*******************************************
	������ϵ
********************************************/
#include "EventMng.h"					//�¼�����
#include "Queue.h"						//����
#include "PrinterCtrl.h"

/*******************************************
    ���ض���
********************************************/
#define EVENT_QUEUE_NUM				(1<<4)			//�¼����д洢����


/*******************************************
    ���ر���
********************************************/
static QUEUE_STRUCT_t 	m_EventMng[EVENT_LEVEL_MAX];
static EVENT			m_EventQueue[EVENT_LEVEL_MAX][EVENT_QUEUE_NUM];

/*******************************************
	ȫ�ֱ���
********************************************/

/*******************************************
	�ڲ���������
********************************************/

/*******************************************
		�ⲿ����
*******************************************/
/******************************************
���ܣ�
�����ߣ�
�������ڣ�2017-05-25
������
����ֵ��
*******************************************/
void EventMng_SetEvent_Normal(EVENT_NAME e, EVENT_NOTIFY notify)
{
	EVENT event;
	
	event.event = e;
	event.notify = notify;
	
	QUEUE_StructIn(&m_EventMng[EVENT_LEVEL_NORMAL], &event, 1);
}
/******************************************
���ܣ�
�����ߣ�
�������ڣ�2017-05-25
������
����ֵ��
*******************************************/
void EventMng_SetEvent_Level(EVENT_NAME e, EVENT_NOTIFY notify, EVENT_LEVEL level)
{
	EVENT event;
	
	event.event = e;
	event.notify = notify;
	
	QUEUE_StructIn(&m_EventMng[level], &event, 1);
}
/******************************************
���ܣ�
�����ߣ�
�������ڣ�2017-05-25
������
����ֵ��
*******************************************/
bool EventMng_GetEvent(EVENT *event)
{
	int i;
	int count;
//	EVENT null_event ;
	for(i = 0; i < EVENT_LEVEL_MAX; i++)
	{
		count = QUEUE_StructOut(&m_EventMng[i], event, 1);
		if(count > 0)
		{
			return True;
		}
	}
	return False;
}

/******************************************
���ܣ�
�����ߣ�
�������ڣ�2017-05-25
������
����ֵ��
*******************************************/
void EventMng_Init(void)
{
	int i;
	
	for(i = 0; i < EVENT_LEVEL_MAX; i++)
	{
		QUEUE_StructCreate(&m_EventMng[i], m_EventQueue[i], EVENT_QUEUE_NUM*sizeof(EVENT), sizeof(EVENT));
	}
}

