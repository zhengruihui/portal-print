/********************** (C) COPYRIGHT 2015 DediBot ******************************
  * @file    AxisCtrl.c
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-03-25
  * @brief   电机控制
*********************************************************************************/
/*******************************************
	包含关系
********************************************/
#include "AxisCtrl.h"			//电机控制
#include "BrakeCtrl.h"
#define DRIVER_EN				(LOW)
#define DRIVER_DIS				(HIGH)
#define DRIVER_FWD				(LOW)
#define DRIVER_REV				(HIGH)
#define DRIVER_MOVE				(HIGH)
#define DRIVER_STOP				(LOW)

#define Driver_Enable(driver)	HAL_GPIO_WritePin(m_Drivers[driver].en_setting.port, m_Drivers[driver].en_setting.pin, (GPIO_PinState)DRIVER_EN)
#define Driver_Disable(driver)	HAL_GPIO_WritePin(m_Drivers[driver].en_setting.port, m_Drivers[driver].en_setting.pin, (GPIO_PinState)DRIVER_DIS)
#define Driver_SetFwd(driver)	HAL_GPIO_WritePin(m_Drivers[driver].dir_setting.port, m_Drivers[driver].dir_setting.pin, (GPIO_PinState)DRIVER_FWD)
#define Driver_SetRev(driver)	HAL_GPIO_WritePin(m_Drivers[driver].dir_setting.port, m_Drivers[driver].dir_setting.pin, (GPIO_PinState)DRIVER_REV)
#define Driver_Move(driver)		HAL_GPIO_WritePin(m_Drivers[driver].motion_setting.port, m_Drivers[driver].motion_setting.pin, (GPIO_PinState)DRIVER_MOVE)
#define Driver_Stop(driver)		HAL_GPIO_WritePin(m_Drivers[driver].motion_setting.port, m_Drivers[driver].motion_setting.pin, (GPIO_PinState)DRIVER_STOP)


#define SIGNAL_REVERSE			(True)
#define SIGNAL_NOTREVERSE		(False)


typedef struct {
	GPIO_TypeDef *port;
	u16	pin;
	// u8	level_P;
	// u8	level_N;
}PORT_PIN;

typedef struct {
	PORT_PIN		motion_setting;
	PORT_PIN		dir_setting;
	PORT_PIN		en_setting;
}DRIVER_PORT;

typedef struct
{
	enum _DRIVER driver;
	u8	isDirReverse;
}AXIS_STRU;


/*******************************************
	本地变量
********************************************/

const AXIS_STRU m_Axis[AXIS_MAX] =
{
					//	对应的轴定义	移动方向设置
/* X1_AXIS */		{	DRIVER_MAX,		SIGNAL_REVERSE		},	
/* Y1_AXIS */		{	DRIVER_MAX,		SIGNAL_NOTREVERSE	},
/* Z1_AXIS */		{	DRIVER_MAX,		SIGNAL_NOTREVERSE	},
/* E1_AXIS */		{	DRIVER_MAX,		SIGNAL_NOTREVERSE	},

/* X2_AXIS */		{	STEPPER8,		SIGNAL_NOTREVERSE	},
/* Y2_AXIS */		{	STEPPER9,		SIGNAL_REVERSE	},
/* Z2_AXIS */		{	STEPPER10,		SIGNAL_REVERSE	},
/* E2_AXIS_1 */		{	STEPPER_M1,		SIGNAL_NOTREVERSE	},
/* E2_AXIS_2 */		{	STEPPER_M2,		SIGNAL_NOTREVERSE	},
/* E2_AXIS_3 */		{	STEPPER_M3,		SIGNAL_NOTREVERSE	},
/* E2_AXIS_4 */		{	STEPPER_M4,		SIGNAL_NOTREVERSE	},


/* X3_AXIS */		{	DRIVER_MAX,		SIGNAL_REVERSE		},
/* Y3_AXIS */		{	DRIVER_MAX,		SIGNAL_REVERSE	    },
/* Z3_AXIS */		{	DRIVER_MAX,		SIGNAL_NOTREVERSE	},
/* E3_AXIS */		{	DRIVER_MAX,		SIGNAL_NOTREVERSE	},


/* W_AXIS */		{	STEPPER7,		SIGNAL_REVERSE	}
};

