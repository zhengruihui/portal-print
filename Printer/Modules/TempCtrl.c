/********************** (C) COPYRIGHT 2015 DediBot ******************************
  * @file    TempCtrl.c
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-03-25
  * @brief   �¶ȿ���
*********************************************************************************/
/*******************************************
	������ϵ
********************************************/
#include "TempCtrl.h"						//�¶ȿ���
#include "Configuration.h"					//��������
#include "FDM_config.h"
#include <math.h>							//��ѧ������
#include "PrinterCtrl.h"					//״̬����
#include "stdlib.h"							//״̬����
#include "HeatCtrl.h"						//����ͷ����

/*******************************************
	�ڲ���������
********************************************/
static void TempCtrl_Heat_OutTime( TEMPCTRL_OBJ *obj);
static void TempCtrl_Pid_ForceSetOutput(TEMPCTRL_OBJ *obj, float inDuty);
static void TempCtrl_Pid_Calc(TEMPCTRL_OBJ *obj);
static void TempCtrl_MixedCalc(TEMPCTRL_OBJ *obj, s16 *opDuty, s16 *opDutyGain);
static void TempCtrl_PwmCalc(TEMPCTRL_OBJ *obj);
static void TempCtrl_ErrorDataClear(TEMPCTRL_OBJ *obj);

/*******************************************
	����ȫ������
********************************************/
TEMPCTRL_OBJ m_TempCtrl_Obj_Tbl[TEMPCTRL_ITEMMAX];


const ERRORS m_ErrorTbl[TEMPCTRL_ITEMMAX] = 
{
					 // 	�¶ȹ���ֵ	�¶ȹ���			���ȳ�ʱ			�������쳣		����ʱ�¶Ȳ�����	�豸������
/* TEMPCTRL_EXTRUDER1 */	{240,	ERROR_TEMOVERTOP1,	ERROR_TEMTIMEOUT1,	ERROR_TEMSEN1,	ERROR_TEMHEAD1,		ERROR_TEMBADHEAD	},
/* TEMPCTRL_EXTRUDER2_1 */	{240,	ERROR_TEMOVERTOP1,	ERROR_TEMTIMEOUT1,	ERROR_TEMSEN1,	ERROR_TEMHEAD1,		ERROR_TEMBADHEAD	},
/* TEMPCTRL_EXTRUDER2_2 */	{240,	ERROR_TEMOVERTOP1,	ERROR_TEMTIMEOUT1,	ERROR_TEMSEN1,	ERROR_TEMHEAD1,		ERROR_TEMBADHEAD	},
/* TEMPCTRL_EXTRUDER2_3 */	{240,	ERROR_TEMOVERTOP1,	ERROR_TEMTIMEOUT1,	ERROR_TEMSEN1,	ERROR_TEMHEAD1,		ERROR_TEMBADHEAD	},
/* TEMPCTRL_EXTRUDER2_4 */	{240,	ERROR_TEMOVERTOP1,	ERROR_TEMTIMEOUT1,	ERROR_TEMSEN1,	ERROR_TEMHEAD1,		ERROR_TEMBADHEAD	},
/* TEMPCTRL_EXTRUDER3_1 */	{240,	ERROR_TEMOVERTOP1,	ERROR_TEMTIMEOUT1,	ERROR_TEMSEN1,	ERROR_TEMHEAD1,		ERROR_TEMBADHEAD	},
/* TEMPCTRL_EXTRUDER3_2 */	{240,	ERROR_TEMOVERTOP1,	ERROR_TEMTIMEOUT1,	ERROR_TEMSEN1,	ERROR_TEMHEAD1,		ERROR_TEMBADHEAD	}
};

const TEMPDEVICE m_DeviceTbl[TEMPCTRL_ITEMMAX] = 
{
/* TEMPCTRL_EXTRUDER1   */	{HEATER_MAX,	TEMPSENSOR_MAX},

/* TEMPCTRL_EXTRUDER2_1 */	{HEATER_1,	TEMPSENSOR_4},
/* TEMPCTRL_EXTRUDER2_2 */	{HEATER_2,	TEMPSENSOR_5},
/* TEMPCTRL_EXTRUDER2_3 */	{HEATER_3,	TEMPSENSOR_6},
/* TEMPCTRL_EXTRUDER2_4 */	{HEATER_4,	TEMPSENSOR_1},

/* TEMPCTRL_EXTRUDER3_1 */	{HEATER_MAX,	TEMPSENSOR_MAX},
/* TEMPCTRL_EXTRUDER3_2 */	{HEATER_MAX,	TEMPSENSOR_MAX}
};

