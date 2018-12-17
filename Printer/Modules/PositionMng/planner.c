/********************** (C) COPYRIGHT 2016 DediBot ******************************
  * @file    Planner.c
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-07-07
  * @brief   电机控制算法-速度曲线计算
*********************************************************************************/
#include "planner.h"
#include "math.h"
#include "stdlib.h"
#include "AxisCtrl.h"
#include "macros.h"
#include "Configuration.h"
#include "Calculate.h"
#include "stepper.h"
#include "usbd_cdc_if.h"



// default settings
#define DEFAULT_ACCELERATION          3000		// 默认打印加速度	mm/s^2
#define DEFAULT_RETRACT_ACCELERATION  3000		// 默认回抽加速度
#define DEFAULT_TRAVEL_ACCELERATION   3000		// 默认空走加速度

// The speed change that does not require acceleration (i.e. the software might assume it can be done instantaneously)
#define DEFAULT_JERK                  20.0    	// (mm/sec)

#define DEFAULT_MINIMUMFEEDRATE       0.0     // minimum feedrate
#define DEFAULT_MINTRAVELFEEDRATE     0.0

// Minimum planner junction speed. Sets the default minimum speed the planner plans for at the end
// of the buffer and all stops. This should not be much greater than zero and should only be changed
// if unwanted behavior is observed on a user's machine when running at very slow speeds.
#define MINIMUM_PLANNER_SPEED 0.05f// (mm/sec)

//最多步数轴的移动步数小于这个值，则认为不移动
#define DROPSEGMENTS		5

/*******************************************
	外部参考变量
********************************************/
float g_Max_Feedrate[AXIS_MAX]; 												//各轴速度上限
float g_Axis_Steps_per_Unit[AXIS_MAX];											//各轴每毫米的步进数 step/mm
float g_MinimumFeedrate;															//打印最低速度
float g_MinTravelFeedrate;														//空走最低速度
float g_Acceleration;         													//打印加速度 mm/s^2. 通过 M204 SXXXX 来设置
float g_Retract_Acceleration; 													//回抽加速度 mm/s^2. 通过 M204 TXXXX 来设置
float g_Travel_Acceleration;  													//空走加速度 mm/s^2. 通过 M204 MXXXX 来设置
float g_Max_jerk[AXIS_MAX];          												//轴最大瞬时速度变化量
unsigned long g_Max_Acceleration_Units_per_Sqr_Second[AXIS_MAX]; 					//各轴加速度上限 mm/s^2
unsigned long g_Axis_Steps_per_Sqr_Second[AXIS_MAX];								//各轴加速度上限 step/s^2


/*******************************************
    本地变量
********************************************/
static long m_CurrentPosition[AXIS_MAX];               							//当前绝对位置 step
static float m_CurrentPosition_Float[AXIS_MAX];               					//当前绝对位置 step
static float m_Previous_Speed[AXIS_MAX]; 										//上一条指令各轴速度

/*******************************************
	内部函数声明
********************************************/
//计算加速阶段的移动距离
static FORCE_INLINE float Estimate_Acceleration_Distance(float initial_rate, float target_rate, float acceleration);
static FORCE_INLINE float Intersection_Distance(float initial_rate, float final_rate, float acceleration, float distance);
static void Calculate_Trapezoid_for_Block(block_t* block, float entry_factor, float exit_factor);//速度的梯形曲线计算
//计算以一定加速度在一定距离内能够加速到目标速度的最大/最小速度
static FORCE_INLINE float Cal_Max_Allowable_Speed(float acceleration, float target_velocity, float distance);
//缓冲区中指令开始速度变化后更新上一条移动指令的开始速度
static void Planner_Reverse_Pass_Kernel(block_t* previous, block_t* current, block_t* next);
//根据新加的移动指令从头到尾更新缓冲区中的移动指令的开始速度
static void Planner_Reverse_Pass(PLANNER *this);
//缓冲区中指令开始速度变化后更新下一条移动指令的开始速度
static void Planner_Forward_Pass_Kernel(block_t* previous, block_t* current, block_t* next);
//从尾到头更新缓冲区中的移动指令的开始速度
static void Planner_Forward_Pass(PLANNER *this);
//重新计算所有缓冲区指令的速度的梯形曲线
static void Planner_Recalculate_Trapezoids(PLANNER *this);
//根据新加移动指令更新缓冲区指令数据
static void Planner_Recalculate(PLANNER *this);
//初始化设置
static void Planner_ResetDefaultConfig(void);

