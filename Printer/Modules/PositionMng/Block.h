/********************** (C) COPYRIGHT 2016 DediBot ******************************
  * @file    block.h
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-07-07
  * @brief   
*********************************************************************************/
#ifndef BLOCK_H
#define BLOCK_H

#include "axisctrl.h"

#define  FORCE_INLINE __attribute__((always_inline)) __INLINE

// This struct is used when buffering the setup for each linear movement "nominal" values are as specified in
// the source g-code and may never actually be reached if acceleration management is active.
typedef struct {
	
	u8	axesCount;
	enum _AXIS axes[AXIS_MAX];
	
	// Fields used by the bresenham algorithm for tracing the line
	long steps[AXIS_MAX];                     // Step count along each axis
	unsigned long step_event_count;           // The number of step events required to complete this block
	long accelerate_until;                    // The index of the step event on which to stop acceleration
	long decelerate_after;                    // The index of the step event on which to start decelerating
	long acceleration_rate;                   // The acceleration rate used for acceleration calculation
	unsigned long direction_bits;             // The direction bit set for this block (refers to *_DIRECTION_BIT in config.h)

	// Fields used by the motion planner to manage acceleration
	// float speed_x, speed_y, speed_z, speed_e;          // Nominal mm/sec for each axis
	float nominal_speed;                               // The nominal speed for this block in mm/sec
	float entry_speed;                                 // Entry speed at previous-current junction in mm/sec
	float max_entry_speed;                             // Maximum allowable junction entry speed in mm/sec
	float millimeters;                                 // The total travel of this block in mm
	float acceleration;                                // acceleration mm/sec^2
	unsigned char recalculate_flag;                    // Planner flag to recalculate trapezoids on entry junction
	unsigned char nominal_length_flag;                 // Planner flag for nominal speed always reached

	// Settings for the trapezoid generator
	unsigned long nominal_rate;                        // The nominal step rate for this block in step_events/sec
	unsigned long initial_rate;                        // The jerk-adjusted step rate at start of block
	unsigned long final_rate;                          // The minimal rate at exit
	unsigned long acceleration_st;                     // acceleration steps/sec^2

	volatile char busy;
} block_t;

//缓冲区相关设置
#define BLOCK_BUFFER_SIZE		16												//缓冲区指令个数
#define BLOCK_MOD(n) ((n)&(BLOCK_BUFFER_SIZE-1))

typedef struct {
	block_t blocks[BLOCK_BUFFER_SIZE];            								//缓冲区
	volatile unsigned char m_Block_Buffer_Head;           						//缓冲区的头
	volatile unsigned char m_Block_Buffer_Tail;           						//缓冲区的尾
}BLOCK_MNGER;


/*******************************************************************
功能：取得下一个block的下标
作成日期：2016/7/7
参数:
	int8_t block_index	基准block的下标
返回值：
	int8_t	下一个block的下标
********************************************************************/
FORCE_INLINE int8_t Block_Next_Index(int8_t block_index) 
{ 
	return BLOCK_MOD(block_index + 1); 
}
/*******************************************************************
功能：取得上一个block的下标
作成日期：2016/7/7
参数:
	int8_t block_index	基准block的下标
返回值：
	int8_t	上一个block的下标
********************************************************************/
FORCE_INLINE int8_t Block_Prev_Index(int8_t block_index)
{ 
	return BLOCK_MOD(block_index - 1); 
}

/*******************************************************************
功能：询问缓冲区是否已满
作成日期：2016/7/7
参数: 无
返回值：
	bool	TRUE：	已满
			FALSE：	未满
********************************************************************/
FORCE_INLINE bool Block_Is_Full(BLOCK_MNGER *bm)
{
	return (bool)(BLOCK_MOD(bm->m_Block_Buffer_Head + 1) == bm->m_Block_Buffer_Tail);
}
/*******************************************************************
功能：询问缓冲区是否为空
作成日期：2016/7/7
参数: 无
返回值：
bool		TRUE：	不空
			FALSE：	空
********************************************************************/
FORCE_INLINE bool Block_Is_Queued(BLOCK_MNGER *bm) 
{ 
	return (bool)(bm->m_Block_Buffer_Head != bm->m_Block_Buffer_Tail); 
}

/*******************************************************************
功能：缓冲区中指令个数
作成日期：2016/7/7
参数:无
返回值：缓冲区中指令个数
********************************************************************/
FORCE_INLINE uint8_t Block_MovesPlanned(BLOCK_MNGER *bm) 
{ 
	return BLOCK_MOD(bm->m_Block_Buffer_Head - bm->m_Block_Buffer_Tail + BLOCK_BUFFER_SIZE); 
}
/*******************************************************************
功能：缓冲区尾部后移
作成日期：2016/7/7
参数: 无
返回值：无
********************************************************************/
FORCE_INLINE void Block_Discard_Current_Block(BLOCK_MNGER *bm) 
{
	if (Block_Is_Queued(bm))
		bm->m_Block_Buffer_Tail = BLOCK_MOD(bm->m_Block_Buffer_Tail + 1);
}

/*******************************************************************
功能：取得尾部移动指令
作成日期：2016/7/7
参数: 无
返回值：
	block_t*	移动指令
********************************************************************/
FORCE_INLINE block_t* Block_Get_Current_Block(BLOCK_MNGER *bm) {
	if (Block_Is_Queued(bm)) {
		block_t* block = &(bm->blocks[bm->m_Block_Buffer_Tail]);
		block->busy = True;
		return block;
	}
	else
		return NULL;
}

/*******************************************************************
功能：清空
作成日期：2016/7/7
参数: 无
返回值：
	block_t*	移动指令
********************************************************************/
FORCE_INLINE void Block_Clear(BLOCK_MNGER *bm) {
	bm->m_Block_Buffer_Tail = bm->m_Block_Buffer_Head;
}


#endif /* BLOCK_H */
