/********************** (C) COPYRIGHT 2015 DediBot ******************************
  * @file    Input.c
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-03-28
  * @brief   �������
*********************************************************************************/

/*******************************************
	������ϵ
********************************************/
#include "Input.h"							//IO����
#include "tim.h"
/*******************************************
    ���غ�
********************************************/
#define TIMER_CYCLE_BASE	(TIM7_CYCLE)		//��ʱ����
#define INPUT_WATCH_CYCLE 	10000			//�����	10ms

/*******************************************
    ���ض���
********************************************/
struct St_InputDetect {
	u8 status_first:1;
	u8 status_last:1;
	u8 reversed:6;
};

typedef struct
{
	GPIO_TypeDef *port;
	u16	pin;
}INPUT_SETTING;

/*******************************************
    ����ȫ�ֱ���
********************************************/
struct St_InputDetect  m_InPortState[INPUT_MAX];
static u32	m_TimerCounter;


static const INPUT_SETTING m_InputPortSettings[INPUT_MAX] =
{
///* INPUT_KEY1, */				{GET_PORT(KEY1_PIN), GET_PIN(KEY1_PIN)},
///* INPUT_KEY2, */				{GET_PORT(KEY2_PIN), GET_PIN(KEY2_PIN)},
/* INPUT_LIMIT1, */				{GET_PORT(LIMIT1_PIN),	GET_PIN(LIMIT1_PIN)},
/* INPUT_LIMIT2, */				{GET_PORT(LIMIT2_PIN),	GET_PIN(LIMIT2_PIN)},
/* INPUT_LIMIT3, */				{GET_PORT(LIMIT3_PIN),	GET_PIN(LIMIT3_PIN)},
/* INPUT_LIMIT4, */				{GET_PORT(LIMIT4_PIN),	GET_PIN(LIMIT4_PIN)},
/* INPUT_LIMIT5, */				{GET_PORT(LIMIT5_PIN),	GET_PIN(LIMIT5_PIN)},
/* INPUT_LIMIT6, */				{GET_PORT(LIMIT6_PIN),	GET_PIN(LIMIT6_PIN)},
/* INPUT_LIMIT7, */				{GET_PORT(LIMIT7_PIN),	GET_PIN(LIMIT7_PIN)},
/* INPUT_LIMIT8, */				{GET_PORT(LIMIT8_PIN),	GET_PIN(LIMIT8_PIN)},
/* INPUT_LIMIT9, */				{GET_PORT(LIMIT9_PIN),	GET_PIN(LIMIT9_PIN)},
/* INPUT_LIMIT10, */			{GET_PORT(LIMIT10_PIN),	GET_PIN(LIMIT10_PIN)},
/* INPUT_LIMIT11, */			{GET_PORT(LIMIT11_PIN),	GET_PIN(LIMIT11_PIN)},
/* INPUT_LIMIT12, */			{GET_PORT(LIMIT12_PIN),	GET_PIN(LIMIT12_PIN)},
/* INPUT_LIMIT13, */			{GET_PORT(LIMIT13_PIN),	GET_PIN(LIMIT13_PIN)},
/* INPUT_LIMIT14, */			{GET_PORT(LIMIT14_PIN),	GET_PIN(LIMIT14_PIN)},
/* INPUT_LIMIT15, */			{GET_PORT(LIMIT15_PIN),	GET_PIN(LIMIT15_PIN)},
/* INPUT_LIMIT16, */			{GET_PORT(LIMIT16_PIN),	GET_PIN(LIMIT16_PIN)},
/*  */
/* INPUT_MATERIAL1, */			{GET_PORT(MATERIAL1_CHK_PIN),	GET_PIN(MATERIAL1_CHK_PIN)},
/* INPUT_MATERIAL2, */			{GET_PORT(MATERIAL2_CHK_PIN),	GET_PIN(MATERIAL2_CHK_PIN)},
/* INPUT_MATERIAL3, */			{GET_PORT(MATERIAL3_CHK_PIN),	GET_PIN(MATERIAL3_CHK_PIN)},
/* INPUT_MATERIAL4, */			{GET_PORT(MATERIAL4_CHK_PIN),	GET_PIN(MATERIAL4_CHK_PIN)},
/* INPUT_MATERIAL5, */			{GET_PORT(MATERIAL5_CHK_PIN),	GET_PIN(MATERIAL5_CHK_PIN)},
/* INPUT_MATERIAL6, */			{GET_PORT(MATERIAL6_CHK_PIN),	GET_PIN(MATERIAL6_CHK_PIN)},
/*  */
///* INPUT_MATERIAL_BLOCK1, */	{GET_PORT(MATERIAL1_BLOCK_PIN), GET_PIN(MATERIAL1_BLOCK_PIN)},
///* INPUT_MATERIAL_BLOCK2, */	{GET_PORT(MATERIAL2_BLOCK_PIN), GET_PIN(MATERIAL2_BLOCK_PIN)},

/* INPUT_SD, */					{GET_PORT(SD_EXIST_PIN), GET_PIN(SD_EXIST_PIN)}
};

/*******************************************
	�ⲿ�ο�����
********************************************/

/*******************************************
	�ڲ���������
********************************************/

/*******************************************
		�ڲ�����
*******************************************/
u8 detectInputStatus( enum _INPUT inputPort )
{
	u8 status = LOW;

	if(inputPort < INPUT_MAX)
	{
		status = (u8)HAL_GPIO_ReadPin(m_InputPortSettings[inputPort].port, m_InputPortSettings[inputPort].pin);
	}
	
	return status;
}

/*******************************************
		�ⲿ����
*******************************************/

/*******************************************
���ܣ���ʼ��(ģʽ�趨���˿ڳ�ʼ��)
�����ߣ�cao-dl
�������ڣ�2015/2/10
����:��
����ֵ: ��
********************************************/
void Input_Init(void)
{
	int i;

	for( i = 0; i < INPUT_MAX; i ++)
	{
		m_InPortState[i].status_first = detectInputStatus((enum _INPUT)i);
		m_InPortState[i].status_last = m_InPortState[i].status_first;
	}
	m_TimerCounter = 0;
}

/*******************************************
���ܣ��źż�⣬���μ��һ����ȷ���ź�
�����ߣ�chen-lj
�������ڣ�2017/2/17
����:   ��
����ֵ: ��
********************************************/
void Input_Watch(void)
{
	u8  i;
	u8  status;

	m_TimerCounter += TIMER_CYCLE_BASE;
	if(m_TimerCounter >= INPUT_WATCH_CYCLE)
	{
		m_TimerCounter = 0;
		for( i = 0; i < INPUT_MAX; i ++)
		{
			status = detectInputStatus((enum _INPUT)i);

			if(m_InPortState[i].status_first == status)							//���μ���ǰ��һ��
			{
				m_InPortState[i].status_last = status;							//�õ��µ�״̬
			}
			m_InPortState[i].status_first = status;								//���μ���Ϊǰ��
		}
	}
}
/*******************************************
���ܣ������˿�״̬ȡ��	�з�������
�����ߣ�chen-lj
�������ڣ�2017/2/17
����:   �����˿ڶ���
����ֵ: �˿�״̬
********************************************/
u8 Input_GetState( enum _INPUT inputPort )
{
	return (m_InPortState[inputPort].status_last);
}
/*******************************************
���ܣ������˿�״̬ȡ��	�޷�������
�����ߣ�chen-lj
�������ڣ�2017/2/17
����:   �����˿ڶ���
����ֵ: �˿�״̬
********************************************/
u8 Input_GetDirectState( enum _INPUT inputPort )
{
	return detectInputStatus(inputPort);
}
