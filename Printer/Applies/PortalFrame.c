/********************** (C) COPYRIGHT 2015 DediBot ******************************
  * @file    PortalFrame.c
  * @author  Dedibot
  * @version V1.2.0
  * @date    
  * @brief   ���ż�
*********************************************************************************/

/*******************************************
	������ϵ
********************************************/
#include "PortalFrame.h"             	//���ż�
#include "ProtocolData.h"
#include "Print.h"
#include "DataParse.h"

/*******************************************
    ���غ�
********************************************/

/*******************************************
    ����ȫ�ֱ���
********************************************/


/*******************************************
	�ⲿ�ο�����
********************************************/

/*******************************************
	�ڲ���������
********************************************/

/******************************************
���ܣ�����״̬Ϊ����
�����ߣ�liu-cy
�������ڣ�2016/12/29
�������¼�
����ֵ��
*******************************************/
void PortalFrame_Stop_Y(PORTALFRAME *this, u8 select)
{
	this->extruder_Stru[select].status = PF_STAGE_STANDBY;
	TempCtrl_StopHeat(this->extruder_Stru[select].heatItem);
	PositionMng_Stop_Y(&this->positionMng, select);
	PositionMng_MoveHome_Y(&this->positionMng, select);
}
/*******************************************
		�ⲿ����
*******************************************/
/******************************************
���ܣ�����״̬Ϊ����
�����ߣ�liu-cy
�������ڣ�2016/12/29
�������¼�
����ֵ��
*******************************************/
void PortalFrame_SetStandbyState(PORTALFRAME *this)
{
	int i;
	
	for(i = 0; i < this->extruderCount; i++)
	{
		this->extruder_Stru[i].status = PF_STAGE_STANDBY;
	}
	this->stage = PF_STAGE_STANDBY;
}
/******************************************
���ܣ�����״̬�¼�����
�����ߣ�liu-cy
�������ڣ�2016/12/29
�������¼�
����ֵ��
*******************************************/
void PortalFrame_EventActionOfWait(PORTALFRAME *this, EVENT event)
{
	int i;
	
	switch(event.event)
	{
		case PRINTER_EVENT_STANDBY:					//�ص�����
		case PRINTER_EVENT_STOPFRAME:				//ֹͣ�����żܴ�ӡ
			Print_Exit(this);
		break;
		case PRINTER_EVENT_PRINT:					//��ӡ
		case PRINTER_EVENT_STARTFRAME:		//��ӡ
			if(Print_Start(this))
			{
				for(i = 0; i < this->extruderCount; i++)
				{
					this->extruder_Stru[i].status = PF_STAGE_PRINT;
				}
				this->stage = PF_STAGE_PRINT;
			}
		break;
		case PRINTER_EVENT_CONTROL:					//����,ִ��Gcode
			g_ControlBuf[strlen(g_ControlBuf)] = '\n';
			PrintDataMng_ReceDataToQueue(&this->printDataMng, (char *)g_ControlBuf, strlen(g_ControlBuf));
		break;	
		default:
			break;
	}
}

/******************************************
���ܣ�����״̬�¼�����
�����ߣ�liu-cy
�������ڣ�2016/12/29
�������¼�
����ֵ��
*******************************************/
void PortalFrame_EventActionOfPrint(PORTALFRAME *this, EVENT event)
{
	int i;
	
	switch(event.event)
	{
		case PRINTER_EVENT_STANDBY:					//�ص�����
		case PRINTER_EVENT_STOPFRAME:				//ֹͣ�����żܴ�ӡ
			Print_Stop(this);
			break;
		case PRINTER_EVENT_PRINT:					//��ӡ
			if(NULL != event.notify)
			{
				event.notify(_NG);					//ʧ��
			}
			break;
		case PRINTER_EVENT_CONTROL:					//����,ִ��Gcode
			break;
		case PRINTER_EVENT_PAUSE:
			Print_Pause(this);
			break;
		case PRINTER_EVENT_CONTINUE:
			Print_Continue(this);
			break;
		case PRINTER_EVENT_STOP_FRAME3_Y:
			PortalFrame_Stop_Y(this, g_PortalFrame_Y_Operation);
			break;
		default:
			break;
	}
}
/******************************************
���ܣ��¼�����
�����ߣ�liu-cy
�������ڣ�2016/12/29
������
����ֵ��ȡ�õ��¼�
*******************************************/
void PortalFrame_EventAction(PORTALFRAME *this, EVENT e)
{
	switch(this->stage)
	{
		case PF_STAGE_STANDBY:				//����״̬
			PortalFrame_EventActionOfWait(this, e);
			break;	
		case PF_STAGE_PRINT:				//��ӡ
			PortalFrame_EventActionOfPrint(this, e);
			break;
		default:break;
	}
}