const DRIVER_PORT m_Drivers[DRIVER_MAX] =
{ 
//	// 轴名称		Port								pin
/* STEPPER_M1 */	{
					{GET_PORT(STEPPER_M1_STEP_PIN),		GET_PIN(STEPPER_M1_STEP_PIN)	},
					{GET_PORT(STEPPER_M1_DIR_PIN),		GET_PIN(STEPPER_M1_DIR_PIN)		},
					{GET_PORT(STEPPER_M1_EN_PIN),		GET_PIN(STEPPER_M1_EN_PIN)		}
					},
/* STEPPER_M2 */	{
					{GET_PORT(STEPPER_M2_STEP_PIN),		GET_PIN(STEPPER_M2_STEP_PIN)	},
					{GET_PORT(STEPPER_M2_DIR_PIN),		GET_PIN(STEPPER_M2_DIR_PIN)		},
					{GET_PORT(STEPPER_M2_EN_PIN),		GET_PIN(STEPPER_M2_EN_PIN)		}
					},
/* STEPPER_M3 */	{
					{GET_PORT(STEPPER_M3_STEP_PIN),		GET_PIN(STEPPER_M3_STEP_PIN)	},
					{GET_PORT(STEPPER_M3_DIR_PIN),		GET_PIN(STEPPER_M3_DIR_PIN)		},
					{GET_PORT(STEPPER_M3_EN_PIN),		GET_PIN(STEPPER_M3_EN_PIN)		}
					},
/* STEPPER_M4 */	{
					{GET_PORT(STEPPER_M4_STEP_PIN),	    GET_PIN(STEPPER_M4_STEP_PIN)	},
					{GET_PORT(STEPPER_M4_DIR_PIN),		GET_PIN(STEPPER_M4_DIR_PIN)		},
					{GET_PORT(STEPPER_M4_EN_PIN),		GET_PIN(STEPPER_M4_EN_PIN)		}
					},
/* STEPPER_M5 */	{
					{GET_PORT(STEPPER_M5_STEP_PIN),				GET_PIN(STEPPER_M5_STEP_PIN) },
					{GET_PORT(STEPPER_M5_DIR_PIN),		GET_PIN(STEPPER_M5_DIR_PIN)		},
					{GET_PORT(STEPPER_M5_EN_PIN),		GET_PIN(STEPPER_M5_EN_PIN)		}
					},
/* STEPPER_M6 */	{
					{GET_PORT(STEPPER_M6_STEP_PIN),		GET_PIN(STEPPER_M6_STEP_PIN)	},
					{GET_PORT(STEPPER_M6_DIR_PIN),		GET_PIN(STEPPER_M6_DIR_PIN)		},
					{GET_PORT(STEPPER_M6_EN_PIN),		GET_PIN(STEPPER_M6_EN_PIN)		}
					},
/* STEPPER7 */		{ 
					{GET_PORT(STEPPER7_STEP_PIN),		GET_PIN(STEPPER7_STEP_PIN)		},
					{GET_PORT(STEPPER7_DIR_PIN),		GET_PIN(STEPPER7_DIR_PIN)		},
					{GET_PORT(STEPPER7_EN_PIN),			GET_PIN(STEPPER7_EN_PIN)		}
					},
/* STEPPER8 */		{
					{GET_PORT(STEPPER8_STEP_PIN),		GET_PIN(STEPPER8_STEP_PIN)		},
					{GET_PORT(STEPPER8_DIR_PIN),		GET_PIN(STEPPER8_DIR_PIN)		},
					{GET_PORT(STEPPER8_EN_PIN),			GET_PIN(STEPPER8_EN_PIN)		}
					},
/* STEPPER9 */		{
					{GET_PORT(STEPPER9_STEP_PIN),		GET_PIN(STEPPER9_STEP_PIN)		},
					{GET_PORT(STEPPER9_DIR_PIN),		GET_PIN(STEPPER9_DIR_PIN)		},
					{GET_PORT(STEPPER9_EN_PIN),			GET_PIN(STEPPER9_EN_PIN)		}
					},
/* STEPPER10 */		{
					{GET_PORT(STEPPER10_STEP_PIN),		GET_PIN(STEPPER10_STEP_PIN)		},
					{GET_PORT(STEPPER10_DIR_PIN),		GET_PIN(STEPPER10_DIR_PIN)		},
					{GET_PORT(STEPPER10_EN_PIN),		GET_PIN(STEPPER10_EN_PIN)		}
					},
/* STEPPER11 */		{ 
					{GET_PORT(STEPPER11_STEP_PIN),		GET_PIN(STEPPER11_STEP_PIN)		},
					{GET_PORT(STEPPER11_DIR_PIN),		GET_PIN(STEPPER11_DIR_PIN)		},
					{GET_PORT(STEPPER11_EN_PIN),		GET_PIN(STEPPER11_EN_PIN)		}
					},
/* STEPPER12 */		{
					{GET_PORT(STEPPER12_STEP_PIN),		GET_PIN(STEPPER12_STEP_PIN)		},
					{GET_PORT(STEPPER12_DIR_PIN),		GET_PIN(STEPPER12_DIR_PIN)		},
					{GET_PORT(STEPPER12_EN_PIN),		GET_PIN(STEPPER12_EN_PIN)		}
					},
/* STEPPER13 */		{
					{GET_PORT(STEPPER13_STEP_PIN),		GET_PIN(STEPPER13_STEP_PIN)		},
					{GET_PORT(STEPPER13_DIR_PIN),		GET_PIN(STEPPER13_DIR_PIN)		},
					{GET_PORT(STEPPER13_EN_PIN),		GET_PIN(STEPPER13_EN_PIN)		}
					},
/* STEPPER14 */		{
					{GET_PORT(STEPPER14_STEP_PIN),		GET_PIN(STEPPER14_STEP_PIN)		},
					{GET_PORT(STEPPER14_DIR_PIN),		GET_PIN(STEPPER14_DIR_PIN)		},
					{GET_PORT(STEPPER14_EN_PIN),		GET_PIN(STEPPER14_EN_PIN)		}
					},
/* STEPPER15 */		{
					{GET_PORT(STEPPER14_STEP_PIN),		GET_PIN(STEPPER14_STEP_PIN)		},
					{GET_PORT(STEPPER14_DIR_PIN),		GET_PIN(STEPPER14_DIR_PIN)		},
					{GET_PORT(STEPPER14_EN_PIN),		GET_PIN(STEPPER14_EN_PIN)		}
					},

};