/*******************************************
	�ⲿ�ο�����
********************************************/

/*******************************************
	�ⲿ����
********************************************/

/*******************************************************************
���ܣ�����ռ�ձ� �����ѹ
�����ߣ�chen-lj
�������ڣ�2016/10/17
��������
����ֵ�� ��
********************************************************************/
static void TempCtrl_PwmCtrlOutput(TEMPCTRL_OBJ *obj)
{
	if(obj->HeatCtrl.pwm_Count < obj->HeatCtrl.pwm_ON_OFF_Switch)
	{
		HeatCtrl_On(obj->device.heater);
	}
	else
	{
		HeatCtrl_Off(obj->device.heater);
	}
	obj->HeatCtrl.pwm_Count += PWM_INCREMENT;
	if(obj->HeatCtrl.pwm_Count >= PWM_COUNT_CYCLE)
	{
		obj->HeatCtrl.pwm_Count = 0;
	}
}
/*******************************************************************
���ܣ�����ռ�ձ� �����ѹ
�����ߣ�chen-lj
�������ڣ�2016/10/17
��������
����ֵ�� ��
********************************************************************/
static void TempCtrl_PwmDataClear(TEMPCTRL_OBJ *obj)
{
	obj->HeatCtrl.mixedHeatCtrl.heatStatus = HEATSTATUS_FIXED_SET;
	
	TempCtrl_Pid_ForceSetOutput(obj, 0);
	PWM_SETDUTY(obj, 0, 1);
}

