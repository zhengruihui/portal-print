/********************** (C) COPYRIGHT 2015 DediBot ******************************
  * @file    SdPrint.c
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-03-25
  * @brief   SD����
*********************************************************************************/

/*******************************************
	������ϵ
********************************************/
#include "Print.h"				//SD������
#include "DataParse.h"			//ͨ�����ݽ���
#include "usbd_cdc_if.h"
#include "ProtocolMng.h"
#include "LampCtrl.h"
/*******************************************


*******************************************
    ����ȫ�ֱ���
********************************************/

/*******************************************
	�ⲿ�ο�����
********************************************/

/*******************************************
	�ڲ���������
********************************************/

/*******************************************
		�ⲿ����
*******************************************/
void Print_Init( PORTALFRAME *this )
{
	this->printInfo.stage = PRINT_STAGE_STOPPRINT;
	memset(&this->printInfo, 0, sizeof(PRINTINFO));
}

//��ӡ��������ֹͣ��ӡ�Ĵ���
void Print_Exit( PORTALFRAME *this )
{
	int i;
	LampCtrl_OFF();
	PositionMng_Clear(&this->positionMng);
	PrintDataMng_Stop(&this->printDataMng);
	this->printInfo.printGcodeCount = 0;
	this->printInfo.TotalPrintTime = 0;
	this->printInfo.PrintTime = 0;
	for(i = 0; i < this->extruderCount; i++)
	{
		TempCtrl_StopHeat(this->extruder_Stru[i].heatItem);
	}

	PortalFrame_Go_Standby(this);
}

/******************************************
���ܣ���ӡǰԤ����
�����ߣ�
�������ڣ�
������
����ֵ��
*******************************************/
bool Print_Start( PORTALFRAME *this )
{
	bool res;
	// ��Ҫ����Ƿ���Կ�ʼ��ӡ

	//���ļ�����ʼ��ȡ����
	res = PrintDataMng_LocalStart(&this->printDataMng);

	if(res)
	{
		this->printInfo.totalgcodeSize = PrintDataMng_GetTotalSize(&this->printDataMng);
		this->printInfo.printGcodeCount = 0;
		this->printInfo.TotalPrintTime = 0;
		this->printInfo.PrintTime = 0;
		this->printInfo.stage = PRINT_STAGE_PRINTING;
		LampCtrl_ON();	
	}	
	return res;
}