/*******************************************
	外部参考变量
********************************************/
/*******************************************
	内部函数声明
********************************************/



/*******************************************
	内部函数
********************************************/


/*******************************************
		外部函数
********************************************/


void AxisCtrl_Enable(enum _AXIS axis)
{
	if(axis == Z2_AXIS)
	{
		BrakeCtrl_OFF();
	}
	Driver_Enable(m_Axis[axis].driver);
}

void AxisCtrl_Disable(enum _AXIS axis)
{
	Driver_Disable(m_Axis[axis].driver);
}

void AxisCtrl_SetDirFront(enum _AXIS axis)
{
	if(m_Axis[axis].isDirReverse == SIGNAL_NOTREVERSE)
	{
		Driver_SetFwd(m_Axis[axis].driver);
	}
	else
	{
		Driver_SetRev(m_Axis[axis].driver);
	}
}
void AxisCtrl_SetDirBack(enum _AXIS axis)
{
	if(m_Axis[axis].isDirReverse == SIGNAL_NOTREVERSE)
	{
		Driver_SetRev(m_Axis[axis].driver);
	}
	else
	{
		Driver_SetFwd(m_Axis[axis].driver);
	}
}
void AxisCtrl_Move(enum _AXIS axis)
{
		Driver_Move(m_Axis[axis].driver);
}
void AxisCtrl_Stop(enum _AXIS axis)
{
		Driver_Stop(m_Axis[axis].driver);
}

/*******************************************
功能：所有轴停止
作成者：liu-cy
作成日期：2017/3/2
参数:   无
返回值: 无
********************************************/
void AxisCtrl_All_Stop( void )
{
	u8 i;
	for(i = 0; i < DRIVER_MAX; i++)
	{
		Driver_Disable((enum _DRIVER)i);
	}
}

/*********************************************
功能：初始化
作成者：cao-dl
作成日期：2014/11/19
参数：无
返回值：无
*********************************************/
void AxisCtrl_Init( void )
{
	AxisCtrl_All_Stop();
}
