/********************** (C) COPYRIGHT 2017 DediBot ******************************
  * @file    ProtocolData.h
  * @author  Dedibot
  * @version V
  * @date    2017-05-24
  * @brief   wifi通信管理
*********************************************************************************/
#include "wifimng.h"
#include "printerctrl.h"
#include "portalframe.h"
#include "globals.h"
#include "tempctrl.h"
#include "tim.h"
#include "usart.h"

#define TIMER_CYCLE_BASE	(TIM7_CYCLE)
#define UPLOAD_INTERVAL		(100000)

#define EXTRUDER_DIS	(50)

u8 m_SendBuffer[1024];
u32 m_SendDataLen;
u32 m_Upload_timer;

extern long m_CurrentPosition[AXIS_MAX];               									//当前绝对位置 step

typedef struct
{
	u8 status;
	u8 temp[4];
	u8 x[4];
	u8 y[4];
	u8 z[4];
	u8 w[4];
}EXTRUDER_DATA;

typedef struct
{
	u8 printMode;
	u8 frameStatus;
	u8 printTime[4];
	u8 printProcess[2];
	u8 extruderAngle;
	
	u8 extruderCount;
	EXTRUDER_DATA *extruderData;
}PORTALFRAME_DATA;


typedef struct
{
	u8 frameCount;
	PORTALFRAME_DATA *frameData;
}UPLOAD_DATA;


UPLOAD_DATA m_UploadData;
PORTALFRAME_DATA m_PortalFrame_Data[PORTALFRAME_MAX];
EXTRUDER_DATA m_Extruder_Data[EXTRUDER_MAX];


void WifiMng_Init(void)
{
	int extruderCount = 0;
	
	m_UploadData.frameCount = PORTALFRAME_MAX;
	m_UploadData.frameData = m_PortalFrame_Data;
	
	m_PortalFrame_Data[PORTALFRAME_1].extruderCount = g_printer.portalFrame[PORTALFRAME_1].extruderCount;
	m_PortalFrame_Data[PORTALFRAME_1].extruderData = &m_Extruder_Data[EXTRUDER_1];
	extruderCount += g_printer.portalFrame[PORTALFRAME_1].extruderCount;
	
	m_PortalFrame_Data[PORTALFRAME_2].extruderCount = g_printer.portalFrame[PORTALFRAME_2].extruderCount;
	m_PortalFrame_Data[PORTALFRAME_2].extruderData = &m_Extruder_Data[EXTRUDER_1+extruderCount];
	extruderCount += g_printer.portalFrame[PORTALFRAME_2].extruderCount;
	
	m_PortalFrame_Data[PORTALFRAME_3].extruderCount = g_printer.portalFrame[PORTALFRAME_3].extruderCount;
	m_PortalFrame_Data[PORTALFRAME_3].extruderData = &m_Extruder_Data[EXTRUDER_1+extruderCount];
	
	m_Upload_timer = 0;
}

