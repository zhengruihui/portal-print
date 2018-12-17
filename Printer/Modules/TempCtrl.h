/********************** (C) COPYRIGHT 2011 STMicroelectronics ********************
  * @file    TempCtrl.h
  * @author  Dedibot
  * @version V1.2.0
  * @date    2017-05-21
  * @brief   ���ȿ���
*********************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TEMP_CTRL_H
#define TEMP_CTRL_H
#include "de_type.h"
#include "tim.h"
#include "heatctrl.h"
#include "TempSensor.h"						//�¶ȼ��
#include "ErrorMng.h"

#define TEMPRANGE  401

#define MAXTEMP							360
#define HEADTEM_CHECK_RABGE				20			//��ͷ�쳣��ⷶΧ
#define HEADTEM_CHECK_TIME				5000		//��ͷ�쳣�����10s
#define MAX_TEMPOOVER_CNT				50			//�¶ȹ�������������
#define MAX_TEMPLOWER_CNT				500			//�¶ȹ�������������
#define MAX_TEMPOVER_TIME				900			//���ȳ�ʱʱ�䣨��λ���룩
#define TEMPOVER_SCAN_TIME				1000		//���Ȳ�ѯʱ�䣨1ms*1000=1s��
#define SILKLESS_TEMP					10			//ȱ��ʱ��ȡ�����¶�ADֵ
#define MAX_SILKLESS_CNT				3			//ȱ���ж���������
#define MAX_TEMP_DEV					10			//�¶ȳ�ʱ�ж�����¶�ƫ������
#define STOP_TEMP_DEV                   30          //����ӡ�������²�ﵽ30�ȳ���10S��ֹͣ��ӡ
#define STOP_TEMPOVER_TIME              10          //��С��20��ʱ�ܹ�����ʱ��
#define TEMP_REACH						True		//�¶ȵ����ж�
#define TEMP_UNREACH					False		//�¶�δ�����ж�
#define MAX_PRINTINGTEMPOVER_TIME       10
#define MAX_PRINTINGTEMP_DEV            30


/***** PID������ض��� *****/
#define PID_GAIN						(1000)
#define PID_LIMIT_MAX					(PID_GAIN)
#define PID_LIMIT_MIN					(0)
#define PID_DATA_GAIN					(10000)
#define PID_DATA_LIMIT_MAX				(PID_GAIN*PID_DATA_GAIN)
#define PID_DATA_LIMIT_MIN				(0)

#define PID_DIR_PLUS					(0)											//+����
#define PID_DIR_MINUS					(1)											//-����

#define PID_KP							(PID_GAIN*PID_DATA_GAIN*1/100)//(PID_GAIN*PID_DATA_GAIN*2/100)					//1��=2%
#define PID_KIT							(20*1000000/TIMER_CYCLE_CALC)	 				//30s
#define PID_KP_OPSTGAIN					(5)						 					//��ǰ�¶�>Ŀ���¶��һ������� �� ��ǰ�¶�<Ŀ���¶��һ��ڽ���ʱ��P��������

/***** �̶�ռ�ձ���ض��� *****/
#define FIXED_DUTY_100					(1000)
#define FIXED_DUTY_50					(500)
#define FIXED_GAIN						(1000)

/***** ��ϼ�����ض��� *****/
#define TEMP_ENVIRONMENT				(20)
#if POWER_TYPE==POWER_24V
	/* �뻷��ÿ���1�棬������ʧ�ı��������� */
	#define TEMP_EXT_LOSSRATE			(0.00074008132)				//����ɢ��ϵ��
	#define TEMP_EXT_IDEAL				(0.133285312)
#else /* POWER_TYPE==POWER_19V */
	/* �뻷��ÿ���1�棬������ʧ�ı��������� */
	#define TEMP_EXT_LOSSRATE			(0.00105969108)				//����ɢ��ϵ��
	#define TEMP_EXT_IDEAL				(0.201380909)
#endif
/* ��Ŀ���ٶ����5�����ڣ����¶ȱ仯�ٶ�<0.5��/s�ı仯�ٶ�ʱ���л�ΪPID���� */
#define TEMP_EXT_SWCOND_TEMP			(5)
#define TEMP_EXT_SWCOND_SPD				(0.1)

/***** PWM������� *****/
#define TIMER_CYCLE_CALC				(TIM7_CYCLE)		//ռ�ձȼ���		5ms
#define TIMER_CYCLE_OUTPUT				(TIM6_CYCLE)		//ռ�ձ�������	0.5ms
#define PWM_TIMER_CYCLE					(100000)	//ռ�ձ�����		100ms
#define PWM_COUNT_CYCLE					(PWM_TIMER_CYCLE/TIMER_CYCLE_OUTPUT)
#define PWM_INCREMENT					(1)
#define PWM_SETDUTY(obj,duty,gain)		(obj->HeatCtrl.pwm_ON_OFF_Switch = duty*PWM_COUNT_CYCLE/gain)