/*******************************************************************
���ܣ�����PID���
�����ߣ�chen-lj
�������ڣ�2017/2/26
��������
����ֵ�� ��
********************************************************************/
static void TempCtrl_Pid_ForceSetOutput(TEMPCTRL_OBJ *obj, float inDuty)
{
	PIDDATA *pidData = &obj->HeatCtrl.pidData;
	
	pidData->i_Integral = limit(inDuty*PID_GAIN*PID_DATA_GAIN, PID_DATA_LIMIT_MIN, PID_DATA_LIMIT_MAX);
	pidData->pid_Output = pidData->i_Integral/PID_DATA_GAIN;
}
/*******************************************************************
���ܣ��¶�PID���ƣ������²�������ռ�ձ�
�����ߣ�chen-lj
�������ڣ�2016/10/17
��������
����ֵ�� ��
********************************************************************/
static void TempCtrl_Pid_Calc(TEMPCTRL_OBJ *obj)
{
	PIDDATA *pidData = &obj->HeatCtrl.pidData;
	float aDiff = obj->HeatCtrl.tempDiff;
	float aDiff_Abs = fabs(obj->HeatCtrl.tempDiff);
	s32 aP;
	s32 aI;
	
	pidData->pid_Dir = (obj->HeatCtrl.tempChg_TC > 0)? PID_DIR_PLUS: 
							((obj->HeatCtrl.tempChg_TC < 0)? PID_DIR_MINUS: pidData->pid_Dir);

	if(aDiff <= 0)																//����
	{
		aP = -aDiff * pidData->p_Gain;
		aI = ((aP+pidData->i_Integral) >= PID_DATA_LIMIT_MAX)? 0: aP / pidData->i_IntegralTime;
		
		if((aDiff_Abs > 2) && (pidData->pid_Dir == PID_DIR_MINUS))	aP *= pidData->p_OppositeGain;
	}
	else																		//����
	{
		aP = -aDiff * pidData->p_Gain;
		aI = ((aP+pidData->i_Integral) <= PID_DATA_LIMIT_MIN)? 0: aP / pidData->i_IntegralTime;
		
		if((aDiff_Abs > 2) && (pidData->pid_Dir == PID_DIR_PLUS))	aP *= pidData->p_OppositeGain;
	}
	
	aI += pidData->i_Integral;
	pidData->i_Integral = limit(aI, PID_DATA_LIMIT_MIN, PID_DATA_LIMIT_MAX);

	pidData->pid_Output = (aP + aI) / PID_DATA_GAIN;
}
/*******************************************************************
���ܣ��̶�ռ�ձȺ�PID���ƽ�ϼ���ռ�ձ�
�����ߣ�chen-lj
�������ڣ�2017/2/26
��������
����ֵ�� ��
********************************************************************/
static void TempCtrl_MixedCalc(TEMPCTRL_OBJ *obj, s16 *opDuty, s16 *opDutyGain)
{
	MIXEDHEATCTRL *mixedHeatCtrl = &obj->HeatCtrl.mixedHeatCtrl;
	float tempDiff = obj->HeatCtrl.tempDiff;
	float aEfficiency;
	float aTarget = max(obj->TargetTemp, TEMP_ENVIRONMENT);
	float aCurrent = max(obj->CurrentTemp, TEMP_ENVIRONMENT);
	s16	  aDuty = 0;
	s16   aDutyGain = 1;
	
	switch(mixedHeatCtrl->heatStatus)
	{
		case HEATSTATUS_FIXED_SET:
			if(tempDiff < -mixedHeatCtrl->switchCond_Temp)
			{
				aEfficiency = 1 - mixedHeatCtrl->lossRate*(aTarget+aCurrent-TEMP_ENVIRONMENT*2)/2;
				aEfficiency = limit(aEfficiency, 0, 1);
				mixedHeatCtrl->timeCounter = (-tempDiff)*mixedHeatCtrl->idealRise/aEfficiency*1000000/TIMER_CYCLE_CALC;
				aDuty = 1;
			}
			mixedHeatCtrl->heatStatus = HEATSTATUS_FIXED_HEATING;
			break;
		case HEATSTATUS_FIXED_HEATING:
			if(tempDiff < -mixedHeatCtrl->switchCond_Temp)
			{
				if(mixedHeatCtrl->timeCounter > 0)
				{
					aDuty = 1;
					mixedHeatCtrl->timeCounter--;
				}
				else 
				{
					aDuty = 0;
					// float aTemp = max(aCurrent-50, TEMP_ENVIRONMENT);
					// aDuty = mixedHeatCtrl->lossRate*1000*aTemp;
					aDutyGain = 1000;
					if(obj->HeatCtrl.tempChg_500 <= mixedHeatCtrl->switchCond_Spd)
					{
						mixedHeatCtrl->heatStatus = HEATSTATUS_FIXED_SET;
					}
				}
			}
			else if(tempDiff > mixedHeatCtrl->switchCond_Temp)
			{}
			else
			{
				TempCtrl_Pid_ForceSetOutput(obj, mixedHeatCtrl->lossRate*(aCurrent-TEMP_ENVIRONMENT));
				mixedHeatCtrl->heatStatus = HEATSTATUS_PID_HEATING;
			}
			break;
		case HEATSTATUS_PID_HEATING:
			if(fabs(tempDiff) <= mixedHeatCtrl->switchCond_Temp)
			{
				TempCtrl_Pid_Calc(obj);
				aDuty = obj->HeatCtrl.pidData.pid_Output;
				aDutyGain = PID_GAIN;
			}
			else
			{
				mixedHeatCtrl->heatStatus = HEATSTATUS_FIXED_SET;
			}
			break;
		default:
			break;

	}
	*opDuty = aDuty;
	*opDutyGain = aDutyGain;
}

