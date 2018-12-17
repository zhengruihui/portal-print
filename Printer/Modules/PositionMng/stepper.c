/********************** (C) COPYRIGHT 2016 DediBot ******************************
  * @file    Stepper.c
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-07-07
  * @brief   电机控制算法-在时间中断中计算间隔，并输出脉冲
*********************************************************************************/
#include "stepper.h"
#include "string.h"
#include "math.h"
#include "input.h"
#include "speed_lookuptable.h"
#include "AxisCtrl.h"
#include "tim.h"
#include "calculate.h"
#include "macros.h"
#include "globals.h"

#define MAX_STEP_FREQUENCY 65535 // Max step frequency for Ultimaker (5000 pps / half step)

#define LIMIT_ON				(HIGH)
#define LIMIT_OFF				(LOW)

#define WAIT_TIMER				(&htim6)			//定时器选择
#define WAIT_TIMER_FREQ			(1000000)			//1MHz
#define WAIT_TIME_STEP			(4)					//步进脉冲宽度3~4us
#define WAIT_TIME_DIR			(5)					//方向信号改变后等待5us

#define WAIT_TIMER_COUNT		(__HAL_TIM_GET_COUNTER(WAIT_TIMER))					//当前计数值
#define WAIT_TIMER_PERIOD		(__HAL_TIM_GET_AUTORELOAD(WAIT_TIMER)+1)			//计数周期
#define WAIT_TIME2CLK(time)		(time*WAIT_TIMER_FREQ/1000000)						//等待的时钟数计算

//等待处理
#define DO_WAIT(start, time)	do\
								{\
									if(WAIT_TIMER_COUNT >= start)\
									{\
										if(WAIT_TIMER_COUNT - start >= WAIT_TIME2CLK(time) )\
										{\
											break;\
										}\
									}\
									else\
									{\
										if(WAIT_TIMER_COUNT+WAIT_TIMER_PERIOD - start >= WAIT_TIME2CLK(time))\
										{\
											break;\
										}\
									}\
								}while(1);

/*******************************************
    本地变量
********************************************/
MOVE_MODULE g_MoveModules[MOVE_MODULE_MAX];

static unsigned long direction_bits;        											//上一条指令的方向
static long counter[AXIS_MAX];													//用于移动判断
static volatile long count_position[AXIS_MAX]; 									//实际当前位置 step
static volatile signed char count_direction[AXIS_MAX];							//带有方向的单步移动


static volatile u32 m_EndStop_Hit_Bits = 0; //限位开关是否触发过标志位

static TIM_HandleTypeDef *const m_TimDefine[MOVE_MODULE_MAX] = 
{
/* MOVE_MODULE_1 */		&htim2,
/* MOVE_MODULE_2 */     &htim3,
/* MOVE_MODULE_3 */     &htim4,
/* MOVE_MODULE_4 */     &htim5,
/* MOVE_MODULE_5 */		&htim9
};


/*******************************************
	内部函数声明
********************************************/
//计算 (u32 * u32)>>24
static void MultiU24X32toH16(unsigned short *result, unsigned long mul1, unsigned long mul2);
//速度step/s → 中断间隔
static FORCE_INLINE unsigned short Calc_Timer(MOVE_MODULE *this, unsigned short step_rate);
//设置每个轴的方向
static void Set_Stepper_Direction(MOVE_MODULE *this);
//新一条移动指令的初始化
static FORCE_INLINE void Trapezoid_Generator_Reset(MOVE_MODULE *this);
//设置中断时间
static FORCE_INLINE void Set_Speed(MOVE_MODULE *this, u16 speed);
/*******************************************
	内部函数
********************************************/
/*******************************************************************
功能：(u32 * u32)>>24
作成日期：2016/7/7
参数: 
	unsigned short *result	结果
	unsigned long mul1		乘数
	unsigned long mul2		被乘数
返回值：无
********************************************************************/
void MultiU24X32toH16(unsigned short *result, unsigned long mul1, unsigned long mul2)
{
	unsigned long mul1h16 = (mul1 >> 16);
	unsigned long mul1l16 = (mul1 & 0x0FFFF);
	unsigned long mul2h16 = (mul2 >> 16);
	unsigned long mul2l16 = (mul2 & 0x0FFFF);
	
	*result = ((mul1h16*mul2h16) << 8) + (((mul1h16 * mul2l16) + (mul2h16 * mul1l16) + ((mul1l16 * mul2l16) >> 16)) >> 8);
}

