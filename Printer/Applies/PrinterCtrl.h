/********************** (C) COPYRIGHT 2015 DediBot ******************************
  * @file    StatusManage.h
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-03-28
  * @brief   状态管理
*********************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STATUS_MANAGE_H
#define STATUS_MANAGE_H
#include "stm32f4xx.h"
#include "EventMng.h"
#include "PortalFrame.h"             	//龙门架
#include "fatfs.h"

typedef enum
{
	PORTALFRAME_1,
	PORTALFRAME_2,
	PORTALFRAME_3,
	
	PORTALFRAME_MAX
}PORTALFRAME_SEL;

typedef struct
{
	bool isActive[PORTALFRAME_MAX];
	bool hasConfig[PORTALFRAME_MAX];
	PORTALFRAME portalFrame[PORTALFRAME_MAX];
	PORTALFRAME_SEL active_PortalFrame;
	
	FIL configFile;
	PORTALFRAME_CONFIG config[PORTALFRAME_MAX];
	
	struct{
		u8 isSDExistFlag:1;				//SD卡是否存在
	}flags;
	
}PRINTER;

extern PRINTER g_printer;

/*******************************************
	输出接口
********************************************/
extern void PrinterCtrl_Init(void);

extern void PrinterCtrl_Pro(void);

extern void PrinterCtrl_EventAction(void);

extern void PrinterCtrl_Run( void );

extern void PrinterCtrl_SwitchFrame(PORTALFRAME_SEL frame);

extern void PrinterCtrl_ReadConfig(void);

extern void SetupPortalFrame(PORTALFRAME_SEL frame, u8 gcodeFileNum);

#endif  //STATUS_MANAGE_H
