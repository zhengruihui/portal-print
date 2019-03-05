/********************** (C) COPYRIGHT 2015 DediBot ******************************
  * @file    DataParse.c
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-03-28
  * @brief   异常管理
*********************************************************************************/
/*******************************************
	包含关系
********************************************/
#include "DataParse.h"						//打印数据分析
#include "TempCtrl.h"
#include "stdlib.h"

/*******************************************
	本地全部变量
********************************************/

/*******************************************
    本地全局变量
********************************************/

/*******************************************
	外部参考变量
********************************************/

/*******************************************
	外部参考变量
********************************************/

/*******************************************
	内部函数声明
********************************************/
/*******************************************
	外部函数声明
********************************************/
/******************************************
功能：暂停打印,各轴回到原点向反方向
作成者：liu-cy
作成日期：2017/1/6
参数：无
返回值：无
*******************************************/
void DataParse_M0( PORTALFRAME *this )
{
	Print_Finish(this);
}

/******************************************
功能：恢复打印
作成者：zheng-rh
作成日期：2017/12/25
参数：无
返回值：无
*******************************************/
void DataParse_M24( PORTALFRAME *this )
{
	Print_Continue(this);
}

/******************************************
功能：暂停打印,各轴回到原点向反方向
作成者：liu-cy
作成日期：2017/1/6
参数：无
返回值：无
*******************************************/
void DataParse_M25( PORTALFRAME *this )
{
	//Print_Pause(this);
	Print_Finish(this);
}

/******************************************
功能：M84
	设置步进电机定时关闭时间
作成者：cao-dl
作成日期：2014/12/23
参数：
	value：设定时间
返回值：无
*******************************************/
void DataParse_M84( PORTALFRAME *this, float value )
{
	int i;
	
	Stepper_QuickStop(this->positionMng.mainMoveModule);
	Stepper_QuickStop(this->positionMng.freeMoveModule);
	
	for(i = 0; i < this->positionMng.totalAxesCount; i++)
	{
		AxisCtrl_Disable(this->positionMng.axes[i]);
	}
	
	//延时关闭
	// if( value > 0.0f )
	// {
		// m_Stepper_Inactive_Time = value * 1000;
	// }
	// //全轴禁止
	// else
	// {
		// AxisCtrl_All_Stop();
	// }
}
/******************************************
功能：
	设置挤出头温度
作成者：cao-dl
作成日期：2014/12/23
参数：
返回值：无
*******************************************/
void DataParse_SetExtruderTemp( PORTALFRAME *this, char *gcode, bool isNeedWait )
{
	char *endptr;
	int cmpIdx, schIdx, schIdx2;
	int len = strlen(gcode);
	int size = len+1;
	char cmpStr[10];
	
	for(schIdx = 0; schIdx < len; schIdx++)
	{
		if(gcode[schIdx] == 'S')
		{
			cmpStr[0] = gcode[schIdx];
			schIdx++;
			for(schIdx2 = schIdx; schIdx2 < size; schIdx2++)
			{
				if(gcode[schIdx2] == '_')
				{
					memcpy(&cmpStr[1], &gcode[schIdx], schIdx2-schIdx);
					cmpStr[schIdx2-schIdx+1] = '\0';
					schIdx = schIdx2+1;
					break;
				}
				else if((gcode[schIdx2] < '0') || (gcode[schIdx2] > '9'))
				{
					cmpStr[1] = '\0';
					break;
				}
			}
			for(cmpIdx = 0; cmpIdx < this->extruderCount; cmpIdx++)
			{
				if(0 == strcmp(cmpStr, this->extruderCodes[cmpIdx]))
				{
					TempCtrl_setTargetTemp(this->extruder_Stru[cmpIdx].heatItem, (float)strtod(&gcode[schIdx],&endptr), isNeedWait);
					if(False != isNeedWait)
					{
						EventMng_SetEvent_Normal(PRINTER_EVENT_SETWAITTEMP, NULL);
						this->printInfo.stage = PRINT_STAGE_WAITTEMP;
					}
					// break;
				}
			}
		}
	}
}
/*******************************************************************
功能：要检查的字符是否在目标字符串里，有：True，无：False
作成者：cao-dl
作成日期：2014/11/19
参数:
	code:要检查的字符
返回值：
	True：目标字符找到
	False：目标字符未找到
********************************************************************/
bool DataParse_code_seen_chr( char *gcode, const char code, float *data )
{
	bool	flag;
	char*   strchr_pointer;
	strchr_pointer = strchr(gcode, code);
	flag = (bool)(strchr_pointer != NULL);
	if( flag == False )
	{
		*data = 0.0;
	}
	else
	{
		*data = strtod(&gcode[strchr_pointer - gcode + 1], NULL);
	}
	return flag;
}
/*******************************************************************
功能：要检查的字符是否在目标字符串里，有：True，无：False
作成者：cao-dl
作成日期：2014/11/19
参数:
	code:要检查的字符
返回值：
	True：目标字符找到
	False：目标字符未找到
********************************************************************/
bool DataParse_code_seen_str( char *gcode, const char* code, float *data )
{
	bool	flag;
	char*   strchr_pointer;
	strchr_pointer = strstr(gcode, code);
	flag = (bool)(strchr_pointer != NULL);
	if( flag == False )
	{
		*data = 0.0;
		//return False;
	}
	else
	{
		*data = strtod(&gcode[strchr_pointer - gcode + strlen(code)], NULL);
		//return True;
	}
	return flag;
}
/*******************************************************************
功能：分割去除字符串中code之后的数据
作成者：deng-xy
作成日期：2016/5/19
参数:
	gcode：元数据
	code:分割符号

********************************************************************/
void DataParse_code_tok_chr( char *gcode, char *code )
{
	if(*gcode == code[0])
	{
		*gcode = '\0';
	}
	else
	{
		gcode = strtok( gcode, code );
	}
}

