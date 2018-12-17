/********************** (C) COPYRIGHT 2015 DediBot ******************************
  * @file    Input.c
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-03-28
  * @brief   IO控制
*********************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef INPUT_H
#define INPUT_H
#include "stm32f429xx.h"
#include "de_type.h"

/* 信号定义 */
enum _INPUT{
//	INPUT_KEY1,				//按键
//	INPUT_KEY2,				//按键
	INPUT_LIMIT1,			//限位开关
	INPUT_LIMIT2,			//限位开关
	INPUT_LIMIT3,			//限位开关
	INPUT_LIMIT4,			//限位开关
	INPUT_LIMIT5,			//限位开关
	INPUT_LIMIT6,			//限位开关
	INPUT_LIMIT7,			//限位开关
	INPUT_LIMIT8,			//限位开关
	INPUT_LIMIT9,			//限位开关
	INPUT_LIMIT10,			//限位开关
	INPUT_LIMIT11,			//限位开关
	INPUT_LIMIT12,			//限位开关
	INPUT_LIMIT13,			//限位开关
	INPUT_LIMIT14,			//限位开关
	INPUT_LIMIT15,			//限位开关
	INPUT_LIMIT16,			//限位开关
	
	INPUT_MATERIAL1,		//有无料信号
	INPUT_MATERIAL2,		//有无料信号
	INPUT_MATERIAL3,		//有无料信号
	INPUT_MATERIAL4,		//有无料信号
	INPUT_MATERIAL5,		//有无料信号
	INPUT_MATERIAL6,		//有无料信号

//	INPUT_MATERIAL_BLOCK1,	//堵料信号
//	INPUT_MATERIAL_BLOCK2,	//堵料信号
	
	INPUT_SD,				//SD卡拨插信号检测输入引脚，低电平表示插上SD卡，高电平表示SD卡拨出

	INPUT_MAX
};
/*******************************************
	输出接口
********************************************/
//IO初始化
extern void Input_Init(void);
//入力端口状态监视
extern void Input_Watch(void);
//入力端口状态取得
// 有防抖处理
extern u8 Input_GetState( enum _INPUT);
//无防抖处理
extern u8 Input_GetDirectState( enum _INPUT);

#endif //INPUT_H