/*******************************************
	内部函数
********************************************/

/*******************************************************************
功能：计算加速阶段的移动距离
作成日期：2016/7/7
参数:
	float initial_rate	起始速度
	float target_rate	目标速度
	float acceleration	加速度
返回值：
	float	加速阶段的移动距离
********************************************************************/
FORCE_INLINE float Estimate_Acceleration_Distance(float initial_rate, float target_rate, float acceleration)
{
	if (acceleration == 0)
	{
		return 0; // acceleration was 0, set acceleration distance to 0
	}
	return (target_rate * target_rate - initial_rate * initial_rate) / (acceleration * 2);
}

/*******************************************************************
功能：计算加速阶段的移动距离
作成日期：2016/7/7
参数:
	float initial_rate	起始速度
	float final_rate	终止速度
	float acceleration	加速度
	float distance		总距离
返回值：
	float	加速阶段的移动距离
********************************************************************/
FORCE_INLINE float Intersection_Distance(float initial_rate, float final_rate, float acceleration, float distance)
{
	if (acceleration == 0)
	{
		return 0; // acceleration was 0, set intersection distance to 0
	}
	return (acceleration * 2 * distance - initial_rate * initial_rate + final_rate * final_rate) / (acceleration * 4);
}

// Calculates trapezoid parameters so that the entry- and exit-speed is compensated by the provided factors.

/*******************************************************************
功能：根据开始速度，结束速度，加速度以及总距离计算速度的梯形曲线
作成日期：2016/7/7
参数:
	block_t* block		移动数据结构
	float entry_factor	开始速度百分比
	float exit_factor	结束速度百分比
返回值：无
********************************************************************/
void Calculate_Trapezoid_for_Block(block_t* block, float entry_factor, float exit_factor) {
	unsigned long initial_rate = ceil(block->nominal_rate * entry_factor),
				final_rate = ceil(block->nominal_rate * exit_factor); // (steps per second)
	long acceleration;
	int32_t accelerate_steps;
	int32_t decelerate_steps;
	int32_t plateau_steps;

	// Limit minimal step rate (Otherwise the timer will overflow.)
	NOLESS(initial_rate, 120);
	NOLESS(final_rate, 120);

	acceleration = block->acceleration_st;
	accelerate_steps = ceil(Estimate_Acceleration_Distance(initial_rate, block->nominal_rate, acceleration));
	decelerate_steps = floor(Estimate_Acceleration_Distance(block->nominal_rate, final_rate, -acceleration));

	// Calculate the size of Plateau of Nominal Rate.
	plateau_steps = block->step_event_count - accelerate_steps - decelerate_steps;

	// Is the Plateau of Nominal Rate smaller than nothing? That means no cruising, and we will
	// have to use intersection_distance() to calculate when to abort acceleration and start braking
	// in order to reach the final_rate exactly at the end of this block.
	if (plateau_steps < 0) {
		accelerate_steps = ceil(Intersection_Distance(initial_rate, final_rate, acceleration, block->step_event_count));
		accelerate_steps = max(accelerate_steps, 0); // Check limits due to numerical round-off
		accelerate_steps = min((uint32_t)accelerate_steps, block->step_event_count);//(We can cast here to unsigned, because the above line ensures that we are above zero)
		plateau_steps = 0;
	}

	// block->accelerate_until = accelerate_steps;
	// block->decelerate_after = accelerate_steps+plateau_steps;
	CRITICAL_SECTION_START;  // Fill variables used by the stepper in a critical section
	if (!block->busy) { // Don't update variables if block is busy.
		block->accelerate_until = accelerate_steps;
		block->decelerate_after = accelerate_steps + plateau_steps;
		block->initial_rate = initial_rate;
		block->final_rate = final_rate;
	}
//	printf("accelerate_steps = %d\n",accelerate_steps);
//	printf("decelerate_steps = %d\n",decelerate_steps);
	CRITICAL_SECTION_END;
}