void Wifi_GetData(void)
{
	int frameId;
	int extruderId;
	s16 process = 0;
	s32 time;
	s32 temp;
	s32 position;
	
	for(frameId = 0; frameId < m_UploadData.frameCount; frameId++)
	{
		PORTALFRAME_DATA *pfData = &m_PortalFrame_Data[frameId];
		
		pfData->printMode = g_printer.config[frameId].printMode;
		
		pfData->frameStatus = PortalFrame_Get_MainStatus(&g_printer.portalFrame[frameId]);
		
		time = g_printer.portalFrame[frameId].printInfo.TotalPrintTime;
		pfData->printTime[0] = time >> (32-8);
		pfData->printTime[1] = (time >> (32-16)) & 0x0FF;
		pfData->printTime[2] = (time >> 8) & 0x0FF;
		pfData->printTime[3] = time & 0x0FF;
			
		if(pfData->frameStatus == PF_STAGE_STANDBY)
		{
			memset(pfData->printProcess, 0, sizeof(pfData->printProcess));
		}
		else
		{
			process = 1000 * ((float)g_printer.portalFrame[frameId].printInfo.printGcodeCount/g_printer.portalFrame[frameId].printInfo.totalgcodeSize);
			pfData->printProcess[0] = process >> 8;
			pfData->printProcess[1] = process & 0x0FF;
		}
		
		pfData->extruderAngle = g_printer.portalFrame[frameId].extruder_Stru[0].angle;
		
		for(extruderId = 0; extruderId < pfData->extruderCount; extruderId++)
		{
			EXTRUDER_DATA *exData = &pfData->extruderData[extruderId];
			
			exData->status = g_printer.portalFrame[frameId].extruder_Stru[extruderId].status;
			
			temp = TempCtrl_getCurrentTemp(g_printer.portalFrame[frameId].extruder_Stru[extruderId].heatItem);
			exData->temp[0] = temp >> (32-8);
			exData->temp[1] = (temp >> (32-16)) & 0x0FF;
			exData->temp[2] = (temp >> 8) & 0x0FF;
			exData->temp[3] = temp & 0x0FF;
			
			position = g_printer.portalFrame[frameId].positionMng.mainAxesPos[0] * 10;
			exData->x[0] = position >> (32-8);
			exData->x[1] = (position >> (32-16)) & 0x0FF;
			exData->x[2] = (position >> 8) & 0x0FF;
			exData->x[3] = position & 0x0FF;
			position = g_printer.portalFrame[frameId].positionMng.mainAxesPos[1] * 10;
			exData->y[0] = position >> (32-8);
			exData->y[1] = (position >> (32-16)) & 0x0FF;
			exData->y[2] = (position >> 8) & 0x0FF;
			exData->y[3] = position & 0x0FF;
			if(frameId < PORTALFRAME_3)
			{
				position = g_printer.portalFrame[frameId].positionMng.mainAxesPos[2] * 10;
			}
			else
			{
				position = g_printer.portalFrame[frameId].positionMng.mainAxesPos[3] * 10;
			}
			exData->z[0] = position >> (32-8);
			exData->z[1] = (position >> (32-16)) & 0x0FF;
			exData->z[2] = (position >> 8) & 0x0FF;
			exData->z[3] = position & 0x0FF;
			
			if(g_printer.config[frameId].printMode == PRINTMODE_INFINITE)
			{
				position = g_printer.portalFrame[frameId].positionMng.freeAxesPos[0] * 10;
			}
			else
			{
				position = 0;
			}
			exData->w[0] = position >> (32-8);
			exData->w[1] = (position >> (32-16)) & 0x0FF;
			exData->w[2] = (position >> 8) & 0x0FF;
			exData->w[3] = position & 0x0FF;
		}
	}
}

void WifiMng_Package(void)
{
	int index = 0;
	int frameId;
	int extruderId;
	
	m_SendBuffer[index] = m_UploadData.frameCount;
	index++;
	
	for(frameId = 0; frameId < m_UploadData.frameCount; frameId++)
	{
		PORTALFRAME_DATA *pfData = &m_PortalFrame_Data[frameId];
		
		m_SendBuffer[index] = pfData->printMode;
		index++;
		
		m_SendBuffer[index] = pfData->frameStatus;
		index++;
		
		memcpy(&m_SendBuffer[index], pfData->printTime, 4);
		index += 4;
		
		memcpy(&m_SendBuffer[index], pfData->printProcess, 2);
		index += 2;
		
		m_SendBuffer[index] = pfData->extruderAngle;
		index++;
		
		m_SendBuffer[index] = pfData->extruderCount;
		index++;
		
		for(extruderId = 0; extruderId < pfData->extruderCount; extruderId++)
		{
			EXTRUDER_DATA *exData = &pfData->extruderData[extruderId];
		
			memcpy(&m_SendBuffer[index], &exData->status, 1);
			index += 1;
		
			memcpy(&m_SendBuffer[index], exData->temp, 4);
			index += 4;
		
			memcpy(&m_SendBuffer[index], exData->x, 4);
			index += 4;
		
			memcpy(&m_SendBuffer[index], exData->y, 4);
			index += 4;
		
			memcpy(&m_SendBuffer[index], exData->z, 4);
			index += 4;
		
			memcpy(&m_SendBuffer[index], exData->w, 4);
			index += 4;
		}
	}
	m_SendDataLen = index;
}

void WifiMng_Timer_5_0(void)
{
	m_Upload_timer += TIMER_CYCLE_BASE;
	
	if(m_Upload_timer >= UPLOAD_INTERVAL)
	{
		Wifi_GetData();
		WifiMng_Package();
		USART1_Write(m_SendBuffer, m_SendDataLen);
		
		m_Upload_timer = 0;
	}
}

























