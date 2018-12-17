/********************** (C) COPYRIGHT 2015 DediBot ******************************
  * @file    PrinterCtrl.c
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-03-28
  * @brief   状态管理
*********************************************************************************/

/*******************************************
	包含关系
********************************************/
#include "PrinterCtrl.h"				//打印机控制
#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#include "ErrorMng.h"					//异常管理
#include "input.h"
#include "ProtocolMng.h"
#include "ProtocolData.h"
#include "macros.h"
#include "wifimng.h"
#include "BrakeCtrl.h"
#include "RS485.h"

/*******************************************
    本地宏
********************************************/
#define SD_EXIST						LOW
#define SD_NOTEXIST						HIGH

#define CONFIG_FILENAME					"config.ini"

/*******************************************
    本地全局变量
********************************************/
PRINTER g_printer;


static char * const m_PortalFrameSearchString_Start[PORTALFRAME_MAX] = 
{
	"PortalFrame_1_Start",
	"PortalFrame_2_Start",
	"PortalFrame_3_Start"
};

static char * const m_PortalFrameSearchString_End[PORTALFRAME_MAX] = 
{
	"PortalFrame_1_End",
	"PortalFrame_2_End",
	"PortalFrame_3_End"
};

#define STR_FILENAME_START	"filename_start"
#define STR_FILENAME_END	"filename_end"

#define STR_MODE_NORMAL		"normal"
#define STR_MODE_INFINITE	"infinite"

/*******************************************
	外部参考变量
********************************************/

/*******************************************
	内部函数声明
********************************************/
static void SD_Check(void)
{
	u8 isSD = SD_EXIST;//Input_GetState(INPUT_SD);
	
	if(isSD == SD_EXIST)
	{
		if(g_printer.flags.isSDExistFlag == False)
		{
			//Logger_string("SD card detected.\n");
			//HAL_Delay(1000);
			if(FR_OK == FATFS_MountSDdisk())
			{
				Logger_string("mount SD card.\n");
				
				PrinterCtrl_ReadConfig();
				g_printer.flags.isSDExistFlag = True;
			}
		}
	}
	else
	{
		if(g_printer.flags.isSDExistFlag == True)
		{
			Logger_string("SD card removed.\n");
			
			if(FR_OK == FATFS_UmountSDdisk())
			{
				Logger_string("umount SD card.\n");
			}
			//不管卸载成不成功，都设置为不存在
			g_printer.flags.isSDExistFlag = False;
		}
	}
}