/*******************************************************************
功能：计算以一定加速度在一定距离内能够加速到目标速度的最大/最小速度
作成日期：2016/7/7
参数:
	float acceleration		加速度
	float target_velocity	目标速度
	float distance			移动距离
返回值：
	float	速度
********************************************************************/
FORCE_INLINE float Cal_Max_Allowable_Speed(float acceleration, float target_velocity, float distance)
{
	return sqrt(target_velocity * target_velocity - 2 * acceleration * distance);
}

/*******************************************************************
功能：缓冲区中指令开始速度变化后更新上一条移动指令的开始速度
作成日期：2016/7/7
参数:
	block_t* previous	上一个指令（不使用）
	block_t* current	当前指令
	block_t* next		下一个指令
返回值：无
********************************************************************/
void Planner_Reverse_Pass_Kernel(block_t* previous, block_t* current, block_t* next)
{
	if (!current) return;

	if (next) {
		// If entry speed is already at the maximum entry speed, no need to recheck. Block is cruising.
		// If not, block in state of acceleration or deceleration. Reset entry speed to maximum and
		// check for maximum allowable speed reductions to ensure maximum possible planned speed.
		float max_entry_speed = current->max_entry_speed;
		if (current->entry_speed != max_entry_speed) {

			// If nominal length true, max junction speed is guaranteed to be reached. Only compute
			// for max allowable speed if block is decelerating and nominal length is false.
			if (!current->nominal_length_flag && max_entry_speed > next->entry_speed) {
				current->entry_speed = min(max_entry_speed,
                                   Cal_Max_Allowable_Speed(-current->acceleration, next->entry_speed, current->millimeters));
			}
			else {
				current->entry_speed = max_entry_speed;
			}
			current->recalculate_flag = True;
		}
	} // Skip last block. Already initialized and set for recalculation.
}

/*******************************************************************
功能：根据新加的移动指令从头到尾更新缓冲区中的移动指令的开始速度
作成日期：2016/7/7
参数: 无
返回值：无
********************************************************************/
void Planner_Reverse_Pass(PLANNER *this) {
	uint8_t block_index = this->block_Mnger->m_Block_Buffer_Head;
	unsigned char tail;

  //Make a local copy of block_buffer_tail, because the interrupt can alter it
	CRITICAL_SECTION_START;
		tail = this->block_Mnger->m_Block_Buffer_Tail;
	CRITICAL_SECTION_END

	if (BLOCK_MOD(this->block_Mnger->m_Block_Buffer_Head - tail + BLOCK_BUFFER_SIZE) > 3) { // moves queued
		block_t* block[3] = { NULL, NULL, NULL };
		block_index = BLOCK_MOD(this->block_Mnger->m_Block_Buffer_Head - 3);
		while (block_index != tail) {
			block_index = Block_Prev_Index(block_index);
			block[2] = block[1];
			block[1] = block[0];
			block[0] = &(this->block_Mnger->blocks[block_index]);
			Planner_Reverse_Pass_Kernel(block[0], block[1], block[2]);
		}
	}
}