/*******************************************************************
���ܣ�����PWM���ռ�ձ�
�����ߣ�chen-lj
�������ڣ�2017/2/26
������TEMPCTRL_OBJ *obj
����ֵ�� ��
********************************************************************/
static void TempCtrl_PwmCalc(TEMPCTRL_OBJ *obj)
{
	float aDiffTemp = obj->CurrentTemp - obj->TargetTemp;
	float aTempChg_TC = obj->CurrentTemp - obj->LastTemp;
	s16	 aDuty = 0;
	s16  aDutyGain = 1;
	
	obj->HeatCtrl.tempChg_TC = aTempChg_TC;
	obj->HeatCtrl.tempDiff = aDiffTemp;
	
	/* 500ms�¶ȱ仯 */
	if(++obj->HeatCtrl.timeCounter_500 >= TIMER_COUNT_500)
	{
		obj->HeatCtrl.tempChg_500 = obj->CurrentTemp - obj->HeatCtrl.lastTemp_500;
		obj->HeatCtrl.lastTemp_500 = obj->CurrentTemp;
		obj->HeatCtrl.timeCounter_500 = 0;
	}

	switch(obj->HeatCtrl.heatType)
	{
		case HEATTYPE_FIXED:													//�̶�ռ�ձȿ���
			aDuty = (aDiffTemp >= 0)? 0: obj->HeatCtrl.pwm_FixedDuty;
			aDutyGain = FIXED_GAIN;
			break;
		case HEATTYPE_PID:
			TempCtrl_Pid_Calc(obj);
			aDuty = obj->HeatCtrl.pidData.pid_Output;
			aDutyGain = PID_GAIN;
			break;
		case HEATTYPE_MIXED:
			TempCtrl_MixedCalc(obj, &aDuty, &aDutyGain);
			break;
		default:
			break;
	}
	PWM_SETDUTY(obj, aDuty, aDutyGain);
	// {
		// float fDuty = (float)aDuty/aDutyGain;
		// fDuty = min(fDuty, 0.8);
		// PWM_SETDUTY(obj, fDuty, 1);
	// }
}


/*******************************************************************
���ܣ�Ŀ���¶��趨
�����ߣ�chen-lj
�������ڣ�2014/12/23
������
	enum TEMPCTRL_ITEMS item	���Ȳ���	
    temp:Ŀ���¶�
����ֵ�� ��
********************************************************************/
void TempCtrl_setTargetTemp(enum TEMPCTRL_ITEMS item, s16 temp, bool isNeedWait )
{
	TEMPCTRL_OBJ *obj = &m_TempCtrl_Obj_Tbl[item];
	if( (temp > 0))// && ( False == ErrManage_Get_StopPri_ErrStau() ) )
	{
		obj->TargetTemp = temp;
		obj->errCtrl.HeadErrorTime = 0;
		if(obj->CurrentTemp < obj->TargetTemp)
		{
			obj->CtrlFlag.Temp_Reach = TEMP_UNREACH;
			obj->CtrlFlag.WaitTempReached = isNeedWait;
			// IndicateCtrl_Set(INDICATE_TIME_150MS, INDICATE_TIME_150MS, INDICATE_YELLOW, False);
		}
	}
	else
	{
		TempCtrl_StopHeat(item);
	}
}

/*******************************************************************
���ܣ�Ŀ���¶�ȡ��
�����ߣ�chen-lj
�������ڣ�2017/01/22
������  ��
����ֵ��
      Ŀ���¶�
********************************************************************/
s16 TempCtrl_getTargetTemp(  enum TEMPCTRL_ITEMS item  )
{
	return (m_TempCtrl_Obj_Tbl[item].TargetTemp);
}

/*******************************************************************
���ܣ��¶�ȡ��
�����ߣ�chen-lj
�������ڣ�2017/01/22
������  ��
����ֵ��
      ��ǰ�������¶�
********************************************************************/
float TempCtrl_getCurrentTemp( enum TEMPCTRL_ITEMS item )
{
	return (m_TempCtrl_Obj_Tbl[item].CurrentTemp);
}

void TempCtrl_HeadCheck(TEMPCTRL_OBJ *obj)
{
	obj->errCtrl.HeadErrorTime++;
	if(obj->errCtrl.HeadErrorTime ==1 )
	{
		obj->errCtrl.HeadErrorStartTemp = obj->CurrentTemp;
	}
	if(obj->errCtrl.HeadErrorTime >= HEADTEM_CHECK_TIME)
	{
		if(obj->CurrentTemp - obj->errCtrl.HeadErrorStartTemp < 5)
		{
			ErrorMng_SetError(obj->errCtrl.Errors.err_NotRise);
		}
	}
}
/*******************************************************************
���ܣ�ֹͣ����
�����ߣ�cao-dl
�������ڣ�2014/11/19
��������
����ֵ����
********************************************************************/
void TempCtrl_StopHeat( enum TEMPCTRL_ITEMS item )
{
	int id;
	if(item < TEMPCTRL_ITEMMAX)
	{
		m_TempCtrl_Obj_Tbl[item].TargetTemp = 0;
	}
	else if(item == TEMPCTRL_ITEM_ALL)
	{
		for(id = 0; id < TEMPCTRL_ITEMMAX; id++)
		{
			m_TempCtrl_Obj_Tbl[id].TargetTemp = 0;
		}
	}
	// IndicateCtrl_Set(INDICATE_TIME_NEVER, INDICATE_TIME_ALWAYS, INDICATE_YELLOW, False);	//�Ƶ���
}

