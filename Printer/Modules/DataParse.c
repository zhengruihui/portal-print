/********************** (C) COPYRIGHT 2015 DediBot ******************************
  * @file    DataParse.c
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-03-28
  * @brief   �쳣����
*********************************************************************************/
/*******************************************
	������ϵ
********************************************/
#include "DataParse.h"						//��ӡ���ݷ���
#include "TempCtrl.h"
#include "stdlib.h"

/*******************************************
	����ȫ������
********************************************/

/*******************************************
    ����ȫ�ֱ���
********************************************/

/*******************************************
	�ⲿ�ο�����
********************************************/

/*******************************************
	�ⲿ�ο�����
********************************************/

/*******************************************
	�ڲ���������
********************************************/
/*******************************************
	�ⲿ��������
********************************************/
/******************************************
���ܣ���ͣ��ӡ,����ص�ԭ���򷴷���
�����ߣ�liu-cy
�������ڣ�2017/1/6
��������
����ֵ����
*******************************************/
void DataParse_M0( PORTALFRAME *this )
{
	Print_Finish(this);
}

/******************************************
���ܣ��ָ���ӡ
�����ߣ�zheng-rh
�������ڣ�2017/12/25
��������
����ֵ����
*******************************************/
void DataParse_M24( PORTALFRAME *this )
{
	Print_Continue(this);
}

/******************************************
���ܣ���ͣ��ӡ,����ص�ԭ���򷴷���
�����ߣ�liu-cy
�������ڣ�2017/1/6
��������
����ֵ����
*******************************************/
void DataParse_M25( PORTALFRAME *this )
{
	//Print_Pause(this);
	Print_Finish(this);
}

/******************************************
���ܣ�M84
	���ò��������ʱ�ر�ʱ��
�����ߣ�cao-dl
�������ڣ�2014/12/23
������
	value���趨ʱ��
����ֵ����
*******************************************/
void DataParse_M84( PORTALFRAME *this, float value )
{
	int i;
	
	Stepper_QuickStop(this->positionMng.mainMoveModule);
	Stepper_QuickStop(this->positionMng.freeMoveModule);
	
	for(i = 0; i < this->positionMng.totalAxesCount; i++)
	{
		AxisCtrl_Disable(this->positionMng.axes[i]);
	}
	
	//��ʱ�ر�
	// if( value > 0.0f )
	// {
		// m_Stepper_Inactive_Time = value * 1000;
	// }
	// //ȫ���ֹ
	// else
	// {
		// AxisCtrl_All_Stop();
	// }
}
/******************************************
���ܣ�
	���ü���ͷ�¶�
�����ߣ�cao-dl
�������ڣ�2014/12/23
������
����ֵ����
*******************************************/
void DataParse_SetExtruderTemp( PORTALFRAME *this, char *gcode, bool isNeedWait )
{
	char *endptr;
	int cmpIdx, schIdx, schIdx2;
	int len = strlen(gcode);
	int size = len+1;
	char cmpStr[10];
	
	for(schIdx = 0; schIdx < len; schIdx++)
	{
		if(gcode[schIdx] == 'S')
		{
			cmpStr[0] = gcode[schIdx];
			schIdx++;
			for(schIdx2 = schIdx; schIdx2 < size; schIdx2++)
			{
				if(gcode[schIdx2] == '_')
				{
					memcpy(&cmpStr[1], &gcode[schIdx], schIdx2-schIdx);
					cmpStr[schIdx2-schIdx+1] = '\0';
					schIdx = schIdx2+1;
					break;
				}
				else if((gcode[schIdx2] < '0') || (gcode[schIdx2] > '9'))
				{
					cmpStr[1] = '\0';
					break;
				}
			}
			for(cmpIdx = 0; cmpIdx < this->extruderCount; cmpIdx++)
			{
				if(0 == strcmp(cmpStr, this->extruderCodes[cmpIdx]))
				{
					TempCtrl_setTargetTemp(this->extruder_Stru[cmpIdx].heatItem, (float)strtod(&gcode[schIdx],&endptr), isNeedWait);
					if(False != isNeedWait)
					{
						EventMng_SetEvent_Normal(PRINTER_EVENT_SETWAITTEMP, NULL);
						this->printInfo.stage = PRINT_STAGE_WAITTEMP;
					}
					// break;
				}
			}
		}
	}
}
/*******************************************************************
���ܣ�Ҫ�����ַ��Ƿ���Ŀ���ַ�����У�True���ޣ�False
�����ߣ�cao-dl
�������ڣ�2014/11/19
����:
	code:Ҫ�����ַ�
����ֵ��
	True��Ŀ���ַ��ҵ�
	False��Ŀ���ַ�δ�ҵ�
********************************************************************/
bool DataParse_code_seen_chr( char *gcode, const char code, float *data )
{
	bool	flag;
	char*   strchr_pointer;
	strchr_pointer = strchr(gcode, code);
	flag = (bool)(strchr_pointer != NULL);
	if( flag == False )
	{
		*data = 0.0;
	}
	else
	{
		*data = strtod(&gcode[strchr_pointer - gcode + 1], NULL);
	}
	return flag;
}
/*******************************************************************
���ܣ�Ҫ�����ַ��Ƿ���Ŀ���ַ�����У�True���ޣ�False
�����ߣ�cao-dl
�������ڣ�2014/11/19
����:
	code:Ҫ�����ַ�
����ֵ��
	True��Ŀ���ַ��ҵ�
	False��Ŀ���ַ�δ�ҵ�
********************************************************************/
bool DataParse_code_seen_str( char *gcode, const char* code, float *data )
{
	bool	flag;
	char*   strchr_pointer;
	strchr_pointer = strstr(gcode, code);
	flag = (bool)(strchr_pointer != NULL);
	if( flag == False )
	{
		*data = 0.0;
		//return False;
	}
	else
	{
		*data = strtod(&gcode[strchr_pointer - gcode + strlen(code)], NULL);
		//return True;
	}
	return flag;
}
/*******************************************************************
���ܣ��ָ�ȥ���ַ�����code֮�������
�����ߣ�deng-xy
�������ڣ�2016/5/19
����:
	gcode��Ԫ����
	code:�ָ����

********************************************************************/
void DataParse_code_tok_chr( char *gcode, char *code )
{
	if(*gcode == code[0])
	{
		*gcode = '\0';
	}
	else
	{
		gcode = strtok( gcode, code );
	}
}

