/********************** (C) COPYRIGHT 2015 DediBot ******************************
  * @file    Input.c
  * @author  Dedibot
  * @version V1.2.0
  * @date    2016-03-28
  * @brief   IO����
*********************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef INPUT_H
#define INPUT_H
#include "stm32f429xx.h"
#include "de_type.h"

/* �źŶ��� */
enum _INPUT{
//	INPUT_KEY1,				//����
//	INPUT_KEY2,				//����
	INPUT_LIMIT1,			//��λ����
	INPUT_LIMIT2,			//��λ����
	INPUT_LIMIT3,			//��λ����
	INPUT_LIMIT4,			//��λ����
	INPUT_LIMIT5,			//��λ����
	INPUT_LIMIT6,			//��λ����
	INPUT_LIMIT7,			//��λ����
	INPUT_LIMIT8,			//��λ����
	INPUT_LIMIT9,			//��λ����
	INPUT_LIMIT10,			//��λ����
	INPUT_LIMIT11,			//��λ����
	INPUT_LIMIT12,			//��λ����
	INPUT_LIMIT13,			//��λ����
	INPUT_LIMIT14,			//��λ����
	INPUT_LIMIT15,			//��λ����
	INPUT_LIMIT16,			//��λ����
	
	INPUT_MATERIAL1,		//�������ź�
	INPUT_MATERIAL2,		//�������ź�
	INPUT_MATERIAL3,		//�������ź�
	INPUT_MATERIAL4,		//�������ź�
	INPUT_MATERIAL5,		//�������ź�
	INPUT_MATERIAL6,		//�������ź�

//	INPUT_MATERIAL_BLOCK1,	//�����ź�
//	INPUT_MATERIAL_BLOCK2,	//�����ź�
	
	INPUT_SD,				//SD�������źż���������ţ��͵�ƽ��ʾ����SD�����ߵ�ƽ��ʾSD������

	INPUT_MAX
};
/*******************************************
	����ӿ�
********************************************/
//IO��ʼ��
extern void Input_Init(void);
//�����˿�״̬����
extern void Input_Watch(void);
//�����˿�״̬ȡ��
// �з�������
extern u8 Input_GetState( enum _INPUT);
//�޷�������
extern u8 Input_GetDirectState( enum _INPUT);

#endif //INPUT_H