void ReadConfigFile(void)
{
	int i;
	char readData[100] = {0};
	char *pData;
	PORTALFRAME_SEL portal;
	u8 stage;
	u8 res;
	u8 count;
	
	enum {
		NONE,
		PORTAL_FRAME,
		FILE_NAME,
		ANGLE,
		MODE
	};
	
	stage = NONE;
	
	Logger_string("start to read configure\n");
		
	memset(g_printer.config, 0, sizeof(g_printer.config));
	memset(g_printer.hasConfig, 0, sizeof(g_printer.hasConfig));
	
	res = FATFS_OpenFile(&g_printer.configFile, CONFIG_FILENAME, FA_READ);
	if(res == 0)
	{
		Logger_string("open configure file successfully.\n");

		while(FATFS_GetOneLine(&g_printer.configFile, readData) != 0)
		{
			pData = strtok(readData, "\n");
			pData = strtok(readData, "\r");
			switch(stage)
			{
				case NONE:
					for(i = 0; i < PORTALFRAME_MAX; i++)
					{
						if(strcmp(pData, m_PortalFrameSearchString_Start[i]) == 0)
						{
							portal = (PORTALFRAME_SEL)i;
							g_printer.hasConfig[i] = True;
							stage = PORTAL_FRAME;
							
							Logger_string("portal frame: ");
							Logger_long(portal);
							Logger_string(" start\n");
								
							break;
						}
					}
				break;
				case PORTAL_FRAME:
					if(strcmp(pData, STR_FILENAME_START) == 0)
					{
						stage = FILE_NAME;
						
						Logger_string(STR_FILENAME_START);
						Logger_string("\n");
					}
					break;
				case FILE_NAME:
					if(strcmp(pData, STR_FILENAME_END) == 0)
					{
						stage = ANGLE;
						
						Logger_string(STR_FILENAME_END);
						Logger_string("\n");
					}
					else
					{
						count = g_printer.config[portal].printFileCount;
						if(count < GCODEFILE_NUM_MAX)
						{
							strcpy(g_printer.config[portal].printFileName[count], pData);
							g_printer.config[portal].printFileCount++;
						
							Logger_string("filename: ");
							Logger_string(g_printer.config[portal].printFileName[count]);
							Logger_string("\n");
						}
					}
					break;
				case ANGLE:
					g_printer.config[portal].extruderAngle = (float)atof(pData);
					stage = MODE;
					
					Logger_string("angle: ");
					Logger_long(g_printer.config[portal].extruderAngle);
					Logger_string("\n");
					break;
				case MODE:
					if(strcmp(pData, STR_MODE_NORMAL) == 0)
					{
						Logger_string("print mode: normal\n");
						g_printer.config[portal].printMode = PRINTMODE_NORMAL;
					}
					if(strcmp(pData, STR_MODE_INFINITE) == 0)
					{
						Logger_string("print mode: infinite\n");
						g_printer.config[portal].printMode = PRINTMODE_INFINITE;
					}
					stage = NONE;
					break;
			}
			// for(i = 0; i < PORTALFRAME_MAX; i++)
			// {
				// if(strcmp(pData, m_PortalFrameSearchString_End[i]) == 0)
				// {
					// stage = NONE;
					
					// Logger_string("portal frame: ");
					// Logger_long(portal);
					// Logger_string(" end\n");
						
					// break;
				// }
			// }
		}
			

			// isPortal = False;
			// for(i = 0; i < 100; i++)
			// {
				// if(isspace(readData[i]))
				// {
					// readData[i] = '\0';
				// }
			// }
			// for(i = 0; i < PORTALFRAME_MAX; i++)
			// {
				// if(strcmp(readData, m_PortalFrameSearchString[i]) == 0)
				// {
					// portal = (PORTALFRAME_SEL)i;
					// g_printer.hasConfig[i] = True;
					// isPortal = True;
					
					// Logger_string("portal frame: ");
					// Logger_long(portal);
					// Logger_string("\n");
						
					// break;
				// }
			// }
			// if(isPortal)
			// {
				// if(FATFS_GetOneLine(&g_printer.configFile, readData) != 0)
				// {
					// for(i = 0; i < 100; i++)
					// {
						// if(isspace(readData[i]))
						// {
							// readData[i] = '\0';
						// }
					// }
					// strcpy(g_printer.config[portal].printFileName, readData);
					
					// Logger_string("filename: ");
					// Logger_string(g_printer.config[portal].printFileName);
					// Logger_string("\n");
				// }
				// if(FATFS_GetOneLine(&g_printer.configFile, readData) != 0)
				// {
					// g_printer.config[portal].extruderAngle = (float)atof(readData);
					
					// Logger_string("angle: ");
					// Logger_long(g_printer.config[portal].extruderAngle);
					// Logger_string("\n");
				// }
				// if(FATFS_GetOneLine(&g_printer.configFile, readData) != 0)
				// {
					// for(i = 0; i < 100; i++)
					// {
						// if(isspace(readData[i]))
						// {
							// readData[i] = '\0';
						// }
					// }
					// if(strcmp(readData, "normal") == 0)
					// {
						// Logger_string("print mode: normal\n");
						// g_printer.config[portal].printMode = PRINTMODE_NORMAL;
					// }
					// if(strcmp(readData, "infinite") == 0)
					// {
						// Logger_string("print mode: infinite\n");
						// g_printer.config[portal].printMode = PRINTMODE_INFINITE;
					// }
				// }
			// }
	}
	else
	{
		Logger_string("failed to open configure file.\n");
	}
	FATFS_CloseFile(&g_printer.configFile);
}

