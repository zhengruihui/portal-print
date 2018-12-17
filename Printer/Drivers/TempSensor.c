/********************** (C) COPYRIGHT 2017 STMicroelectronics ********************
  * @file    TempSensor.c
  * @author  Dedibot
  * @version V1.2.0
  * @date    2017-07-05
  * @brief   温度传感器检测
*********************************************************************************/
#include "TempSensor.h"
#include "tim.h"
#include "adc.h"
#include "main.h"

#define TIMER_CYCLE_BASE	(TIM7_CYCLE)
#define SAMPLE_INTERVAL		(10000)
#define	TEMP_OFFSET			(50)

#define TEMP_ERROR_VALUE	(9999)

static s32 m_TimerCounter;
static s8 m_SamplingCounter;
u16 m_Temp_SampleData[SAMPLE_COUNT][TEMPSENSOR_MAX];

float m_Temperature[TEMPSENSOR_MAX];
TEMP_STATUS m_TempStatus[TEMPSENSOR_MAX];

const u16 m_TempValue[]={
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 									 //-50~-41
		1, 1, 1, 1, 1, 1, 1, 1, 2, 2,  									 //-40~-31
		2, 2, 2, 2, 3, 3, 3, 3, 3, 4,  									 //-30~-21
		4, 4, 4, 5, 5, 5, 5, 6, 6, 7,  									 //-20~-11
		7, 7, 8, 8, 9, 9, 10, 10, 11, 11,  								 //-10~-1
		12, 13, 13, 14, 15, 16, 16, 17, 18, 19,							 //0~9
		20, 21, 22, 23, 24, 25, 27, 28, 29, 31,                          //10~19
		32, 33, 35, 37, 38, 40, 42, 44, 45, 47,                          //20~29
		49, 52, 54, 56, 58, 61, 64, 66, 69, 72,                          //30~39
		75, 78, 81, 84, 87, 91, 95, 98, 102, 106,                        //40~49
		111, 114, 119, 123, 127, 132, 136, 141, 146, 152,                //50~59
		157, 163, 168, 174, 180, 187, 193, 200, 206, 213,                //60~69
		220, 228, 235, 243, 251, 259, 268, 276, 284, 293,                //70~79
		302, 311, 321, 331, 341, 351, 361, 372, 383, 394,                //80~89
		405, 417, 429, 441, 453, 466, 478, 491, 504, 517,                //90~99
		531, 544, 557, 570, 584, 597, 612, 628, 643, 658,                //100~109
		674, 690, 707, 723, 740, 757, 774, 791, 809, 826,                //110~119
		844, 863, 883, 903, 923, 943, 962, 981, 1000, 1020,              //120~129
		1039, 1060, 1081, 1101, 1122, 1144, 1165, 1186, 1208, 1230,      //130~139
		1252, 1274, 1297, 1319, 1342, 1365, 1388, 1410, 1433, 1456,      //140~149
		1478, 1499, 1519, 1540, 1561, 1582, 1605, 1627, 1650, 1673,      //150~159
		1696, 1720, 1743, 1767, 1790, 1813, 1836, 1860, 1883, 1906,      //160~169
		1930, 1954, 1978, 2002, 2026, 2051, 2072, 2095, 2116, 2138,      //170~179
		2160, 2184, 2208, 2232, 2243, 2279, 2299, 2320, 2341, 2362,      //180~189
		2382, 2405, 2427, 2449, 2471, 2493, 2512, 2531, 2550, 2569,      //190~199
		2589, 2605, 2622, 2639, 2656, 2673, 2689, 2708, 2727, 2743,      //200~209
		2761, 2780, 2797, 2815, 2832, 2850, 2868, 2884, 2900, 2917,      //210~219
		2934, 2951, 2965, 2981, 2996, 3011, 3027, 3040, 3056, 3070,      //220~229
		3084, 3098, 3112, 3126, 3138, 3153, 3165, 3177, 3190, 3202,      //230~239
		3215, 3227, 3240, 3250, 3263, 3274, 3287, 3297, 3308, 3319,      //240~249
		3330, 3340, 3351, 3360, 3371, 3382, 3390, 3399, 3410, 3419,      //250~259
		3427, 3439, 3447, 3456, 3488, 3474, 3480, 3488, 3497, 3506,      //260~269
		3512, 3521, 3531, 3537, 3543, 3552, 3558, 3567, 3574, 3580,      //270~279
		3586, 3592, 3599, 3608, 3615, 3621, 3624, 3631, 3637, 3644,      //280~289
		3650, 3657, 3660, 3666, 3673, 3680, 3683, 3690, 3693, 3700,      //290~299
		3704, 3709, 3714, 3719, 3724, 3729, 3734, 3738, 3743, 3748,      //300~309 
		3752, 3757, 3761, 3766, 3770, 3774, 3778, 3783, 3787, 3791,      //310~319 
		3795, 3799, 3803, 3807, 3811, 3815, 3819, 3822, 3826, 3830,      //320~329 
		3833, 3837, 3840, 3844, 3848, 3851, 3855, 3858, 3861, 3864,      //330~339 
		3868, 3871, 3874, 3877, 3880, 3883, 3886, 3889, 2691, 3895,      //340~349  
		3898      																											//350
};
const u16 m_TempValueSize = sizeof(m_TempValue)/sizeof(u16);


