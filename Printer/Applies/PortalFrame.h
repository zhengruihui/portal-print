/********************** (C) COPYRIGHT 2015 DediBot ******************************
  * @file    PortalFrame.h
  * @author  Dedibot
  * @version V1.2.0
  * @date    
  * @brief   ���ż�
*********************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PORTALFRAME_H
#define PORTALFRAME_H
#include "stm32f4xx.h"
#include "structs.h"
#include "PositionMng.h"
#include "PrintDataMng.h"
#include "EventMng.h"
#include "configuration.h"

#define GCODEFILE_NUM_MAX	(20)
//��ӡ�е���״̬
typedef enum 
{
	PRINT_STAGE_STOPPRINT,		//ֹͣ��
	PRINT_STAGE_READPRINTTIME,	//��ȡ��ӡʱ��
	PRINT_STAGE_WAITLIMIT,     //�ȴ���ʼ�ź�
	PRINT_STAGE_PRINTING,		//��ӡ��
	PRINT_STAGE_WAITTEMP,		//�ȴ�����
	PRINT_STAGE_STOPWAITING,	//ֹͣ�ȴ���
	PRINT_STAGE_STOPACTION,	 	//ֹͣ������
	PRINT_STAGE_PAUSEACTION,	//��ͣ������
	PRINT_STAGE_PAUSE,			//��ͣ��

	
	
	PRINT_STAGE_MAX
}PRINT_STAGE;

//��ӡʱ��Ҫ�������Ϣ
typedef struct {
	PRINT_STAGE stage;			    //��ӡ״̬
	
	u32 totalgcodeSize;				//gcode���ֽ���

	u32 printGcodeCount;			//�Ѵ�ӡgcode�ֽ���
	
	u32 TotalPrintTime;
	
	u32 PrintTime;					//��ӡʱ��
	
	EVENT eventSaved;				//������¼�
	
	char gcode[ONELINE_GCODE_MAXSIZE];
	
}PRINTINFO;

//��ӡ����Ҫ״̬
typedef enum
{
	PF_STAGE_STANDBY,
	PF_STAGE_PRINT,
	PF_STAGE_PAUSE,
	
}PF_STAGE;

//��ӡģʽ
typedef enum
{
	PRINTMODE_NORMAL,
	PRINTMODE_INFINITE
}PRINTMODE;


typedef struct
{
	PRINTMODE printMode;
	u8	printFileCount;
	u8	printFileSel;
	char printFileName[GCODEFILE_NUM_MAX][100];
	float extruderAngle;
}PORTALFRAME_CONFIG;


typedef struct
{
	PF_STAGE stage;
	
	int extruderCount;
	char **extruderCodes;
	EXTRUDER_STRU *extruder_Stru;
	
	POSITIONMNG positionMng;
	PRINTDATAMNG printDataMng;
	
	PRINTINFO printInfo;
	
	float current_gcode_line;
	float total_gcode_line;
	
}PORTALFRAME;



/*******************************************
	����ӿ�
********************************************/
extern void PortalFrame_Init_1( PORTALFRAME *this );
extern void PortalFrame_Init_2( PORTALFRAME *this );
extern void PortalFrame_Init_3( PORTALFRAME *this );
extern void PortalFrame_Setup( PORTALFRAME *this, PORTALFRAME_CONFIG *config );

extern void PortalFrame_Go_Standby( PORTALFRAME *this );

extern void PortalFrame_EventAction(PORTALFRAME *this, EVENT e);
extern void PortalFrame_Pro(PORTALFRAME *this);

extern u8 PortalFrame_Get_MainStatus( PORTALFRAME *this );

#endif  //STATUS_MANAGE_H