void SetupPortalFrame(PORTALFRAME_SEL frame, u8 gcodeFileNum)
{
	if(	(g_printer.hasConfig[frame]) && (PortalFrame_Get_MainStatus(&g_printer.portalFrame[frame]) == PF_STAGE_STANDBY) &&
		(gcodeFileNum < g_printer.config[frame].printFileCount))
	{
		g_printer.config[frame].printFileSel = gcodeFileNum;
		PortalFrame_Setup(&g_printer.portalFrame[frame], &g_printer.config[frame]);
	}
	else
	{
		Logger_string("failed to setup: no configure or portal frame is already printing.\n");
	}
}

/*******************************************
		外部函数
*******************************************/
/******************************************
功能：事件处理
作成者：liu-cy
作成日期：2016/12/29
参数：
返回值：取得的事件
*******************************************/
void PrinterCtrl_EventAction(void)
{
	EVENT event;
	int i;
	
	if(EventMng_GetEvent(&event) != True)
	{
		return;
	}
	
	switch(event.event)
	{
		case PRINTER_EVENT_STANDBY:				//待机状态
			//全部停止
			Logger_string("all portal frame stop printing\n");
			//for(i = 0; i < PORTALFRAME_MAX; i++)
			//{
				PortalFrame_EventAction(&g_printer.portalFrame[g_printer.active_PortalFrame], event);
			//}
			
			Logger_string("complete\n");
			break;	
		case PRINTER_EVENT_PRINT:				//打印
//			Logger_string("all portal frame start to print\n");
//			//开始打印
//			if(!g_printer.flags.isSDExistFlag)
//			{
//				//无SD卡处理
//			}
//			for(i = 0; i < PORTALFRAME_MAX; i++)
//			{
//				SetupPortalFrame((PORTALFRAME_SEL)i, g_GcodeFileSelect);
//				PortalFrame_EventAction(&g_printer.portalFrame[i], event);
//			}
//			
//			Logger_string("complete\n");
			Logger_string("portal frame ");
			Logger_long(g_printer.active_PortalFrame);
			Logger_string(" start to print.\n");
			
			
			SetupPortalFrame(g_printer.active_PortalFrame, g_GcodeFileSelect);
			PortalFrame_EventAction(&g_printer.portalFrame[g_printer.active_PortalFrame], event);
			
			Logger_string("complete\n");
			break;
		case PRINTER_EVENT_READCONFIG:				//读取配置文件
			PrinterCtrl_ReadConfig();
			break;
		case PRINTER_EVENT_SW_FRAME:				//切换龙门架
			Logger_string("switch to portal frame ");
			Logger_long(g_PortalFrameSelect);
			Logger_string("\n");
			
			PrinterCtrl_SwitchFrame((PORTALFRAME_SEL)g_PortalFrameSelect);
			
			Logger_string("complete\n");
			break;
		case PRINTER_EVENT_STARTFRAME:				//选中的龙门架开始打印
			Logger_string("portal frame ");
			Logger_long(g_printer.active_PortalFrame);
			Logger_string(" start to print.\n");
			
			
			SetupPortalFrame(g_printer.active_PortalFrame, g_GcodeFileSelect);
			PortalFrame_EventAction(&g_printer.portalFrame[g_printer.active_PortalFrame], event);
			
			Logger_string("complete\n");
			break;
		case PRINTER_EVENT_STOPFRAME:				//选中的龙门架停止打印
			Logger_string("portal frame ");
			Logger_long(g_printer.active_PortalFrame);
			Logger_string(" stop printing.\n");
			
			PortalFrame_EventAction(&g_printer.portalFrame[g_printer.active_PortalFrame], event);
			
			Logger_string("complete\n");
			break;
		case PRINTER_EVENT_PAUSE:
			Logger_string(" pause printing.\n");
			PortalFrame_EventAction(&g_printer.portalFrame[g_printer.active_PortalFrame], event);
			break;
		case PRINTER_EVENT_CONTINUE:
			Logger_string(" continue printing.\n");
			PortalFrame_EventAction(&g_printer.portalFrame[g_printer.active_PortalFrame], event);
			break;
		case PRINTER_EVENT_STOP_FRAME3_Y:			//第三个龙门架的选中的Y轴结束打印
			Logger_string("No.");
			Logger_long(g_PortalFrame_Y_Operation);
			Logger_string(" Extruder of portal frame 3 stops printing.\n");
			
			if(g_printer.active_PortalFrame == PORTALFRAME_3)
			{
				PortalFrame_EventAction(&g_printer.portalFrame[g_printer.active_PortalFrame], event);
				Logger_string("complete\n");
			}
			else
			{
				Logger_string("wrong portal frame is selected.\n");
			}
			
			break;
		case PRINTER_EVENT_SETWAITTEMP:
//			for(i = 0; i < PORTALFRAME_MAX; i++)
//			{
//				if(PortalFrame_Get_MainStatus(&g_printer.portalFrame[i]) == PF_STAGE_PRINT)
//				{
//					PortalFrame_EventAction(&g_printer.portalFrame[i], event);
//				}
//			}
			PortalFrame_EventAction(&g_printer.portalFrame[g_printer.active_PortalFrame], event);
			break;
		default:
			PortalFrame_EventAction(&g_printer.portalFrame[g_printer.active_PortalFrame], event);
			break;
	}
}
/******************************************
功能：打印机状态处理
作成者：liu-cy
作成日期：2016/11/24
参数：无
返回值：无
*******************************************/
void PrinterCtrl_Pro(void)
{
	PrinterCtrl_EventAction();		//打印机事件处理
}