/*******************************************************************
功能：设置中断时间
作成日期：
参数: 
返回值：
********************************************************************/
void Set_Speed(MOVE_MODULE *this, u16 speed)
{
	__HAL_TIM_SET_AUTORELOAD(this->tim,speed);		//设置自动重装值
}
/*******************************************************************
功能：速度step/s → 中断间隔
作成日期：2016/7/7
参数: 
	unsigned short step_rate	速度step/s
返回值：
	中断间隔
********************************************************************/
FORCE_INLINE unsigned short Calc_Timer(MOVE_MODULE *this, unsigned short step_rate)
{
	unsigned short timer;
	u8 step_loops;

	NOMORE(step_rate, MAX_STEP_FREQUENCY);

	if (step_rate > 20000) { // If steprate > 20kHz >> step 4 times
		step_rate = (step_rate >> 2) & 0x3fff;
		step_loops = 4;
	}
	else if (step_rate > 10000) { // If steprate > 10kHz >> step 2 times
		step_rate = (step_rate >> 1) & 0x7fff;
		step_loops = 2;
	}
	else {
		step_loops = 1;
	}

	NOLESS(step_rate, 1 << 5);
	step_rate -= 1 << 5; // Correct for minimal speed
	if (step_rate >= (8 * 256)) { // higher step rate
		const unsigned short *table_address = &speed_lookuptable_fast[(unsigned char)(step_rate >> 8)][0];
		unsigned char tmp_step_rate = (step_rate & 0x00ff);
		unsigned short gain = *(table_address + 1);

		timer = *table_address - (((unsigned long)tmp_step_rate * (unsigned long)gain) >> 8);
	}
	else { // lower step rates
		const unsigned short *table_address = &speed_lookuptable_slow[(unsigned char)(step_rate >> 3)][0];
		unsigned char tmp_step_rate = (step_rate & 0x0007);
		unsigned short gain = *(table_address + 1);
		timer = *table_address - ((tmp_step_rate * gain) >> 3);
	}
	if (timer < 100) { 
		timer = 100; 
	}//(20kHz this should never happen)
	
	this->step_loops = step_loops;
	
	return timer;
}

/*******************************************************************
功能：设置每个轴的方向
	X_AXIS=A_AXIS and Y_AXIS=B_AXIS for COREXY
	X_AXIS=A_AXIS and Z_AXIS=C_AXIS for COREXZ
作成日期：2016/7/7
参数: 无
返回值：无
********************************************************************/
void Set_Stepper_Direction(MOVE_MODULE *this) 
{
	u8 i;
	enum _AXIS axis;
	
	for(i = 0; i < this->current_block->axesCount; i++)
	{
		axis = this->current_block->axes[i];
		if(TEST(direction_bits, axis))
		{
			AxisCtrl_SetDirBack(axis);
			count_direction[axis] = -1;
		}
		else
		{
			AxisCtrl_SetDirFront(axis);
			count_direction[axis] = 1;
		}
	}
}

/*******************************************************************
功能：新一条移动指令的初始化
作成日期：2016/7/7
参数: 无
返回值：无
********************************************************************/
FORCE_INLINE void Trapezoid_Generator_Reset(MOVE_MODULE *this) 
{
	int i;
	int bit = 0;
	long mask = 0;
	
	for(i = 0; i < this->current_block->axesCount; i++)
	{
		bit = 1 << this->current_block->axes[i];
		if((bit & this->disabled_Axis_Mask) == 0)
		{
			mask |= bit;
		}
		else
		{
			this->current_block->steps[this->current_block->axes[i]] = 0;		//该轴不移动
		}
	}

	if ((this->current_block->direction_bits & mask) != (direction_bits & mask)) {
		direction_bits = (direction_bits & ~mask) | (this->current_block->direction_bits & mask);
		Set_Stepper_Direction(this);
	}

	this->deceleration_time = 0;
	// step_rate to timer interval
	this->OCR1A_nominal = Calc_Timer(this, this->current_block->nominal_rate);
	// make a note of the number of step loops required at nominal speed
	this->step_loops_nominal = this->step_loops;
	this->acc_step_rate = this->current_block->initial_rate;
	this->acceleration_time = Calc_Timer(this, this->acc_step_rate);
	Set_Speed(this, this->acceleration_time);		//设置打印速度
}

