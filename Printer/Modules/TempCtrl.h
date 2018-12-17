/********************** (C) COPYRIGHT 2011 STMicroelectronics ********************
  * @file    TempCtrl.h
  * @author  Dedibot
  * @version V1.2.0
  * @date    2017-05-21
  * @brief   风扇控制
*********************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TEMP_CTRL_H
#define TEMP_CTRL_H
#include "de_type.h"
#include "tim.h"
#include "heatctrl.h"
#include "TempSensor.h"						//温度检测
#include "ErrorMng.h"

#define TEMPRANGE  401

#define MAXTEMP							360
#define HEADTEM_CHECK_RABGE				20			//喷头异常检测范围
#define HEADTEM_CHECK_TIME				5000		//喷头异常检测间隔10s
#define MAX_TEMPOOVER_CNT				50			//温度过高允许最大次数
#define MAX_TEMPLOWER_CNT				500			//温度过低允许最大次数
#define MAX_TEMPOVER_TIME				900			//加热超时时间（单位：秒）
#define TEMPOVER_SCAN_TIME				1000		//加热查询时间（1ms*1000=1s）
#define SILKLESS_TEMP					10			//缺料时获取到的温度AD值
#define MAX_SILKLESS_CNT				3			//缺料判定的最大次数
#define MAX_TEMP_DEV					10			//温度超时判断最大温度偏移允许
#define STOP_TEMP_DEV                   30          //当打印过程中温差达到30度长达10S将停止打印
#define STOP_TEMPOVER_TIME              10          //当小于20度时能工作的时间
#define TEMP_REACH						True		//温度到达判定
#define TEMP_UNREACH					False		//温度未到达判定
#define MAX_PRINTINGTEMPOVER_TIME       10
#define MAX_PRINTINGTEMP_DEV            30


/***** PID控制相关定义 *****/
#define PID_GAIN						(1000)
#define PID_LIMIT_MAX					(PID_GAIN)
#define PID_LIMIT_MIN					(0)
#define PID_DATA_GAIN					(10000)
#define PID_DATA_LIMIT_MAX				(PID_GAIN*PID_DATA_GAIN)
#define PID_DATA_LIMIT_MIN				(0)

#define PID_DIR_PLUS					(0)											//+方向
#define PID_DIR_MINUS					(1)											//-方向

#define PID_KP							(PID_GAIN*PID_DATA_GAIN*1/100)//(PID_GAIN*PID_DATA_GAIN*2/100)					//1℃=2%
#define PID_KIT							(20*1000000/TIMER_CYCLE_CALC)	 				//30s
#define PID_KP_OPSTGAIN					(5)						 					//当前温度>目标温度且还在升温 或 当前温度<目标温度且还在降温时，P项增大倍数

/***** 固定占空比相关定义 *****/
#define FIXED_DUTY_100					(1000)
#define FIXED_DUTY_50					(500)
#define FIXED_GAIN						(1000)

/***** 混合加热相关定义 *****/
#define TEMP_ENVIRONMENT				(20)
#if POWER_TYPE==POWER_24V
	/* 与环境每相差1℃，热能损失的比率增加量 */
	#define TEMP_EXT_LOSSRATE			(0.00074008132)				//喷嘴散热系数
	#define TEMP_EXT_IDEAL				(0.133285312)
#else /* POWER_TYPE==POWER_19V */
	/* 与环境每相差1℃，热能损失的比率增加量 */
	#define TEMP_EXT_LOSSRATE			(0.00105969108)				//喷嘴散热系数
	#define TEMP_EXT_IDEAL				(0.201380909)
#endif
/* 与目标速度相差5℃以内，且温度变化速度<0.5℃/s的变化速度时，切换为PID控制 */
#define TEMP_EXT_SWCOND_TEMP			(5)
#define TEMP_EXT_SWCOND_SPD				(0.1)

/***** PWM控制相关 *****/
#define TIMER_CYCLE_CALC				(TIM7_CYCLE)		//占空比计算		5ms
#define TIMER_CYCLE_OUTPUT				(TIM6_CYCLE)		//占空比输出间隔	0.5ms
#define PWM_TIMER_CYCLE					(100000)	//占空比周期		100ms
#define PWM_COUNT_CYCLE					(PWM_TIMER_CYCLE/TIMER_CYCLE_OUTPUT)
#define PWM_INCREMENT					(1)
#define PWM_SETDUTY(obj,duty,gain)		(obj->HeatCtrl.pwm_ON_OFF_Switch = duty*PWM_COUNT_CYCLE/gain)

/***** 时间定义 *****/
#define TIMER_COUNT_500					(500000/TIMER_CYCLE_CALC)				//500ms



// 加热部件
enum TEMPCTRL_ITEMS {
	TEMPCTRL_EXTRUDER1,
	TEMPCTRL_EXTRUDER2_1,
	TEMPCTRL_EXTRUDER2_2,
	TEMPCTRL_EXTRUDER2_3,
	TEMPCTRL_EXTRUDER2_4,
	TEMPCTRL_EXTRUDER3_1,
	TEMPCTRL_EXTRUDER3_2,
	
