#include "FanCtrl.h"				//风扇控制
#include "tim.h"
#include "de_type.h"

#define TIMER_CYCLE_BASE			(TIM7_CYCLE)
#define PWM_CYCLE_TIME				(100000)
#define PWM_CYCLE_COUNT				(PWM_CYCLE_TIME/TIMER_CYCLE_BASE)

typedef struct
{
	GPIO_TypeDef *port;
	u16	pin;
	u8	level_P;
	u8	level_N;
}SW_SETTING;

typedef struct {
	float percentage;
	u32 Switch_Count;
}FanCtrl;

/*******************************************
	本地变量
********************************************/
 FanCtrl m_Fans[FAN_MAX];	//风扇控制

static long m_PwmCount;	//PWM计数器

static const SW_SETTING m_FanSettings[FAN_MAX] = 
{
				//port					pin					ON		OFF
/* FAN_1 */		{GET_PORT(FAN1_PIN), 	GET_PIN(FAN1_PIN),	LOW,	HIGH},
/* FAN_2 */		{GET_PORT(FAN2_PIN),	GET_PIN(FAN2_PIN),	LOW,	HIGH},
/* FAN_3 */		{GET_PORT(FAN3_PIN),	GET_PIN(FAN3_PIN),	LOW,	HIGH},
/* FAN_4 */		{GET_PORT(FAN4_PIN),	GET_PIN(FAN4_PIN),	LOW,	HIGH},
/* FAN_5 */		{GET_PORT(FAN5_PIN),	GET_PIN(FAN5_PIN),	LOW,	HIGH},
/* FAN_6 */		{GET_PORT(FAN6_PIN),	GET_PIN(FAN6_PIN),	LOW,	HIGH},
/* FAN_7 */		{GET_PORT(FAN7_PIN),	GET_PIN(FAN7_PIN),	LOW,	HIGH},
/* FAN_8 */		{GET_PORT(FAN8_PIN),	GET_PIN(FAN8_PIN),	LOW,	HIGH},
/* FAN_9 */		{GET_PORT(FAN1_PIN),	GET_PIN(FAN1_PIN),	LOW,	HIGH}, //用于控制灯光
/* FAN_10 */	{GET_PORT(FAN1_PIN),	GET_PIN(FAN1_PIN),	LOW,	HIGH}, //用于控制刹车
/* FAN_11 */	{GET_PORT(FAN1_PIN),	GET_PIN(FAN1_PIN),	LOW,	HIGH}, //用于控制激光
/* FAN_12 */	{GET_PORT(FAN1_PIN),	GET_PIN(FAN1_PIN),	LOW,	HIGH}  //用于控制CNC
};

/*******************************************
功能：风扇控制
作成者：liu-cy
作成日期：2017/3/2
参数:
返回值:无
********************************************/

static void FanCtrl_ON(_FAN fan)	//打开风扇
{
	HAL_GPIO_WritePin(m_FanSettings[fan].port, m_FanSettings[fan].pin, (GPIO_PinState)m_FanSettings[fan].level_P);
}
static void FanCtrl_OFF(_FAN fan)	//关闭风扇
{
	HAL_GPIO_WritePin(m_FanSettings[fan].port, m_FanSettings[fan].pin, (GPIO_PinState)m_FanSettings[fan].level_N);
}


/*******************************************
功能：风扇控制初始化
作成者：liu-cy
作成日期：2017/3/2
参数:
返回值:无
********************************************/
void FanCtrl_Init(void)
{
	u8 i;
	for(i = 0; i < FAN_MAX; i++)
	{
		FanCtrl_SetFan((_FAN)i, 100);
	}
}


/*******************************************
功能：风扇管理
作成者：liu-cy
作成日期：2017/3/2
参数:
返回值:无
********************************************/
void FanCtrl_Manage_FAN(void)
{
	u8 i;
	for(i = 0; i < FAN_MAX; i++)
	{
		if(m_PwmCount < m_Fans[i].Switch_Count)		//打开状态溢出则关闭风扇,清除状态
		{
			FanCtrl_ON((_FAN)i);		//打开风扇
		}
		else
		{
			FanCtrl_OFF((_FAN)i);		//关闭风扇
		}			
	}
	if(++m_PwmCount >= PWM_CYCLE_COUNT)
	{
		m_PwmCount = 0;
	}
}

/*******************************************
功能：设置风扇占空比
作成者：liu-cy
作成日期：2017/3/2
参数:
返回值:无
********************************************/
void FanCtrl_SetFan(_FAN fan, float percentage)
{
	if(percentage <= 100 && percentage >=0)
	{
		m_Fans[fan].percentage = percentage;
		m_Fans[fan].Switch_Count = (u32)(m_Fans[fan].percentage/100*PWM_CYCLE_COUNT);
	}
}