/******************************************
���ܣ�����״̬ȡ��
�����ߣ�cao-dl
�������ڣ�2014/12/23
��������
����ֵ��
	g_printer.status����ǰ����״̬
*******************************************/
u8 PortalFrame_Get_MainStatus( PORTALFRAME *this )
{
	return this->stage;
}

/******************************************
���ܣ���ӡ��״̬����
�����ߣ�liu-cy
�������ڣ�2016/11/24
��������
����ֵ����
*******************************************/
void PortalFrame_Pro(PORTALFRAME *this)
{
	bool res;
	u8	length;								//�洢�������ݳ���

	PrintDataMng_Pro(&this->printDataMng);
	switch(this->stage)
	{
		//����
		case PF_STAGE_STANDBY:
			if(False != PositionMng_CanMove(&this->positionMng))
			{				
				res = PrintDataMng_GetOneLine( &this->printDataMng, this->printInfo.gcode, &length );
				if(True == res)
				{
					this->printInfo.gcode[length] = '\0';
					DataParse_G_M_Ctrl( this, this->printInfo.gcode );	//ִ��Gcode
				}
			}
			break;
		//��ӡ
		case PF_STAGE_PRINT:
			Print_Pro(this);
			break;
		default:
			break;
	}
	PositionMng_Pro(&this->positionMng);
}

void PortalFrame_Setup( PORTALFRAME *this, PORTALFRAME_CONFIG *config )
{
	int i;
	
	PrintDataMng_SetDataSource(&this->printDataMng, DATASOURCE_LOCAL);
	PrintDataMng_SetDataFileName(&this->printDataMng, config->printFileName[config->printFileSel]);
	for(i = 0; i < this->extruderCount; i++)
	{
		this->extruder_Stru[i].angle = config->extruderAngle;
	}
	
	if(config->printMode == PRINTMODE_NORMAL)
	{
		PositionMng_SetNormalMode(&this->positionMng);
	}
	else if(config->printMode == PRINTMODE_INFINITE)
	{
		PositionMng_SetInfiniteMode(&this->positionMng);
	}
}

void PortalFrame_Go_Standby( PORTALFRAME *this )
{
	this->stage = PF_STAGE_STANDBY;
}

/******************************************
���ܣ���ʼ��
�����ߣ�chen-lj
�������ڣ�2017-07-08
��������
����ֵ����
*******************************************/
void PortalFrame_Init_1( PORTALFRAME *this )
{
	this->stage = PF_STAGE_STANDBY;
	
	this->extruderCount = g_ExtruderCount_1;
	this->extruderCodes = (char **)g_ExtruderCodes_1;
	this->extruder_Stru = (EXTRUDER_STRU *)g_Extruder_Stru_1;
	this->current_gcode_line = 0;
	this->total_gcode_line = 10000;
	
	PositionMng_Init_1(&this->positionMng);
	PrintDataMng_Init(&this->printDataMng);
	
	Print_Init(this);
}

/******************************************
���ܣ���ʼ��
�����ߣ�chen-lj
�������ڣ�2017-07-08
��������
����ֵ����
*******************************************/
void PortalFrame_Init_2( PORTALFRAME *this )
{
	this->stage = PF_STAGE_STANDBY;
	
	this->extruderCount = g_ExtruderCount_2;
	this->extruderCodes = (char **)g_ExtruderCodes_2;
	this->extruder_Stru = (EXTRUDER_STRU *)g_Extruder_Stru_2;
	this->current_gcode_line = 0;
	this->total_gcode_line = 10000;
	
	PositionMng_Init_2(&this->positionMng);
	PrintDataMng_Init(&this->printDataMng);
	
	Print_Init(this);
}

/******************************************
���ܣ���ʼ��
�����ߣ�chen-lj
�������ڣ�2017-07-08
��������
����ֵ����
*******************************************/
void PortalFrame_Init_3( PORTALFRAME *this )
{
	this->stage = PF_STAGE_STANDBY;
	
	this->extruderCount = g_ExtruderCount_3;
	this->extruderCodes = (char **)g_ExtruderCodes_3;
	this->extruder_Stru = (EXTRUDER_STRU *)g_Extruder_Stru_3;
	this->current_gcode_line = 0;
	this->total_gcode_line = 10000;
	
	PositionMng_Init_3(&this->positionMng);
	PrintDataMng_Init(&this->printDataMng);
	
	Print_Init(this);
}