/*******************************************************************
功能：缓冲区中指令开始速度变化后更新下一条移动指令的开始速度
作成日期：2016/7/7
参数:
	block_t* previous	上一个指令
	block_t* current	当前指令
	block_t* next		下一个指令（不使用）
返回值：无
********************************************************************/
void Planner_Forward_Pass_Kernel(block_t* previous, block_t* current, block_t* next) {
	if (!previous) return;

	// If the previous block is an acceleration block, but it is not long enough to complete the
	// full speed change within the block, we need to adjust the entry speed accordingly. Entry
	// speeds have already been reset, maximized, and reverse planned by reverse planner.
	// If nominal length is true, max junction speed is guaranteed to be reached. No need to recheck.
	if (!previous->nominal_length_flag) {
		if (previous->entry_speed < current->entry_speed) {
			double entry_speed = min(current->entry_speed,
								   Cal_Max_Allowable_Speed(-previous->acceleration, previous->entry_speed, previous->millimeters));
			// Check for junction speed change
			if (current->entry_speed != entry_speed) {
				current->entry_speed = entry_speed;
				current->recalculate_flag = True;
			}
		}
	}
}

/*******************************************************************
功能：从尾到头更新缓冲区中的移动指令的开始速度
作成日期：2016/7/7
参数: 无
返回值：无
********************************************************************/
void Planner_Forward_Pass(PLANNER *this) {
	uint8_t block_index = this->block_Mnger->m_Block_Buffer_Tail;
	block_t* block[3] = { NULL, NULL, NULL };

	while (block_index != this->block_Mnger->m_Block_Buffer_Head) {
		block[0] = block[1];
		block[1] = block[2];
		block[2] = &(this->block_Mnger->blocks[block_index]);
		Planner_Forward_Pass_Kernel(block[0], block[1], block[2]);
		block_index = Block_Next_Index(block_index);
	}
	Planner_Forward_Pass_Kernel(block[1], block[2], NULL);
}

/*******************************************************************
功能：重新计算所有缓冲区指令的速度的梯形曲线
作成日期：2016/7/7
参数: 无
返回值：无
********************************************************************/
void Planner_Recalculate_Trapezoids(PLANNER *this) {
	int8_t block_index = this->block_Mnger->m_Block_Buffer_Tail;
	block_t* current;
	block_t* next = NULL;

	while (block_index != this->block_Mnger->m_Block_Buffer_Head) {
		current = next;
		next = &(this->block_Mnger->blocks[block_index]);
		if (current) {
			// Recalculate if current block entry or exit junction speed has changed.
			if (current->recalculate_flag || next->recalculate_flag) {
				// NOTE: Entry and exit factors always > 0 by all previous logic operations.
				float nom = current->nominal_speed;
				Calculate_Trapezoid_for_Block(current, current->entry_speed / nom, next->entry_speed / nom);
				current->recalculate_flag = False; // Reset current only to ensure next trapezoid is computed
			}
		}
		block_index = Block_Next_Index(block_index);
	}
	// Last/newest block in buffer. Exit speed is set with MINIMUM_PLANNER_SPEED. Always recalculated.
	if (next) {
		float nom = next->nominal_speed;
		Calculate_Trapezoid_for_Block(next, next->entry_speed / nom, (MINIMUM_PLANNER_SPEED) / nom);
		next->recalculate_flag = False;
	}
}

/*******************************************************************
功能：根据新加移动指令更新缓冲区指令数据
作成日期：2016/7/7
参数: 无
返回值：无
********************************************************************/
void Planner_Recalculate(PLANNER *this) {
	Planner_Reverse_Pass(this);
	Planner_Forward_Pass(this);
	Planner_Recalculate_Trapezoids(this);
}