/*******************************************************************
功能：受信数据解析
作成者：cao-dl
作成日期：2014/11/19
参数：
返回值：无
********************************************************************/
bool DataParse_G_M_Ctrl( PORTALFRAME *this, char* gcode )
{
	float	value;
	bool	Status;
	
	Status = False;
	DataParse_code_tok_chr(gcode,";");
	
	//命令行里，有“G”的情况下
	if( DataParse_code_seen_chr(gcode, 'G' , &value ) )
	{
		//‘G’后面的数字
		switch((int)value)
		{
			//快速移动
			case 0: // G0 -> G1
			//可控移动
			case 1: // G1
				//目标位置移动
				PositionMng_PrepareMove_Gcode(&this->positionMng, gcode);
				break;
			case 27: //G28 Home XY Axis one at a time
				PositionMng_MoveHome_XY(&this->positionMng);
			  break;
			case 28: //G28 Home all Axis one at a time
				PositionMng_MoveHome(&this->positionMng);
				break;
			//设置位置
			case 92: // G92
				PositionMng_Set_Position_Gcode( &this->positionMng, gcode, False );
				break;
			default:
				break;
		}
	}
	//命令行里有‘M’时
	else if(DataParse_code_seen_chr(gcode, 'M', &value))
	{
		switch( (int)value )
		{
			case 104: // M104
				DataParse_SetExtruderTemp(this, gcode, False);
				break;
			//设置挤出机温度，并等待
			case 109:
				DataParse_SetExtruderTemp(this, gcode, True);
				break;
			case 140: // M140
				break;
			//设置挤出机温度，并等待
			case 190:
				break;				
			//设置步进电机关闭时间
			case 84:
				DataParse_code_seen_chr(gcode, 'S', &value);
				DataParse_M84(this, value );
				break;
			case 0:		//停止打印
				DataParse_M0(this);
				EventMng_SetEvent_Level(PRINTER_EVENT_STANDBY, NULL ,EVENT_LEVEL_0);
				break;
			case 24:	//恢复SD卡打印
				DataParse_M24(this);
				break;
			case 25:	//暂停SD卡打印
				DataParse_M25(this);
				break;
			case 102:
				//取得打印总行数
				if( DataParse_code_seen_chr(gcode, 'L', &value ) )
				{
				}
				break;
			case 201:															//设置轴的最大加速度
				break;
			case 203:															//设置轴的最大速度
				break;
			case 204:															//设置加速度
				break;
			case 205:															//高级设置
				break;
			case 106:
				if(DataParse_code_seen_chr(gcode, 'S', &value))
				{
					FanCtrl_SetFan((_FAN)9, (uint8_t)value);
				}
			break;
				
			case 110:
				if(DataParse_code_seen_chr(gcode, 'N', &value))
				{
						this->total_gcode_line = value;
						this->current_gcode_line = 0;
				}
				break;	
			default:
				break;
		}
	}
	return (Status);
}