void Update_EndStops(MOVE_MODULE *this)
{
	int i;
	enum _AXIS axis;
	u8 limitLvl;
	bool isStop = True;
	
	for(i = 0; i < this->current_block->axesCount; i++)
	{
		axis = this->current_block->axes[i];
		
		if(this->current_block->steps[axis] > 0)
		{
			limitLvl = Input_GetState(g_Axis_Limit_Tbl[axis]);					//取得限位开关状态
			
			if(limitLvl == LIMIT_ON)											//限位开关触发
			{
				SBI(m_EndStop_Hit_Bits, (u8)axis);
				
				if(TEST(direction_bits, axis))	//-方向
				{
					this->current_block->steps[axis] = 0;//停止移动
				}
			}
			isStop = False;
		}
	}
	if(isStop)
	{
		this->step_events_completed = this->current_block->step_event_count;
	}
}
/*******************************************
	外部函数
********************************************/

//         __________________________
//        /|                        |\     _________________         ^
//       / |                        | \   /|               |\        |
//      /  |                        |  \ / |               | \       s
//     /   |                        |   |  |               |  \      p
//    /    |                        |   |  |               |   \     e
//   +-----+------------------------+---+--+---------------+----+    e
//   |               BLOCK 1            |      BLOCK 2          |    d
//
//                           time ----->
//
//  The trapezoid is the shape the speed curve over time. It starts at block->initial_rate, accelerates
//  first block->accelerate_until step_events_completed, then keeps going at constant speed until
//  step_events_completed reaches block->decelerate_after after which it decelerates until the trapezoid generator is reset.
//  The slope of acceleration is calculated using v = u + at where t is the accumulated timer values of the steps so far.
/*******************************************************************
功能：时间中断函数
	计算步进，输出脉冲
作成日期：2016/7/7
参数: 无
返回值：无
********************************************************************/
void Stepper_Linear_Move( MOVE_MODULE *this )
{
    unsigned short timer;
	block_t *current_block = this->current_block;
	
	// If there is no current block, attempt to pop one from the buffer
	if (!current_block) {
	// Anything in the buffer?
		current_block = Block_Get_Current_Block(&this->block_Mnger);
		if (current_block) {
			u8 i;
			long count;
			this->current_block = current_block;
			current_block->busy = True;
			Trapezoid_Generator_Reset(this);
			count = -(current_block->step_event_count >> 1);
			for(i = 0; i < current_block->axesCount; i++)
			{
				counter[current_block->axes[i]] = count;
			}
			this->step_events_completed = 0;
		}
		else {
			Set_Speed(this, 20000);//0.1ms
		}
	}

	if (current_block != NULL) {
		int8_t i, idx;
		enum _AXIS axis;
		unsigned short step_rate;
		
		if ((this->endStopsType > ENDSTOP_DISABLED) )
		{
			Update_EndStops(this);
		}

		// Take multiple steps per interrupt (For high speed moves)
		for (i = 0; i < this->step_loops; i++) {
			for(idx = 0; idx < current_block->axesCount; idx++)
			{
				axis = current_block->axes[idx];
				counter[axis] += current_block->steps[axis];
				if(counter[axis] >= 0)
				{
					AxisCtrl_Move(axis);
				}
			}
			{u16 temp = 100; while(--temp);}
			for(idx = 0; idx < current_block->axesCount; idx++)
			{
				axis = current_block->axes[idx];
				if(counter[axis] >= 0)
				{
					counter[axis] -= current_block->step_event_count;
					count_position[axis] += count_direction[axis]; 
					AxisCtrl_Stop(axis);
				}
			}
			if(this->step_loops > 1)
			{
				u16 temp = 100; 
				while(--temp);
			}

			this->step_events_completed++;
			if (this->step_events_completed >= current_block->step_event_count) break;
		}
		// Calculate new timer value
		if (this->step_events_completed <= (unsigned long)current_block->accelerate_until) {//加速中
			MultiU24X32toH16(&this->acc_step_rate, this->acceleration_time, current_block->acceleration_rate);
			this->acc_step_rate += current_block->initial_rate;

			// upper limit
			NOMORE(this->acc_step_rate, current_block->nominal_rate);

			// step_rate to timer interval
			timer = Calc_Timer(this, this->acc_step_rate);

			Set_Speed(this, timer);
			this->acceleration_time += timer;
		}
		else if (this->step_events_completed > (unsigned long)current_block->decelerate_after) {//减速中
			MultiU24X32toH16(&step_rate, this->deceleration_time, current_block->acceleration_rate);

			if (step_rate <= this->acc_step_rate) { // Still decelerating?
				step_rate = this->acc_step_rate - step_rate;
				NOLESS(step_rate, current_block->final_rate);
			}
			else
				step_rate = current_block->final_rate;

			// step_rate to timer interval
			timer = Calc_Timer(this, step_rate);

			Set_Speed(this, timer);
			this->deceleration_time += timer;
		}
		else {
			Set_Speed(this, this->OCR1A_nominal);
			this->step_loops = this->step_loops_nominal;
		}

		// If current block is finished, reset pointer
		if (this->step_events_completed >= current_block->step_event_count) {			//到达目标位置
			this->current_block = NULL;
			Block_Discard_Current_Block(&this->block_Mnger);
		}
	}
	//防止出中断时，计数器的值已经大于自动重载值，导致不触发中断
	if(__HAL_TIM_GET_COUNTER(this->tim) >= __HAL_TIM_GET_AUTORELOAD(this->tim))
	{
		__HAL_TIM_SET_COUNTER(this->tim, __HAL_TIM_GET_AUTORELOAD(this->tim)-10);
	}
}

 /*******************************************************************
功能：设置现在实际位置
作成日期：2016/7/7
参数: 
	long *position		XYZE轴现在位置
返回值：无
********************************************************************/
void Stepper_Set_Position(MOVE_MODULE *this, POSPARAM *posParam) {
	int i;
	
	CRITICAL_SECTION_START;

	for(i = 0; i < posParam->axesCount; i++)
	{
		count_position[posParam->axes[i]] = lround(posParam->position[i] * g_Axis_Steps_per_Unit[posParam->axes[i]]);
	}
	CRITICAL_SECTION_END;
}