/*******************************************************************
���ܣ�����Ŀ���¶�ǰ�����ȡ�����Ŀ���¶Ⱥ�ֹͣ����
�����ߣ�zhao-pb
�������ڣ�2015/7/10
����:   ��
����ֵ: ��ǰ����ͷ�¶�
********************************************************************/
void TempCtrl_Manage_Heater( void )
{
	int item;

	for(item = 0; item < TEMPCTRL_ITEMMAX; item++)
	{
		TEMPCTRL_OBJ *obj = &m_TempCtrl_Obj_Tbl[item];
		//�����¶�ȡ��
		obj->LastTemp = obj->CurrentTemp;
		obj->errCtrl.Sensor_Status = TempSensor_GetTemperature(obj->device.sensor, &obj->CurrentTemp);
		
		//�¶ȹ����¶ȴ������쳣meng_xj
		if( obj->errCtrl.Sensor_Status == TEMP_ERROR_LOW)
		{
			obj->errCtrl.SensorBadCnt++;
			if( obj->errCtrl.SensorBadCnt >=  MAX_TEMPLOWER_CNT )
			{  
				obj->errCtrl.SensorBadCnt = 0;  
				ErrorMng_SetError( obj->errCtrl.Errors.err_SensorErr );
			}
		}
		//�¶ȹ����¶ȴ������쳣meng_xj
		else if( obj->errCtrl.Sensor_Status == TEMP_ERROR_HIGH)
		{
			obj->errCtrl.SensorNoConnectCnt++;
			if( obj->errCtrl.SensorNoConnectCnt >= MAX_TEMPOOVER_CNT )
			{
				obj->errCtrl.SensorNoConnectCnt = 0;
				ErrorMng_SetError( obj->errCtrl.Errors.err_SensorErr );
			}
		}
		//�¶ȹ����쳣meng_xj
		if( obj->CurrentTemp > MAXTEMP && (obj->errCtrl.Sensor_Status == TEMP_OK) )
		{
			obj->errCtrl.TempOverCnt++;
			if( obj->errCtrl.TempOverCnt >= MAX_TEMPOOVER_CNT )
			{
				obj->errCtrl.TempOverCnt = 0;
				ErrorMng_SetError( obj->errCtrl.Errors.err_OverHeat );
			}
		}
		
		// ����״̬
		if( obj->TargetTemp > 0 )
		{			
			if( obj->CurrentTemp >= obj->TargetTemp )
			{
				obj->CtrlFlag.Temp_Reach = TEMP_REACH;
			}	
			if((obj->CurrentTemp <= (obj->TargetTemp - HEADTEM_CHECK_RABGE))&&obj->errCtrl.HeadErrorTime<HEADTEM_CHECK_TIME)//��ͷ�쳣���
			{
				TempCtrl_HeadCheck(obj);
			}				
			TempCtrl_PwmCalc(obj);
			obj->errCtrl.TempOverCnt = 0;			
		}
		//������״̬
		else
		{
			TempCtrl_PwmDataClear(obj);
			TempCtrl_ErrorDataClear(obj);
			
			obj->CtrlFlag.Temp_Reach = TEMP_UNREACH;
			//ֹͣ����
			HeatCtrl_Off(obj->device.heater);
		}		
		// ���ȳ�ʱ�ж�
		TempCtrl_Heat_OutTime(obj);	
	}
}