	TEMPCTRL_ITEMMAX 
};
#define TEMPCTRL_ITEM_ALL	TEMPCTRL_ITEMMAX

/* 加热控制方式定义 */
enum _HEATTYPE
{
	HEATTYPE_FIXED,
	HEATTYPE_PID,
	HEATTYPE_MIXED,
	
	HEATTYPE_MAX
};  

/* 加热控制状态定义 */
enum _HEATSTATUS
{
	HEATSTATUS_FIXED_SET,
	HEATSTATUS_FIXED_HEATING,
	HEATSTATUS_PID_HEATING,
	
	HEATSTATUS_MAX
};

// 加热异常code定义
typedef struct
{
	s16					temp_OverHeat;
	ERROR_NAME			err_OverHeat;							//温度过高异常code
	ERROR_NAME			err_OverTime;							//加热超时异常code
	ERROR_NAME			err_SensorErr;							//传感器异常code
	ERROR_NAME			err_NotRise;							//加热时温度不上升
	ERROR_NAME			err_NotAvailable;						//设备不可用
}ERRORS;

// 加热异常code定义
typedef struct
{
	/* 加热超时检测 */
	u32 		Heat_OverTime;							//加热超时时间
	u16 		HeatTime;								//加热计时
	/* 温度过高检测 */
	u8 			TempOverCnt;							//温度过高计数
	/* 传感器异常检测 */
	u8 			SensorNoConnectCnt;						//温度传感器未连接计数
	u8 			SensorBadCnt;							//温度传感器损坏计数
	TEMP_STATUS	Sensor_Status;                       	//喷头温度传感器状态
	/* 加热时温度不上升异常检测 */
	float		HeadErrorStartTemp;						//喷头异常起始温度
	u16			HeadErrorTime;							//加热器损坏计时

	ERRORS		Errors;
}ERRORCTRL;

typedef struct
{
	s32 		i_Integral;
	s32 		i_IntegralTime;
	s32 		p_Gain;									//1摄氏度的差影响的占空比
	s16 		p_OppositeGain;
	s16 		pid_Output;								//1000=100%
	s8			pid_Dir;
	
	#if DEBUG==1
	s32 		p_Value;
	#endif
} PIDDATA;

typedef struct
{
	enum _HEATSTATUS heatStatus;
	float			lossRate;
	float			idealRise;
	float			switchCond_Temp;
	float			switchCond_Spd;
	s16				timeCounter;
} MIXEDHEATCTRL;

typedef struct
{
	PIDDATA			pidData;
	MIXEDHEATCTRL	mixedHeatCtrl;
	enum _HEATTYPE 	heatType;
	s16				pwm_FixedDuty;							//固定占空比时的输出占空比
	s16 			pwm_ON_OFF_Switch;
	s16 			pwm_Count;
	
	s16				timeCounter_500;
	float 			lastTemp_500;							//500ms前温度
	float 			tempChg_500;							//温度变化（每500ms）
	float 			tempChg_TC;								//温度变化（每TIMER_CYCLE周期）
	float 			tempDiff;								//与目标温度差
} HEATCTRL;

typedef struct 
{
	_HEATER	heater;
	_TEMPSENSOR	sensor;
}TEMPDEVICE;

typedef struct
{
	TEMPDEVICE	device;
	HEATCTRL	HeatCtrl;								//加热控制块
	float		CurrentTemp;							//当前温度
	float		LastTemp;								//前次温度
	float		TargetTemp;								//目标温度
	ERRORCTRL	errCtrl;
	struct {
		u8 isAvailable:1;								//设备是否可用
		u8 Temp_Reach:1;								//温度到达标志
		u8 WaitTempReached:1;
		u8 Reversed:6;
	}CtrlFlag;
} TEMPCTRL_OBJ;

extern float TempCtrl_getCurrentTemp( enum TEMPCTRL_ITEMS item );

//设置加热设备是否可用
extern void TempCtrl_setAvailable(enum TEMPCTRL_ITEMS item, bool value );
//温度目标设定
extern void TempCtrl_setTargetTemp(enum TEMPCTRL_ITEMS item, s16 temp, bool );

//温度取得
extern s16 TempCtrl_getTargetTemp(  enum TEMPCTRL_ITEMS item  );
//停止加热
extern void TempCtrl_StopHeat( enum TEMPCTRL_ITEMS item );
//温度管理
extern void TempCtrl_Manage_Heater(void);
//0.5ms周期
extern void TempCtrl_Timer_0_5( void );
//升至目标温度等待
extern bool TempCtrl_IsWaitForTemp( void );
extern bool TempCtrl_IsWaitForTempSingle(u8 item);
//初始化
extern void TempCtrl_Init( void );
extern void TempCtrl_Clear( void );

#endif  //TEMP_CTRL_H