bool Stepper_Is_MoveFinished(MOVE_MODULE *this)
{
	return ((Block_Is_Queued(&this->block_Mnger))?False: True);
}

bool Stepper_Is_Full(MOVE_MODULE *this)
{
	return ((Block_Is_Full(&this->block_Mnger))?True: False);
}

/*******************************************************************
功能：中断函数
作成日期：2016/7/7
参数: 
返回值：无
********************************************************************/
void Stepper_Tim2_Int( void )
{
	Stepper_Linear_Move(&g_MoveModules[MOVE_MODULE_1]);
}
void Stepper_Tim3_Int( void )
{
	Stepper_Linear_Move(&g_MoveModules[MOVE_MODULE_2]);
}
void Stepper_Tim4_Int( void )
{
	Stepper_Linear_Move(&g_MoveModules[MOVE_MODULE_3]);
}
void Stepper_Tim5_Int( void )
{
	Stepper_Linear_Move(&g_MoveModules[MOVE_MODULE_4]);
}
void Stepper_Tim9_Int( void )
{
	Stepper_Linear_Move(&g_MoveModules[MOVE_MODULE_5]);
}

/*******************************************************************
功能：设置E轴现在位置
作成日期：2016/7/7
参数: 
	float e				E轴现在位置
返回值：无
********************************************************************/
// void Stepper_Set_E_Position(long e,u8 E_Number) {
	// CRITICAL_SECTION_START;
	// count_position[E_AXIS+E_Number] = e;
	// CRITICAL_SECTION_END;
// }

/*******************************************************************
功能：取得对应轴当前实际位置 step
作成日期：2016/7/7
参数: 
	enum _AXIS axis		轴编号
返回值：
	该轴当前实际位置 step
********************************************************************/
long Stepper_Get_Position(enum _AXIS axis) {
	return count_position[axis];
}