/*******************************************************************
���ܣ����ȳ�ʱ
�����ߣ�zhao-pb
�������ڣ�2015/7/10
��������
����ֵ����
********************************************************************/
void TempCtrl_Heat_OutTime( TEMPCTRL_OBJ *obj)
{
	u32 heatTime = obj->errCtrl.HeatTime;
	
	if( ( obj->TargetTemp > 0 ) && (( HAL_GetTick() - obj->errCtrl.Heat_OverTime ) >= TEMPOVER_SCAN_TIME ) )
	{
		obj->errCtrl.Heat_OverTime = HAL_GetTick();
		heatTime++;

		//���ȳ�ʱ:5����
		if( ( heatTime >= MAX_TEMPOVER_TIME ) && ( obj->CurrentTemp < ( obj->TargetTemp - MAX_TEMP_DEV ) ) )
		{
			heatTime = 0;
			//�쳣�������¶��쳣meng_xj
			ErrorMng_SetError(obj->errCtrl.Errors.err_OverTime);
		}
		//����Ŀ���¶�
		else if(obj->CurrentTemp >= obj->TargetTemp)
		{
			heatTime = 0;
			if(IsErrorFlag(obj->errCtrl.Errors.err_OverTime) == True)
			{
				ErrorMng_ResetError(obj->errCtrl.Errors.err_OverTime);
			}
		}
		if((obj->CtrlFlag.Temp_Reach==TEMP_REACH)&&( heatTime >= MAX_PRINTINGTEMPOVER_TIME ) && ( obj->CurrentTemp < ( obj->TargetTemp - MAX_PRINTINGTEMP_DEV ) ))
		{
			heatTime = 0;
			//�쳣�������¶��쳣
			ErrorMng_SetError(obj->errCtrl.Errors.err_OverTime);
		}
	}
	else if( obj->TargetTemp <= 0 )
	{
		heatTime = 0;
	}
	obj->errCtrl.HeatTime = heatTime;
}

/*******************************************************************
���ܣ�
�����ߣ�chen-lj
�������ڣ�
��������
����ֵ�� ��
********************************************************************/
static void TempCtrl_ErrorDataClear(TEMPCTRL_OBJ *obj)
{
	obj->errCtrl.Heat_OverTime = 0;
	obj->errCtrl.HeatTime = 0;
	obj->errCtrl.TempOverCnt = 0;
	obj->errCtrl.SensorNoConnectCnt = 0;
	obj->errCtrl.SensorBadCnt = 0;
	obj->errCtrl.Sensor_Status = TEMP_OK;
	obj->errCtrl.HeadErrorStartTemp = 0;
	obj->errCtrl.HeadErrorTime = 0;
}

/*******************************************************************
���ܣ�����Ŀ���¶ȵȴ�
�����ߣ�zhao_pb
�������ڣ�2015/11/18
��������
����ֵ����
********************************************************************/
bool TempCtrl_IsWaitForTemp( void )
{
	int item;
	
	for(item = 0; item < TEMPCTRL_ITEMMAX; item++)
	{
		//δ����Ŀ���¶�
		if(( m_TempCtrl_Obj_Tbl[item].CtrlFlag.Temp_Reach == TEMP_UNREACH  ) && 
			( m_TempCtrl_Obj_Tbl[item].CtrlFlag.WaitTempReached != False  ) && 
			(m_TempCtrl_Obj_Tbl[item].TargetTemp > 0))
		{
			return True;	//��Ҫ�ȴ�
		}
	}

	return False;	//����ȴ�
}