/*******************************************************************
功能：初始化设置
作成日期：2016/7/7
参数: 无
返回值：无
********************************************************************/
void Planner_ResetDefaultConfig() {
	float tmp1[] = AXIS_STEPS_PER_UNIT;
	float tmp2[] = AXIS_MAX_FEEDRATE;
	long tmp3[] = AXIS_MAX_ACCELERATION;
	float tmp4[] = AXIS_MAX_JERK;
	uint8_t i;
	for (i = 0; i < AXIS_MAX; i++) {
		g_Axis_Steps_per_Unit[i] = tmp1[i];
		g_Max_Feedrate[i] = tmp2[i];
		g_Max_Acceleration_Units_per_Sqr_Second[i] = tmp3[i];
		g_Max_jerk[i] = tmp4[i];
	}

	// steps per sq second need to be updated to agree with the units per sq second
	Planner_Reset_Acceleration_Rates();

	g_Acceleration = DEFAULT_ACCELERATION;
	g_Retract_Acceleration = DEFAULT_RETRACT_ACCELERATION;
	g_Travel_Acceleration = DEFAULT_TRAVEL_ACCELERATION;
	g_MinimumFeedrate = DEFAULT_MINIMUMFEEDRATE;
	g_MinTravelFeedrate = DEFAULT_MINTRAVELFEEDRATE;
}


/*******************************************
	外部函数
********************************************/
/*******************************************************************
功能：初始化
作成日期：2016/7/7
参数: 无
返回值：无
********************************************************************/
void Planner_Init() {
	// int i;
	// m_Block_Buffer_Head = m_Block_Buffer_Tail = 0;
	// memset(m_CurrentPosition, 0, sizeof(m_CurrentPosition)); // clear position
	// for (i = 0; i < AXIS_MAX; i++) m_Previous_Speed[i] = 0.0;
	// m_Previous_Nominal_Speed = 0.0;

	Planner_ResetDefaultConfig();
}