/*******************************************
功能：温度传感处理，取得当前温度值
作成者：deng-xy
作成日期：2015/5/14
参数:   无
返回值: 当前温度
********************************************/
// void TempCtrl_GetCurrentTemp(void)
// {
	// ADC_ChannelConfTypeDef ADC1_ChanConf;
	
	// ADC1_ChanConf.Rank=1;                                       				//第1个序列，序列1	
	// ADC1_ChanConf.Channel=ADC_CHANNEL_1;                                   		//通道
	// ADC1_ChanConf.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	// ADC1_ChanConf.Offset=0;   
	
	// HAL_ADC_ConfigChannel(&hadc1,&ADC1_ChanConf);        						//通道配置
	
	// HAL_ADC_Start(&hadc1);
	// HAL_ADC_PollForConversion(&hadc1, 10);
	// if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
	// {
		// m_TempCtrl_Obj_Tbl[TEMPCTRL_EXTRUDER].ADCValue[TempCtrl_Ex_Cnt] = HAL_ADC_GetValue(&hadc1);
		// TempCtrl_Ex_Cnt++;
		// if(TempCtrl_Ex_Cnt > TEM_ADC_VALUE - 1)
		// {
			// TempCtrl_Ex_Cnt = 0;
		// }
	// }
	// HAL_ADC_Stop(&hadc1);
	
// //  sConfig.Channel = ADC_CHANNEL_1;
// //  sConfig.Rank = 1;
// //  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	
	// ADC1_ChanConf.Rank=1;                                       				//第1个序列，序列1	
	// ADC1_ChanConf.Channel=ADC_CHANNEL_2;                                   		//通道
	// ADC1_ChanConf.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	// ADC1_ChanConf.Offset=0;   
	
	// HAL_ADC_ConfigChannel(&hadc1,&ADC1_ChanConf);        						//通道配置
	
	// HAL_ADC_Start(&hadc1);
	// HAL_ADC_PollForConversion(&hadc1, 10);	
	// if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
	// {
		// m_TempCtrl_Obj_Tbl[TEMPCTRL_BED].ADCValue[TempCtrl_Bed_Cnt] = HAL_ADC_GetValue(&hadc1);
		// TempCtrl_Bed_Cnt++;
		// if(TempCtrl_Bed_Cnt > TEM_ADC_VALUE - 1)
		// {
			// TempCtrl_Bed_Cnt = 0;
		// }
	// }
	// HAL_ADC_Stop(&hadc1);
// }


/*******************************************************************
功能：根据AD值查表获取对应温度
作成者：chen-lj
作成日期：2016/10/17
参数：u16 *inAD		温度AD值
返回值： 实际温度（表中查不到时返回351或-51）
********************************************************************/
TEMP_STATUS TempCtrl_Conv_ADtoTemp(u32 inAD, float *outTemp)
{
	u32 inADTemp = inAD;
	s16 left = 0;
	s16 right = m_TempValueSize - 1;
	s16 cur;
	u32 curAD;
	float temp;
	
	if(m_TempValue[right] < inADTemp)  
	{
		*outTemp = TEMP_ERROR_VALUE;
		return TEMP_ERROR_LOW;		//过低异常
	}
	if(m_TempValue[left] > inADTemp)
	{
		*outTemp = TEMP_ERROR_VALUE;
		return TEMP_ERROR_LOW;		//过高异常
	}
	while(1)
	{
		cur = (left+right)>>1;
		curAD = m_TempValue[cur];
		if(inADTemp > curAD)
		{
			left = cur + 1;
		}
		else if(inADTemp < curAD)
		{
			right = cur;
		}
		else
		{
			temp = cur;
			break;
		}
		if(left == right)
		{
			if(0 == left)	//由于开头的判断，此处不会成立
			{
				temp = -1;
			}
			else
			{
				left--;
				temp = left + (float)(inADTemp - m_TempValue[left])/(m_TempValue[right] - m_TempValue[left]);
			}
			break;
		}
	}
	*outTemp = temp - TEMP_OFFSET;
	return TEMP_OK;
}


/*******************************************
功能：温度传感处理，取得当前温度值
作成者：deng-xy
作成日期：2015/5/14
参数:   无
返回值: 当前温度
********************************************/
void TempCtrl_CalCurrentTemp(void)
{
	int sensor;
	int sample;
	int anaValue = 0;
	
	for(sensor = 0; sensor < TEMPSENSOR_MAX; sensor++)
	{
		anaValue = 0;
		for(sample = 0; sample < SAMPLE_COUNT; sample++)
		{
			anaValue += m_Temp_SampleData[sample][sensor];
		}
		anaValue = anaValue >> 3;
		m_TempStatus[sensor] = TempCtrl_Conv_ADtoTemp(anaValue, &m_Temperature[sensor]);
	}
}
/*******************************************
功能：初始化
作成者：
作成日期：
参数:
返回值:无
********************************************/
void TempSensor_Init(void)
{
	m_TimerCounter = 0;
	m_SamplingCounter = 0;
	// TempConvert_Start();
}
/*******************************************
功能：初始化
作成者：
作成日期：
参数:
返回值:无
********************************************/
void TempSensor_Timer_5_0(void)
{
	m_TimerCounter += TIMER_CYCLE_BASE;
	
	if(m_TimerCounter >= SAMPLE_INTERVAL)
	{
		m_TimerCounter = 0;
		
		TempConvert_Start((uint32_t *)m_Temp_SampleData[m_SamplingCounter], TEMPSENSOR_MAX);
		
		m_SamplingCounter++;
		if(m_SamplingCounter >= SAMPLE_COUNT)	m_SamplingCounter = 0;
		
		TempCtrl_CalCurrentTemp();
		
	}
}

/*******************************************
功能：
作成者：
作成日期：
参数:
返回值:无
********************************************/
TEMP_STATUS TempSensor_GetTemperature(_TEMPSENSOR sensor, float *temp)
{
	*temp = m_Temperature[sensor];
	return m_TempStatus[sensor];
}