/*******************************************************************
���ܣ�����Ŀ���¶ȵȴ�
�����ߣ�zhang_jie
�������ڣ�2017/06/05
��������
����ֵ����
********************************************************************/
bool TempCtrl_IsWaitForTempSingle(u8 item)
{
		//δ����Ŀ���¶�
	if(( m_TempCtrl_Obj_Tbl[item].CtrlFlag.Temp_Reach == TEMP_UNREACH  ) && \
		( m_TempCtrl_Obj_Tbl[item].CtrlFlag.WaitTempReached != False  ) && \
		(m_TempCtrl_Obj_Tbl[item].TargetTemp > 0))
		{
			return True;	//��Ҫ�ȴ�
		}
	return False;	//����ȴ�
}
bool TempCtrl_IsWarm(void)
{
	u8 i;
	bool ret = False;
	for(i = 0 ;i < TEMPCTRL_ITEMMAX ;i++)
	{
		if(m_TempCtrl_Obj_Tbl[i].TargetTemp > 0)
		{
			ret = True;
			break;
		}
	}
	return ret;
}
/*******************************************************************
���ܣ���ʼ��
�����ߣ�cao-dl
�������ڣ�2014/11/19
��������
����ֵ����
********************************************************************/
void TempCtrl_Init( void )
{
	int item;
	TEMPCTRL_OBJ *obj;

	for(item = 0; item < TEMPCTRL_ITEMMAX; item++)
	{
		obj = &m_TempCtrl_Obj_Tbl[item];

		obj->device = m_DeviceTbl[item];
		
		obj->TargetTemp = 0;
		obj->CurrentTemp = 0;
		obj->LastTemp = 0;
		
		obj->errCtrl.Heat_OverTime = 0;
		obj->errCtrl.HeatTime = 0;
		obj->errCtrl.TempOverCnt = 0;
		obj->errCtrl.SensorNoConnectCnt = 0;
		obj->errCtrl.SensorBadCnt = 0;
		obj->errCtrl.Errors = m_ErrorTbl[item];
		
		obj->CtrlFlag.Temp_Reach = TEMP_UNREACH;
		obj->CtrlFlag.WaitTempReached = False;
		
		obj->HeatCtrl.pwm_ON_OFF_Switch = 0;
		obj->HeatCtrl.pwm_Count = 0;
		obj->HeatCtrl.timeCounter_500 = 0;
		obj->HeatCtrl.lastTemp_500 = 0;
		
		obj->HeatCtrl.pidData.i_Integral = 0;
		obj->HeatCtrl.pidData.i_IntegralTime = PID_KIT;
		obj->HeatCtrl.pidData.p_Gain = PID_KP;
		obj->HeatCtrl.pidData.p_OppositeGain = PID_KP_OPSTGAIN;
		obj->HeatCtrl.pidData.pid_Output = 0;
		obj->HeatCtrl.pidData.pid_Dir = PID_DIR_PLUS;
		
		obj->HeatCtrl.mixedHeatCtrl.heatStatus = HEATSTATUS_FIXED_SET;
		obj->HeatCtrl.mixedHeatCtrl.lossRate = TEMP_EXT_LOSSRATE;
		obj->HeatCtrl.mixedHeatCtrl.idealRise = TEMP_EXT_IDEAL;
		obj->HeatCtrl.mixedHeatCtrl.switchCond_Temp = TEMP_EXT_SWCOND_TEMP;
		obj->HeatCtrl.mixedHeatCtrl.switchCond_Spd = TEMP_EXT_SWCOND_SPD;
		
		
		//ֹͣ����
		HeatCtrl_Off(obj->device.heater);
		
		m_TempCtrl_Obj_Tbl[item].HeatCtrl.heatType = HEATTYPE_PID;
	}
}

/*******************************************************************
���ܣ�0.5ms����
�����ߣ�chen-lj
�������ڣ�2017/3/4
��������
����ֵ����
********************************************************************/
void TempCtrl_Timer_0_5( void )
{
	int item;
	// int count = 0;
	
	for(item = 0; item < TEMPCTRL_ITEMMAX; item++)
	{
		// if((m_TempCtrl_Obj_Tbl[item].TargetTemp > 0) && (m_TempCtrl_Obj_Tbl[item].TargetTemp-m_TempCtrl_Obj_Tbl[item].CurrentTemp > 20))
		// if((m_TempCtrl_Obj_Tbl[item].TargetTemp-m_TempCtrl_Obj_Tbl[item].CurrentTemp > 20))
		// {
			// count++;
			// if(count > 2)
			// {
				// m_TempCtrl_Obj_Tbl[item].HeatCtrl.pwm_Count = m_TempCtrl_Obj_Tbl[item].HeatCtrl.pwm_ON_OFF_Switch;
			// }
		// }
		
		TempCtrl_PwmCtrlOutput(&m_TempCtrl_Obj_Tbl[item]);
	}
}