void PrinterCtrl_Run(void)
{
	int i;
	
	SD_Check();
	//RS485_run();
	ProtocolMng_Pro();				//数据解析
	PrinterCtrl_Pro();				//打印机控制
	
	for(i = 0; i < PORTALFRAME_MAX; i++)
	{
		PortalFrame_Pro(&g_printer.portalFrame[i]);
	}
}

void PrinterCtrl_SwitchFrame(PORTALFRAME_SEL frame)
{
	if(frame < PORTALFRAME_MAX)
	{
		g_printer.active_PortalFrame = frame;
	}
}

void PrinterCtrl_ReadConfig(void)
{
	//读取配置文件，分配龙门架的打印任务
	ReadConfigFile();
}

void testFan()
{
	u8 i;
	
	for(i = 0; i < FAN_MAX; i++)
	{
		FanCtrl_SetFan((_FAN)i, 100);	
	}
}

void test()
{
	testFan();
}

/******************************************
功能：初始化
作成者：cao-dl
作成日期：2014/12/23
参数：无
返回值：无
*******************************************/
void PrinterCtrl_Init( void )
{
	FanCtrl_Init();
	HeatCtrl_Init();
	Input_Init();
	TempSensor_Init();
	AxisCtrl_Init();
	
	ProtocolMng_InitPort(LINK_PORT_COM);
	ErrorMng_Init();
	EventMng_Init();
	
	ErrorMng_Init();
	EventMng_Init();
	TempCtrl_Init();
	
	Stepper_Init();
	Planner_Init();
	
	g_printer.flags.isSDExistFlag = False;
	g_printer.active_PortalFrame = PORTALFRAME_2;
	PortalFrame_Init_1(&g_printer.portalFrame[PORTALFRAME_1]);
	PortalFrame_Init_2(&g_printer.portalFrame[PORTALFRAME_2]);
	PortalFrame_Init_3(&g_printer.portalFrame[PORTALFRAME_3]);
	
	BrakeCtrl_ON();
	
	WifiMng_Init();
	
//	test();
}

