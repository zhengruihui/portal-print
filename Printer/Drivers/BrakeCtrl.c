#include "BrakeCtrl.h"
#include "main.h"

/*******************************************
���ܣ���ɲ��
�����ߣ�
�������ڣ�
����:
����ֵ:��
********************************************/
void BrakeCtrl_ON(void)
{
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_0, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);
}

/*******************************************
���ܣ��ر�ɲ��
�����ߣ�
�������ڣ�
����:
����ֵ:��
********************************************/
void BrakeCtrl_OFF(void)
{
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_0, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
}
