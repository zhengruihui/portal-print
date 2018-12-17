#include "HeatCtrl.h"
#include "main.h"

typedef struct
{
	GPIO_TypeDef *port;
	u16	pin;
	u8	level_P;
	u8	level_N;
}SW_SETTING;


static const SW_SETTING m_HeatSettings[HEATER_MAX] = 
{
				//port					pin						ON		OFF
/* HEATER1 */	{GET_PORT(HEATER1_PIN), GET_PIN(HEATER1_PIN),	LOW,	HIGH},
/* HEATER2 */	{GET_PORT(HEATER2_PIN),	GET_PIN(HEATER2_PIN),	LOW,	HIGH},
/* HEATER3 */	{GET_PORT(HEATER3_PIN),	GET_PIN(HEATER3_PIN),	LOW,	HIGH},
/* HEATER4 */	{GET_PORT(HEATER4_PIN),	GET_PIN(HEATER4_PIN),	LOW,	HIGH},
/* HEATER5 */	{GET_PORT(HEATER5_PIN),	GET_PIN(HEATER5_PIN),	LOW,	HIGH},
/* HEATER6 */	{GET_PORT(HEATER6_PIN),	GET_PIN(HEATER6_PIN),	LOW,	HIGH},
///* HEATER7 */	{GET_PORT(HEATER7_PIN),	GET_PIN(HEATER7_PIN),	LOW,	HIGH},
///* HEATER8 */	{GET_PORT(HEATER8_PIN),	GET_PIN(HEATER8_PIN),	LOW,	HIGH}
};


/*******************************************

作成者：
作成日期：
参数:
返回值:无
********************************************/
void HeatCtrl_On(_HEATER heater)
{
	HAL_GPIO_WritePin(m_HeatSettings[heater].port, m_HeatSettings[heater].pin, (GPIO_PinState)m_HeatSettings[heater].level_P);
}
/*******************************************

作成者：
作成日期：
参数:
返回值:无
********************************************/
void HeatCtrl_Off(_HEATER heater)
{
	HAL_GPIO_WritePin(m_HeatSettings[heater].port, m_HeatSettings[heater].pin, (GPIO_PinState)m_HeatSettings[heater].level_N);
}

/*******************************************
功能：初始化
作成者：
作成日期：
参数:
返回值:无
********************************************/
void HeatCtrl_Init(void)
{
	u8 i;
	
	for(i = 0; i < HEATER_MAX; i++)
	{
		HeatCtrl_Off((_HEATER)i);
	}
}