/******************************************
���ܣ�ֹͣ��ӡ����
�����ߣ�
�������ڣ�
������
����ֵ��
*******************************************/
void Print_Stop( PORTALFRAME *this)
{
	switch(this->printInfo.stage)
	{
		case PRINT_STAGE_READPRINTTIME:	//ȡ�ô�ӡʱ��
		case PRINT_STAGE_PRINTING:		//��ӡ��
		case PRINT_STAGE_STOPWAITING:	//ֹͣ�ȴ���
		case PRINT_STAGE_WAITTEMP:	    //�ȴ�����״̬
		case PRINT_STAGE_PAUSEACTION:	//��ͣ������
		case PRINT_STAGE_PAUSE:			//��ͣ��
			  PositionMng_Clear(&this->positionMng);
		    PositionMng_MoveHome_XY(&this->positionMng);     //�ع�ԭ��

			  this->printInfo.stage = PRINT_STAGE_STOPACTION;
			break;
		case PRINT_STAGE_STOPACTION:	//ֹͣ������
		case PRINT_STAGE_STOPPRINT:	//ֹͣ��
			// if(NULL != event.notify)
			// {
				// event.notify(_NG);		//ʧ��
			// }
			break;
		default:
			break;
	}
}
/******************************************
���ܣ���ͣ��ӡ������M25��
�����ߣ�
�������ڣ�
������
����ֵ��
*******************************************/
void Print_Pause( PORTALFRAME *this)
{
	switch(this->printInfo.stage)
	{
		case PRINT_STAGE_READPRINTTIME:	//ȡ�ô�ӡʱ��
		case PRINT_STAGE_PRINTING:		//��ӡ��
		case PRINT_STAGE_WAITTEMP:	    //�ȴ�����״̬

			this->printInfo.stage = PRINT_STAGE_PAUSE;
			break;

		case PRINT_STAGE_STOPWAITING:	//ֹͣ�ȴ���
		case PRINT_STAGE_STOPACTION:	//ֹͣ������
		case PRINT_STAGE_STOPPRINT:	//ֹͣ��
		default:
			break;
	}
}
/******************************************
����:�ָ���ӡ����(M24)
�����ߣ�
�������ڣ�
������
����ֵ��
*******************************************/
void Print_Continue( PORTALFRAME *this)
{
	switch(this->printInfo.stage)
	{
		case PRINT_STAGE_READPRINTTIME:	//ȡ�ô�ӡʱ��
		case PRINT_STAGE_PRINTING:		//��ӡ��
		case PRINT_STAGE_WAITTEMP:	    //�ȴ�����״̬
			this->printInfo.stage = PRINT_STAGE_STOPWAITING;
			break;

		case PRINT_STAGE_STOPWAITING:	//ֹͣ�ȴ���
		case PRINT_STAGE_STOPACTION:	//ֹͣ������
			break;
		case PRINT_STAGE_PAUSE:	//��ͣ��
			this->printInfo.stage = PRINT_STAGE_PRINTING;
			break;
		default:
			break;
	}
}
/******************************************
���ܣ�������ӡ����(M0)
�����ߣ�
�������ڣ�
������
����ֵ��
*******************************************/
void Print_Finish( PORTALFRAME *this)
{
	switch(this->printInfo.stage)
	{
		case PRINT_STAGE_READPRINTTIME:	//ȡ�ô�ӡʱ��
		case PRINT_STAGE_PRINTING:		//��ӡ��
		case PRINT_STAGE_WAITTEMP:	    //�ȴ�����״̬
			this->printInfo.stage = PRINT_STAGE_STOPWAITING;
			break;

		case PRINT_STAGE_STOPWAITING:	//ֹͣ�ȴ���
		case PRINT_STAGE_STOPACTION:	//ֹͣ������
		case PRINT_STAGE_STOPPRINT:	//ֹͣ��
		default:
			break;
	}
}
/******************************************
���ܣ���ȡ���������ݲ���ӡ
�����ߣ�
�������ڣ�
��������
����ֵ����
*******************************************/
void Print_Pro( PORTALFRAME *this )
{
	u8	length;								//�洢�������ݳ���
	bool res;
	// char * res;
	int i;

	length = 0;
	// Print_StarTime();//��ӡʱ���ۼ�
	// PrintDataMng_NoPrintDataCheck();
	switch(this->printInfo.stage)
	{
		case PRINT_STAGE_READPRINTTIME:	//ȡ�ô�ӡʱ��
			res = PrintDataMng_GetOneLine( &this->printDataMng, this->printInfo.gcode, &length );
			if(True == res)
			{
				char *pointer = strstr(this->printInfo.gcode, "Print time");
				if(pointer != NULL)
				{
					int time[3];
					int i, j;

					j = 0;
					for(i = 0; i < 3; i++)
					{
						for(; j < length; j++)
						{
							if((this->printInfo.gcode[j] >= '0') && (this->printInfo.gcode[j] <= '9'))
							{
								time[i] = atoi(&this->printInfo.gcode[j]);
								while((this->printInfo.gcode[j] >= '0') && (this->printInfo.gcode[j] <= '9'))
								{
									j++;
									if(j >= length)	break;
								}
								break;
							}
						}
					}
					this->printInfo.TotalPrintTime = time[0]*3600 + time[1]*60 + time[2];
					this->printInfo.stage = PRINT_STAGE_PRINTING;
				}
			}
			if(this->printDataMng.diskErrCount > 10)
			{
				Logger_string("read file error !!!");
				
				Print_Stop(this);
			}
			break;
		case PRINT_STAGE_WAITLIMIT:
			//{GET_PORT(LIMIT8_PIN),	GET_PIN(LIMIT8_PIN)}
					//HAL_GPIO_ReadPin
		    if(GPIO_PIN_SET == HAL_GPIO_ReadPin(GET_PORT(LIMIT8_PIN),	GET_PIN(LIMIT8_PIN)))
				{
					this->printInfo.stage = PRINT_STAGE_PRINTING;
				}
		break;
			
		case PRINT_STAGE_PRINTING:		//��ӡ��
			if(False != PositionMng_CanMove(&this->positionMng))
			{
				this->current_gcode_line ++;
				res = PrintDataMng_GetOneLine( &this->printDataMng, this->printInfo.gcode, &length );
				if(True == res)
				{
					this->printInfo.gcode[length] = '\0';						//�˴������У�gcode����ʱ��Ҫ'\0'
					this->printInfo.printGcodeCount += length;
					DataParse_G_M_Ctrl( this, this->printInfo.gcode );	//ִ��Gcode
				}
			}
			break;
		case PRINT_STAGE_WAITTEMP:	//�ȴ�����״̬
			for(i = 0; i < this->extruderCount; i++)
			{
				if(False != TempCtrl_IsWaitForTempSingle(this->extruder_Stru[i].heatItem))	break;
			}
			if(i >= this->extruderCount)
			{
				this->printInfo.stage = PRINT_STAGE_PRINTING;
			}
			break;
		case PRINT_STAGE_STOPWAITING:	//ֹͣ�ȴ���
			if(False != PositionMng_IsMoveFinished(&this->positionMng))
			{
				PositionMng_MoveHome_XY(&this->positionMng);     //�ع�ԭ��
				this->printInfo.stage = PRINT_STAGE_STOPACTION;
			}
			break;
		case PRINT_STAGE_STOPACTION:	//ֹͣ������
			if(False != PositionMng_IsHomingFinished(&this->positionMng))
			{
				Print_Exit(this);
				this->printInfo.stage = PRINT_STAGE_STOPPRINT;
			}
			break;		
		case PRINT_STAGE_STOPPRINT:	//ֹͣ��
			break;		
		case PRINT_STAGE_PAUSEACTION:	//��ͣ������
			if(False != PositionMng_IsMoveFinished(&this->positionMng))
			{
				PositionMng_MoveHome_XY(&this->positionMng);     //�ع�ԭ��
				this->printInfo.stage = PRINT_STAGE_PAUSE;
			}
			break;
		case PRINT_STAGE_PAUSE:	//��ͣ��
			break;
			

	
		default:
			break;
	}
}
/******************************************
���ܣ��¼�����
�����ߣ�
�������ڣ�
��������
����ֵ����
*******************************************/
void Print_EventAction( PORTALFRAME *this, EVENT event )
{
	switch(event.event)
	{
		  case PRINTER_EVENT_SETWAITTEMP:
			this->printInfo.stage = PRINT_STAGE_WAITTEMP;
			break;

		default:
			break;
	}
}