/*******************************************************************
功能：计算移动相关数据，加入缓冲区并更新缓冲区
作成日期：2016/7/7
参数:
	float x				X轴目标位置
	float y				Y轴目标位置
	float z				Z轴目标位置
	float e				E轴目标位置
	float feed_rate		速度
	uint8_t extruder	挤出器
返回值：无
********************************************************************/
void Planner_Buffer_Line(PLANNER *this, POSPARAM *mainAxes, POSPARAM *extruderAxes, float feed_rate)
{
	int i;

	enum _AXIS axis;
	int extruderAxesCount = (extruderAxes == NULL)? 0: extruderAxes->axesCount;
	int totalAxesCount = mainAxes->axesCount + extruderAxesCount;

	BLOCK_MNGER *blockMnger = this->block_Mnger;
	// Calculate the buffer head after we push this byte
	int next_buffer_head = Block_Next_Index(blockMnger->m_Block_Buffer_Head);

	// The target position of the tool in absolute steps
	// Calculate target position in absolute steps
	//this should be done after the wait, because otherwise a M92 code within the gcode disrupts this calculation somehow

	long target[AXIS_MAX];
	long diff[AXIS_MAX];
	long maxEDiff = 0;
	long maxMainDiff = 0;
	long maxDiff = 0;

	unsigned long db = 0;
	unsigned long extrudermove = 0;

	float delta_mm[AXIS_MAX];
	float maxDeltaE_mm = 0;
	float squareDistance = 0;

	float inverse_millimeters;  // Inverse millimeters to remove multiple divides

	// Calculate moves/second for this move. No divide by zero due to previous checks.
	float inverse_second;

	int moves_queued;

	float speed_factor;

	float current_speed[AXIS_MAX];

	float steps_per_mm;
	unsigned long 	acc_st,
					allsteps;

	float vmax_junction[AXIS_MAX];
	float vmax_junction_Min;
	float vmax_junction_factor;

	float safe_speed;
	double v_allowable;

	// Prepare to set up new block
	block_t* block = &blockMnger->blocks[blockMnger->m_Block_Buffer_Head];

	block->axesCount = totalAxesCount;
	for (i = 0; i < totalAxesCount; i++)
	{
		block->axes[i] = (i < mainAxes->axesCount)? mainAxes->axes[i]: extruderAxes->axes[i-mainAxes->axesCount];
	}

	// The target position of the tool in absolute steps
	// Calculate target position in absolute steps
	//this should be done after the wait, because otherwise a M92 code within the gcode disrupts this calculation somehow

	for(i = 0; i < mainAxes->axesCount; i++)
	{
		axis = mainAxes->axes[i];
		m_CurrentPosition_Float[axis] = mainAxes->position[i];
		target[axis] = lround(mainAxes->position[i] * g_Axis_Steps_per_Unit[axis]);
		diff[axis] = target[axis] - m_CurrentPosition[axis];
		if (diff[axis] < 0) SBI(db, axis);
		block->steps[axis] = labs(diff[axis]);
		if (block->steps[axis] != 0) AxisCtrl_Enable(axis);//enable active axes
		maxMainDiff = max(maxMainDiff, block->steps[axis]);
	}
	for(i = 0; i < extruderAxesCount; i++)
	{
		axis = extruderAxes->axes[i];
		m_CurrentPosition_Float[axis] = extruderAxes->position[i];
		target[axis] = lround(extruderAxes->position[i] * g_Axis_Steps_per_Unit[axis]);
		diff[axis] = target[axis] - m_CurrentPosition[axis];
		if (diff[axis] < 0) 	SBI(db, axis);
		if (diff[axis] != 0)	SBI(extrudermove, axis);
		block->steps[axis] = labs(diff[axis]);
		if (block->steps[axis] != 0) AxisCtrl_Enable(axis);//enable active axes
		maxEDiff = max(maxEDiff, block->steps[axis]);
	}
	block->step_event_count = max(maxMainDiff, maxEDiff);

	// Mark block as not busy (Not executed by the stepper interrupt)
	block->busy = False;

	// Bail if this is a zero-length block
	if (block->step_event_count <= DROPSEGMENTS) return;

	block->direction_bits = db;

	if (extrudermove)
		NOLESS(feed_rate, g_MinimumFeedrate);
	else
		NOLESS(feed_rate, g_MinTravelFeedrate);


  /**
   * This part of the code calculates the total length of the movement.
   * For cartesian bots, the X_AXIS is the real X movement and same for Y_AXIS.
   * But for corexy bots, that is not true. The "X_AXIS" and "Y_AXIS" motors (that should be named to A_AXIS
   * and B_AXIS) cannot be used for X and Y length, because A=X+Y and B=X-Y.
   * So we need to create other 2 "AXIS", named X_HEAD and Y_HEAD, meaning the real displacement of the Head.
   * Having the real displacement of the head, we can calculate the total movement length and apply the desired speed.
   */

	for(i = 0; i < mainAxes->axesCount; i++)
	{
		axis = mainAxes->axes[i];
		delta_mm[axis] = diff[axis] / g_Axis_Steps_per_Unit[axis];
	}
	for(i = 0; i < extruderAxesCount; i++)
	{
		axis = extruderAxes->axes[i];
		delta_mm[axis] = diff[axis] / g_Axis_Steps_per_Unit[axis];
		maxDeltaE_mm = max(maxDeltaE_mm, fabs(delta_mm[axis]));
	}

	if (maxMainDiff <= DROPSEGMENTS) {
		block->millimeters = maxDeltaE_mm;
	}
	else {
		for(i = 0; i < mainAxes->axesCount; i++)
		{
			axis = mainAxes->axes[i];
			squareDistance += square(delta_mm[axis]);
		}
		block->millimeters = sqrt(squareDistance);
	}

	inverse_millimeters = 1.0f / block->millimeters;  // Inverse millimeters to remove multiple divides

	// Calculate moves/second for this move. No divide by zero due to previous checks.
	inverse_second = feed_rate * inverse_millimeters;

	moves_queued = Block_MovesPlanned(blockMnger);

	// Slow down when the buffer starts to empty, rather than wait at the corner for a buffer refill

	block->nominal_speed = block->millimeters * inverse_second; // (mm/sec) Always > 0
	block->nominal_rate = ceil(block->step_event_count * inverse_second); // (step/sec) Always > 0

	// Calculate and limit speed in mm/sec for each axis
	speed_factor = 1.0f; //factor <=1 do decrease speed
	for (i = 0; i < totalAxesCount; i++)
	{
		float cs, mf;

		axis = (i < mainAxes->axesCount)? mainAxes->axes[i]: extruderAxes->axes[i-mainAxes->axesCount];
		current_speed[axis] = delta_mm[axis] * inverse_second;
		cs = fabs(current_speed[axis]);
		mf = g_Max_Feedrate[axis];
		if (cs > mf) speed_factor = min(speed_factor, mf / cs);

	}
	// Correct the speed
	if (speed_factor < 1.0f)
	{
		for (i = 0; i < totalAxesCount; i++)
		{
		axis = (i < mainAxes->axesCount)? mainAxes->axes[i]: extruderAxes->axes[i-mainAxes->axesCount];
			current_speed[axis] *= speed_factor;
		}
		block->nominal_speed *= speed_factor;
		block->nominal_rate *= speed_factor;
	}

	// Compute and limit the acceleration rate for the trapezoid generator.
	steps_per_mm = block->step_event_count / block->millimeters;

	if (maxMainDiff <= DROPSEGMENTS) {
		block->acceleration_st = ceil(g_Retract_Acceleration * steps_per_mm); // convert to: acceleration steps/sec^2
	}
	else if (maxEDiff <= DROPSEGMENTS) {
		block->acceleration_st = ceil(g_Travel_Acceleration * steps_per_mm); // convert to: acceleration steps/sec^2
	}
	else {
		block->acceleration_st = ceil(g_Acceleration * steps_per_mm); // convert to: acceleration steps/sec^2
	}

	// Limit acceleration per axis
	acc_st = block->acceleration_st;
	allsteps = block->step_event_count;
	for (i = 0; i < totalAxesCount; i++)
	{
		axis = (i < mainAxes->axesCount)? mainAxes->axes[i]: extruderAxes->axes[i-mainAxes->axesCount];
		if (g_Axis_Steps_per_Sqr_Second[axis] < ((float)acc_st * block->steps[axis]) / allsteps)
		{
			acc_st = ((float)g_Axis_Steps_per_Sqr_Second[axis] * allsteps) / block->steps[axis];
		}
	}

	block->acceleration_st = acc_st;
	block->acceleration = acc_st / steps_per_mm;
	block->acceleration_rate = (long)(((float)acc_st * 16777216.0f) / STEPTIMER_CLOCK);

	// Start with a safe speed
	vmax_junction[0] = g_Max_jerk[0]/2;
	vmax_junction_Min = vmax_junction[0];
	for (i = 1; i < totalAxesCount; i++)
	{
		axis = (i < mainAxes->axesCount)? mainAxes->axes[i]: extruderAxes->axes[i-mainAxes->axesCount];
		vmax_junction[axis] = g_Max_jerk[axis]/2;

		if (fabs(current_speed[axis]) > vmax_junction[axis])
		{
			vmax_junction_Min = min(vmax_junction_Min, vmax_junction[axis]);
		}
	}
	vmax_junction_factor = 1.0;

	vmax_junction_Min = min(vmax_junction_Min, block->nominal_speed);
	safe_speed = vmax_junction_Min;


	if ((moves_queued > 1) && (this->m_Previous_Nominal_Speed > 0.0001f)) {

		float jerk;
		for (i = 0; i < totalAxesCount; i++)
		{
			axis = (i < mainAxes->axesCount)? mainAxes->axes[i]: extruderAxes->axes[i-mainAxes->axesCount];
			jerk = fabs(current_speed[axis] - m_Previous_Speed[axis]);
			if (jerk > g_Max_jerk[axis]) vmax_junction_factor = min(g_Max_jerk[axis] / jerk, vmax_junction_factor);
		}

		vmax_junction_Min = block->nominal_speed;
		vmax_junction_Min = min(this->m_Previous_Nominal_Speed, vmax_junction_Min * vmax_junction_factor); // Limit speed to max previous speed
	}
	block->max_entry_speed = vmax_junction_Min;

	// Initialize block entry speed. Compute based on deceleration to user-defined MINIMUM_PLANNER_SPEED.
	v_allowable = Cal_Max_Allowable_Speed(-block->acceleration, MINIMUM_PLANNER_SPEED, block->millimeters);
	block->entry_speed = min(vmax_junction_Min, v_allowable);

	// Initialize planner efficiency flags
	// Set flag if block will always reach maximum junction speed regardless of entry/exit speeds.
	// If a block can de/ac-celerate from nominal speed to zero within the length of the block, then
	// the current block and next block junction speeds are guaranteed to always be at their maximum
	// junction speeds in deceleration and acceleration, respectively. This is due to how the current
	// block nominal speed limits both the current and next maximum junction speeds. Hence, in both
	// the reverse and forward planners, the corresponding block junction speed will always be at the
	// the maximum junction speed and may always be ignored for any speed reduction checks.
	block->nominal_length_flag = (block->nominal_speed <= v_allowable);
	block->recalculate_flag = True; // Always calculate trapezoid for new block
	for (i = 0; i < totalAxesCount; i++)
	{
		axis = (i < mainAxes->axesCount)? mainAxes->axes[i]: extruderAxes->axes[i-mainAxes->axesCount];
		// Update previous path unit_vector and nominal speed
		m_Previous_Speed[axis] = current_speed[axis];
		// Update position
		m_CurrentPosition[axis] = target[axis];
	}
	// Update nominal speed
	this->m_Previous_Nominal_Speed = block->nominal_speed;

	Calculate_Trapezoid_for_Block(block, block->entry_speed / block->nominal_speed, safe_speed / block->nominal_speed);

	// Move buffer head
	blockMnger->m_Block_Buffer_Head = next_buffer_head;

	Planner_Recalculate(this);
} // Planner_Buffer_Line()

