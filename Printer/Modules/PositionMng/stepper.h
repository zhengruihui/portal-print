/********************** (C) COPYRIGHT 2016 DediBot ******************************
  * @file    Stepper.h
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-07-07
  * @brief   电机控制算法-在时间中断中计算间隔，并输出脉冲
*********************************************************************************/

#ifndef STEPPER_H
#define STEPPER_H
#include "block.h"
#include "structs.h"


//定时器频率设置
#define STEPTIMER_DIV			(TIM3_DIV36)									/* 定时器时钟分频 */
#define STEPTIMER_CLOCK			2000000											/* 定时器时钟频率 2MHz */

//定时器相关临界区保护
#define ENABLE_STEPPER_DRIVER_INTERRUPT(this)  HAL_TIM_Base_Start_IT((this)->tim)	//触发中断使能
#define DISABLE_STEPPER_DRIVER_INTERRUPT(this) HAL_TIM_Base_Stop_IT((this)->tim)	//触发中断禁止

enum ENDSTOP_TYPE
{
	ENDSTOP_DISABLED,
	ENDSTOP_ENABLED
};

typedef enum
{
	MOVE_MODULE_1,
	MOVE_MODULE_2,
	MOVE_MODULE_3,
	MOVE_MODULE_4,
	MOVE_MODULE_5,
	// MOVE_MODULE_6,
	
	MOVE_MODULE_MAX

}MOVE_MODULE_SEL;

typedef struct 
{
	TIM_HandleTypeDef *tim;
	
	BLOCK_MNGER block_Mnger;
	
	block_t* current_block;														//当前指令
	volatile unsigned long step_events_completed; 								//已移动步数
	long disabled_Axis_Mask;

	long acceleration_time, deceleration_time;									//累计加速时间，减速时间
	unsigned short acc_step_rate; 												//加速阶段的速度 step/s
	uint8_t step_loops;															//单次中断的移动步数
	uint8_t step_loops_nominal;													//匀速运动是单次中断的移动步数
	unsigned short OCR1A_nominal;												//匀速运动时的中断间隔

	enum ENDSTOP_TYPE endStopsType;
}MOVE_MODULE;


extern MOVE_MODULE g_MoveModules[MOVE_MODULE_MAX];


/*******************************************
	外部函数
********************************************/
// Set current position in steps
extern void Stepper_Set_Position(MOVE_MODULE *this, POSPARAM *posParam);
extern bool Stepper_Is_MoveFinished(MOVE_MODULE *this);
extern bool Stepper_Is_Full(MOVE_MODULE *this);
// extern void Stepper_Set_E_Position(long e,u8 E_Number);

// Get current position in steps
extern long Stepper_Get_Position(enum _AXIS axis);
// Get current axis position in mm
extern float Stepper_Get_Axis_Position_mm(enum _AXIS axis);

extern void Stepper_QuickStop(MOVE_MODULE *this);
extern void Stepper_StopAxis(MOVE_MODULE *this, enum _AXIS axis);

extern void Stepper_Tim2_Int( void );
extern void Stepper_Tim3_Int( void );
extern void Stepper_Tim4_Int( void );
extern void Stepper_Tim5_Int( void );
extern void Stepper_Tim6_Int( void );
extern void Stepper_Tim7_Int( void );
extern void Stepper_Tim9_Int( void );


extern void Stepper_Init(void);

extern void Stepper_EndStops_SetFlag(MOVE_MODULE *this, enum ENDSTOP_TYPE flag);

// extern void Stepper_EndStops_Hit_Clear( void ) ;

// extern u8 Stepper_EndStops_Hit_Get( enum EndstopEnum endstop) ;
// extern u8 Stepper_EndStops_GetStatus( enum EndstopEnum endstop) ;

#endif /* STEPPER_H */