/***** ʱ�䶨�� *****/
#define TIMER_COUNT_500					(500000/TIMER_CYCLE_CALC)				//500ms



// ���Ȳ���
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

/* ���ȿ��Ʒ�ʽ���� */
enum _HEATTYPE
{
	HEATTYPE_FIXED,
	HEATTYPE_PID,
	HEATTYPE_MIXED,
	
	HEATTYPE_MAX
};  

/* ���ȿ���״̬���� */
enum _HEATSTATUS
{
	HEATSTATUS_FIXED_SET,
	HEATSTATUS_FIXED_HEATING,
	HEATSTATUS_PID_HEATING,
	
	HEATSTATUS_MAX
};

// �����쳣code����
typedef struct
{
	s16					temp_OverHeat;
	ERROR_NAME			err_OverHeat;							//�¶ȹ����쳣code
	ERROR_NAME			err_OverTime;							//���ȳ�ʱ�쳣code
	ERROR_NAME			err_SensorErr;							//�������쳣code
	ERROR_NAME			err_NotRise;							//����ʱ�¶Ȳ�����
	ERROR_NAME			err_NotAvailable;						//�豸������
}ERRORS;

// �����쳣code����
typedef struct
{
	/* ���ȳ�ʱ��� */
	u32 		Heat_OverTime;							//���ȳ�ʱʱ��
	u16 		HeatTime;								//���ȼ�ʱ
	/* �¶ȹ��߼�� */
	u8 			TempOverCnt;							//�¶ȹ��߼���
	/* �������쳣��� */
	u8 			SensorNoConnectCnt;						//�¶ȴ�����δ���Ӽ���
	u8 			SensorBadCnt;							//�¶ȴ������𻵼���
	TEMP_STATUS	Sensor_Status;                       	//��ͷ�¶ȴ�����״̬
	/* ����ʱ�¶Ȳ������쳣��� */
	float		HeadErrorStartTemp;						//��ͷ�쳣��ʼ�¶�
	u16			HeadErrorTime;							//�������𻵼�ʱ

	ERRORS		Errors;
}ERRORCTRL;

typedef struct
{
	s32 		i_Integral;
	s32 		i_IntegralTime;
	s32 		p_Gain;									//1���϶ȵĲ�Ӱ���ռ�ձ�
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
	s16				pwm_FixedDuty;							//�̶�ռ�ձ�ʱ�����ռ�ձ�
	s16 			pwm_ON_OFF_Switch;
	s16 			pwm_Count;
	
	s16				timeCounter_500;
	float 			lastTemp_500;							//500msǰ�¶�
	float 			tempChg_500;							//�¶ȱ仯��ÿ500ms��
	float 			tempChg_TC;								//�¶ȱ仯��ÿTIMER_CYCLE���ڣ�
	float 			tempDiff;								//��Ŀ���¶Ȳ�
} HEATCTRL;

typedef struct 
{
	_HEATER	heater;
	_TEMPSENSOR	sensor;
}TEMPDEVICE;

typedef struct
{
	TEMPDEVICE	device;
	HEATCTRL	HeatCtrl;								//���ȿ��ƿ�
	float		CurrentTemp;							//��ǰ�¶�
	float		LastTemp;								//ǰ���¶�
	float		TargetTemp;								//Ŀ���¶�
	ERRORCTRL	errCtrl;
	struct {
		u8 isAvailable:1;								//�豸�Ƿ����
		u8 Temp_Reach:1;								//�¶ȵ����־
		u8 WaitTempReached:1;
		u8 Reversed:6;
	}CtrlFlag;
} TEMPCTRL_OBJ;

extern float TempCtrl_getCurrentTemp( enum TEMPCTRL_ITEMS item );

//���ü����豸�Ƿ����
extern void TempCtrl_setAvailable(enum TEMPCTRL_ITEMS item, bool value );
//�¶�Ŀ���趨
extern void TempCtrl_setTargetTemp(enum TEMPCTRL_ITEMS item, s16 temp, bool );

//�¶�ȡ��
extern s16 TempCtrl_getTargetTemp(  enum TEMPCTRL_ITEMS item  );
//ֹͣ����
extern void TempCtrl_StopHeat( enum TEMPCTRL_ITEMS item );
//�¶ȹ���
extern void TempCtrl_Manage_Heater(void);
//0.5ms����
extern void TempCtrl_Timer_0_5( void );
//����Ŀ���¶ȵȴ�
extern bool TempCtrl_IsWaitForTemp( void );
extern bool TempCtrl_IsWaitForTempSingle(u8 item);
//��ʼ��
extern void TempCtrl_Init( void );
extern void TempCtrl_Clear( void );

#endif  //TEMP_CTRL_H
