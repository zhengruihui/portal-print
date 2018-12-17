/********************** (C) COPYRIGHT 2016 DediBot ******************************
  * @file    Macro.h
  * @author  Dedibot
  * @version V1.2.0
  * @date    
  * @brief   宏定义
*********************************************************************************/
#ifndef MACRO_H
#define MACRO_H
#include "stepper.h"

//改变临界区内存时，关中断进行保护
#define CRITICAL_SECTION_START  __disable_irq();
#define CRITICAL_SECTION_END    __enable_irq();

#define DISABLE_TIMER_INT		do	{														\
									int i;													\
									for(i = 0; i < MOVE_MODULE_MAX; i++)					\
									{														\
										DISABLE_STEPPER_DRIVER_INTERRUPT(&g_MoveModules[i]);\
									}														\
								}while(0);
									
#define ENABLE_TIMER_INT		do	{														\
									int i;													\
									for(i = 0; i < MOVE_MODULE_MAX; i++)					\
									{														\
										ENABLE_STEPPER_DRIVER_INTERRUPT(&g_MoveModules[i]);	\
									}														\
								}while(0);


#endif /* MACRO_H */