/*******************************************************************
���ܣ��������ݽ���
�����ߣ�cao-dl
�������ڣ�2014/11/19
������
����ֵ����
********************************************************************/
bool DataParse_G_M_Ctrl( PORTALFRAME *this, char* gcode )
{
	float	value;
	bool	Status;
	
	Status = False;
	DataParse_code_tok_chr(gcode,";");
	
	//��������С�G���������
	if( DataParse_code_seen_chr(gcode, 'G' , &value ) )
	{
		//��G�����������
		switch((int)value)
		{
			//�����ƶ�
			case 0: // G0 -> G1
			//�ɿ��ƶ�
			case 1: // G1
				//Ŀ��λ���ƶ�
				PositionMng_PrepareMove_Gcode(&this->positionMng, gcode);
				break;
			case 27: //G28 Home XY Axis one at a time
				PositionMng_MoveHome_XY(&this->positionMng);
			  break;
			case 28: //G28 Home all Axis one at a time
				PositionMng_MoveHome(&this->positionMng);
				break;
			//����λ��
			case 92: // G92
				PositionMng_Set_Position_Gcode( &this->positionMng, gcode, False );
				break;
			default:
				break;
		}
	}
	//���������С�M��ʱ
	else if(DataParse_code_seen_chr(gcode, 'M', &value))
	{
		switch( (int)value )
		{
			case 104: // M104
				DataParse_SetExtruderTemp(this, gcode, False);
				break;
			//���ü������¶ȣ����ȴ�
			case 109:
				DataParse_SetExtruderTemp(this, gcode, True);
				break;
			case 140: // M140
				break;
			//���ü������¶ȣ����ȴ�
			case 190:
				break;				
			//���ò�������ر�ʱ��
			case 84:
				DataParse_code_seen_chr(gcode, 'S', &value);
				DataParse_M84(this, value );
				break;
			case 0:		//ֹͣ��ӡ
				DataParse_M0(this);
				EventMng_SetEvent_Level(PRINTER_EVENT_STANDBY, NULL ,EVENT_LEVEL_0);
				break;
			case 24:	//�ָ�SD����ӡ
				DataParse_M24(this);
				break;
			case 25:	//��ͣSD����ӡ
				DataParse_M25(this);
				break;
			case 102:
				//ȡ�ô�ӡ������
				if( DataParse_code_seen_chr(gcode, 'L', &value ) )
				{
				}
				break;
			case 201:															//������������ٶ�
				break;
			case 203:															//�����������ٶ�
				break;
			case 204:															//���ü��ٶ�
				break;
			case 205:															//�߼�����
				break;
			case 106:
				if(DataParse_code_seen_chr(gcode, 'S', &value))
				{
					FanCtrl_SetFan((_FAN)9, (uint8_t)value);
				}
			break;
				
			case 110:
				if(DataParse_code_seen_chr(gcode, 'N', &value))
				{
						this->total_gcode_line = value;
						this->current_gcode_line = 0;
				}
				break;	
			default:
				break;
		}
	}
	return (Status);
}