/*******************************************************************
功能：取得对应轴当前实际位置 mm
作成日期：2016/7/7
参数: 
	enum _AXIS axis		轴编号
返回值：
	该轴当前实际位置 mm
********************************************************************/
float Stepper_Get_Axis_Position_mm(enum _AXIS axis) {
	float axis_steps;
    axis_steps = Stepper_Get_Position(axis);
	
	return axis_steps / g_Axis_Steps_per_Unit[axis];
}

/*******************************************************************
功能：结束打印处理
作成日期：2016/7/7
参数: 无
返回值：无
********************************************************************/
void Stepper_QuickStop(MOVE_MODULE *this) {
	DISABLE_STEPPER_DRIVER_INTERRUPT(this);
	
	Block_Clear(&this->block_Mnger);
	this->current_block = NULL;
	this->disabled_Axis_Mask = 0;
	
	ENABLE_STEPPER_DRIVER_INTERRUPT(this);
}

void Stepper_StopAxis(MOVE_MODULE *this, enum _AXIS axis)
{
	int i;
	
	DISABLE_STEPPER_DRIVER_INTERRUPT(this);
	
	this->disabled_Axis_Mask |= 1 << axis;
	
	ENABLE_STEPPER_DRIVER_INTERRUPT(this);
}

/*******************************************************************
功能：初始化
作成日期：2016/7/7
参数: 无
返回值：无
********************************************************************/
void Stepper_Init(void) 
{
	int i;
	MOVE_MODULE *this;
	
	for(i = 0; i < MOVE_MODULE_MAX; i++)
	{
		this = &g_MoveModules[i];
		
		memset(&this->block_Mnger, 0, sizeof(this->block_Mnger));
		this->tim = m_TimDefine[i];
		this->current_block = NULL;
		this->endStopsType = ENDSTOP_DISABLED;
		this->disabled_Axis_Mask = 0;
		
		Set_Speed(this, 20000);													//初始化 10ms中断
	}
	direction_bits = 0;															//运动方向前回值保存
	
	for(i = 0; i < AXIS_MAX; i++)
	{
		AxisCtrl_SetDirFront((enum _AXIS)i);
		count_direction[(enum _AXIS)i] = 1;
	}
}

/*******************************************************************
功能：限位开关启用/停用控制
作成日期：2016/10/25
参数: 无
返回值：无
********************************************************************/
void Stepper_EndStops_SetFlag(MOVE_MODULE *this, enum ENDSTOP_TYPE type) 
{
	this->endStopsType = type; 
}

/*******************************************************************
功能：清除限位开关状态标志
作成日期：2016/10/26
参数: 无
返回值：无
********************************************************************/
// void Stepper_EndStops_Hit_Clear( void ) 
// {
	// m_EndStop_Hit_Bits = 0; 
// }

/*******************************************************************
功能：获取限位开关是否触发过
作成日期：2016/11/21
参数: enum EndstopEnum endstop
返回值：限位开关是否触发过的状态
********************************************************************/
// u8 Stepper_EndStops_Hit_Get( enum EndstopEnum endstop) 
// {
	// if(endstop == ENDSTOPS_MAX)
	// {
		// return m_EndStop_Hit_Bits;
	// }
	// else if(ENDSTOPS_XYZ_MAX == endstop)
	// {
		// return (m_EndStop_Hit_Bits & ((1<<X_MIN) | (1<<Y_MIN) | (1<<Z_MIN)));
	// }
	// else
	// {
		// return (m_EndStop_Hit_Bits & (1<<endstop));
	// }
// }

/*******************************************************************
功能：获取限位开关当前状态
作成日期：2016/11/21
参数: enum EndstopEnum endstop
返回值：限位开关当前状态
********************************************************************/
// u8 Stepper_EndStops_GetStatus( enum EndstopEnum endstop)
// {
	// if(endstop == ENDSTOPS_MAX)
	// {
		// return m_Old_EndStop_Bits;
	// }
	// else if(ENDSTOPS_XYZ_MAX == endstop)
	// {
		// return (m_Old_EndStop_Bits & ((1<<X_MIN) | (1<<Y_MIN) | (1<<Z_MIN)));
	// }
	// else
	// {
		// return (m_Old_EndStop_Bits & (1<<endstop));
	// }
// }

