/********************** (C) COPYRIGHT 2015 DediBot ******************************
  * @file    ErrorMng.h
  * @author  Dedibot
  * @version V1.2.0
  * @date    2017-05-25
  * @brief   �쳣����
*********************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ERRORMNG_H
#define ERRORMNG_H
#include "de_type.h"
/* �쳣���� */
typedef enum
{
	ERROR_COM,			//ͨ���쳣
//	ERROR_BREAK,		//�����ж�
	ERROR_COMTIMOUT,	//ͨ�ų�ʱ
//	ERROR_NOPRINTDATA,	//��ʱ��û�д�ӡ����
	ERROR_TEMOVERTOP1,	//��ͷ1�¶ȹ���
	ERROR_TEMOVERTOP2,	//��ͷ2�¶ȹ���
	ERROR_TEMOVERTOPBAD,//�ȴ��¶ȹ���
	ERROR_TEMTIMEOUT1,	//��ͷ1���ȳ�ʱ
	ERROR_TEMTIMEOUT2,	//��ͷ2���ȳ�ʱ	
	ERROR_TEMTIMEOUTBAD,//�ȴ����ȳ�ʱ	
	ERROR_TEMHEAD1,		//����1ͷ�쳣
	ERROR_TEMHEAD2,		//����2ͷ�쳣
	ERROR_TEMBADHEAD,	//�ȴ��쳣
	ERROR_TEMSEN1,		//��ͷ������1�쳣
	ERROR_TEMSEN2,		//��ͷ������2�쳣
	ERROR_TEMSENBAD,	//�ȴ��������쳣
	ERROR_STARVING1,	//ȱ���쳣
	ERROR_STARVING2,	//ȱ���쳣	
//	ERROR_PUTTY,		//����
	ERROR_LIMIT_X,		//��λ�����쳣X
	ERROR_LIMIT_Y,		//��λ�����쳣Y
	ERROR_LIMIT_Z1,		//��λ�����쳣Z1
	ERROR_LIMIT_Z2,		//��λ�����쳣Z2
	ERROR_POSITION_OUT,	//λ�ó���
	ERROR_MOTOR_DRIVER1,//�������쳣X
	ERROR_MOTOR_DRIVER2,//�������쳣Y
	ERROR_MAX
}ERROR_NAME;

/* �쳣�ȼ����� */
typedef enum
{
	ERROE_LEVEL_0,	//ֹͣ��ӡ���������
	ERROE_LEVEL_1,	//��ͣ��ӡ���������
	ERROE_LEVEL_2,	//��ͣ��ӡ���Զ����
	ERROE_LEVEL_3,	//�Զ����
	ERROE_LEVEL_4,
	ERROE_LEVEL_5,
	ERROE_LEVEL_NORMAL = ERROE_LEVEL_5,
	
	ERROR_LEVEL_MAX
}ERROR_LEVEL;

/* �쳣���Խṹ���� */
typedef struct
{
	// ERROR_NAME	name;
	u32			code;
	ERROR_LEVEL	defaultLevel;
}ERROR_PROPERTY;

// typedef struct 
// {
	// ERROR_NAME	name;
	// ERROR_LEVEL	level;
// }ERROR;
extern void ErrorMng_SetError(ERROR_NAME errName);
extern void ErrorMng_ResetAllError(void);
extern bool ErrorMng_IsError(void);
extern bool IsErrorFlag(ERROR_NAME err);
extern void ErrorMng_ResetError(ERROR_NAME errName);
extern bool ErrorMng_IsErrorLevel(ERROR_LEVEL level);
extern ERROR_LEVEL Get_HighestLevel(void);
extern void ErrorMng_Init(void);
extern u8* GetError(void);
extern bool Get_HighestLevelError( ERROR_NAME *outErr );
#endif  //ERRORMNG_H