/*******************************************************************
功能：设置现在位置
作成日期：2016/7/7
参数:
	float *position		XYZE轴现在位置
	bool isAllSet		是否更改实际位置
返回值：无
********************************************************************/
void Planner_Set_Position(PLANNER *this, POSPARAM *posParam)
{
	int i;
	enum _AXIS axis;

    for (i = 0; i < posParam->axesCount; i++)
	{
		axis = posParam->axes[i];
		m_Previous_Speed[axis] = 0.0;
		m_CurrentPosition_Float[axis] = posParam->position[i];
		m_CurrentPosition[axis] = lround(posParam->position[i] * g_Axis_Steps_per_Unit[axis]);
	}
    this->m_Previous_Nominal_Speed = 0.0; // Resets planner junction speeds. Assumes start from rest.
}
/*******************************************************************
功能：获取轴现在位置
作成日期：2016/7/7
参数:
	enum _AXIS axis
返回值：无
********************************************************************/
float Planner_Get_AxisPosition(enum _AXIS axis)
{
	return m_CurrentPosition_Float[axis];
}
/*******************************************************************
功能：设置E轴现在位置
作成日期：2016/7/7
参数:
	float e				E轴现在位置
返回值：无
********************************************************************/
// void Planner_Set_E_Position(float e, u8 E_Number, bool isAllSet) {
	// m_CurrentPosition[E_AXIS+E_Number] = lround(e * g_Axis_Steps_per_Unit[E_AXIS+E_Number]);
	// if(isAllSet != False)
	// {
		// Stepper_Set_E_Position(m_CurrentPosition[E_AXIS+E_Number],E_Number);
	// }
// }

/*******************************************************************
功能：各轴加速度上限单位转换 mm/s^2→step/s^2
作成日期：2016/7/7
参数:无
返回值：无
********************************************************************/
void Planner_Reset_Acceleration_Rates(void) {
	int i;
	for (i = 0; i < AXIS_MAX; i++)
	{
		g_Axis_Steps_per_Sqr_Second[i] = g_Max_Acceleration_Units_per_Sqr_Second[i] * g_Axis_Steps_per_Unit[i];
	}
}
