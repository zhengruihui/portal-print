/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
     PD0   ------> FMC_D2_DA2
     PD1   ------> FMC_D3_DA3
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, STEPPER8_DIR_PIN_Pin|STEPPER8_STEP_PIN_Pin|STEPPER8_EN_PIN_Pin|STEPPER7_DIR_PIN_Pin 
                          |STEPPER7_STEP_PIN_Pin|FAN9_PIN_Pin|FAN8_PIN_Pin|FAN7_PIN_Pin 
                          |FAN6_PIN_Pin|FAN5_PIN_Pin|FAN4_PIN_Pin|FAN3_PIN_Pin 
                          |FAN2_PIN_Pin|FAN1_PIN_Pin|STEPPER10_DIR_PIN_Pin|STEPPER10_STEP_PIN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOI, STEPPER7_EN_PIN_Pin|STEPPER_M6_STEP_PIN_Pin|STEPPER_M6_EN_PIN_Pin|STEPPER_M5_DIR_PIN_Pin 
                          |STEPPER_M4_STEP_PIN_Pin|STEPPER_M4_EN_PIN_Pin|STEPPER_M3_DIR_PIN_Pin|BEEP1_PIN_Pin 
                          |STEPPER10_EN_PIN_Pin|STEPPER9_DIR_PIN_Pin|STEPPER9_STEP_PIN_Pin|STEPPER9_EN_PIN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, STEPPER_M6_DIR_PIN_Pin|LIGHT3_PIN_Pin|LIGHT2_PIN_Pin|EEPROM_SCL_Pin 
                          |EEPROM_SDA_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, STEPPER_M5_STEP_PIN_Pin|STEPPER_M5_EN_PIN_Pin|STEPPER_M4_DIR_PIN_Pin|STEPPER_M3_STEP_PIN_Pin 
                          |STEPPER_M3_EN_PIN_Pin|STEPPER_M2_DIR_PIN_Pin|STEPPER_M2_STEP_PIN_Pin|STEPPER_M2_EN_PIN_Pin 
                          |STEPPER_M1_DIR_PIN_Pin|STEPPER_M1_STEP_PIN_Pin|STEPPER_M1_EN_PIN_Pin|HEATER4_PIN_Pin 
                          |HEATER3_PIN_Pin|HEATER2_PIN_Pin|HEATER1_PIN_Pin|FAN12_PIN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOH, TEMP_HUMIDITY_SIGNAL1_PIN_Pin|TEMP_HUMIDITY_SIGNAL2_PIN_Pin|TEMP_HUMIDITY_SIGNAL3_PIN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LIGHT4_PIN_GPIO_Port, LIGHT4_PIN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LIGHT1_PIN_Pin|USB_EN_PIN_Pin|STEPPER12_STEP_PIN_Pin|STEPPER12_EN_PIN_Pin 
                          |STEPPER11_DIR_PIN_Pin|STEPPER11_STEP_PIN_Pin|STEPPER11_EN_PIN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOJ, LED_Y_PIN_Pin|LED_R_PIN_Pin|LED_G_PIN_Pin|HEATER6_PIN_Pin 
                          |HEATER5_PIN_Pin|STEPPER15_DIR_PIN_Pin|STEPPER15_STEP_PIN_Pin|STEPPER15_EN_PIN_Pin 
                          |STEPPER14_DIR_PIN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, FAN11_PIN_Pin|FAN10_PIN_Pin|ETHERNET_RESET_PIN_Pin|STEPPER12_DIR_PIN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(WIFI_POWER_CONTROL_PIN_GPIO_Port, WIFI_POWER_CONTROL_PIN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ETHERNET_POWER_CONTROL_PIN_GPIO_Port, ETHERNET_POWER_CONTROL_PIN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOK, STEPPER14_STEP_PIN_Pin|STEPPER14_EN_PIN_Pin|STEPPER13_DIR_PIN_Pin|STEPPER13_STEP_PIN_Pin 
                          |STEPPER13_EN_PIN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PEPin PEPin PEPin PEPin 
                           PEPin PEPin PEPin PEPin 
                           PEPin PEPin PEPin PEPin 
                           PEPin PEPin PEPin PEPin */
  GPIO_InitStruct.Pin = STEPPER8_DIR_PIN_Pin|STEPPER8_STEP_PIN_Pin|STEPPER8_EN_PIN_Pin|STEPPER7_DIR_PIN_Pin 
                          |STEPPER7_STEP_PIN_Pin|FAN9_PIN_Pin|FAN8_PIN_Pin|FAN7_PIN_Pin 
                          |FAN6_PIN_Pin|FAN5_PIN_Pin|FAN4_PIN_Pin|FAN3_PIN_Pin 
                          |FAN2_PIN_Pin|FAN1_PIN_Pin|STEPPER10_DIR_PIN_Pin|STEPPER10_STEP_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PIPin PIPin PIPin PIPin 
                           PIPin PIPin PIPin PIPin 
                           PIPin PIPin PIPin PIPin */
  GPIO_InitStruct.Pin = STEPPER7_EN_PIN_Pin|STEPPER_M6_STEP_PIN_Pin|STEPPER_M6_EN_PIN_Pin|STEPPER_M5_DIR_PIN_Pin 
                          |STEPPER_M4_STEP_PIN_Pin|STEPPER_M4_EN_PIN_Pin|STEPPER_M3_DIR_PIN_Pin|BEEP1_PIN_Pin 
                          |STEPPER10_EN_PIN_Pin|STEPPER9_DIR_PIN_Pin|STEPPER9_STEP_PIN_Pin|STEPPER9_EN_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin PCPin 
                           PCPin */
  GPIO_InitStruct.Pin = STEPPER_M6_DIR_PIN_Pin|LIGHT3_PIN_Pin|LIGHT2_PIN_Pin|EEPROM_SCL_Pin 
                          |EEPROM_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PFPin PFPin PFPin PFPin 
                           PFPin PFPin PFPin PFPin 
                           PFPin PFPin PFPin PFPin 
                           PFPin PFPin PFPin PFPin */
  GPIO_InitStruct.Pin = STEPPER_M5_STEP_PIN_Pin|STEPPER_M5_EN_PIN_Pin|STEPPER_M4_DIR_PIN_Pin|STEPPER_M3_STEP_PIN_Pin 
                          |STEPPER_M3_EN_PIN_Pin|STEPPER_M2_DIR_PIN_Pin|STEPPER_M2_STEP_PIN_Pin|STEPPER_M2_EN_PIN_Pin 
                          |STEPPER_M1_DIR_PIN_Pin|STEPPER_M1_STEP_PIN_Pin|STEPPER_M1_EN_PIN_Pin|HEATER4_PIN_Pin 
                          |HEATER3_PIN_Pin|HEATER2_PIN_Pin|HEATER1_PIN_Pin|FAN12_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : PHPin PHPin PHPin */
  GPIO_InitStruct.Pin = TEMP_HUMIDITY_SIGNAL1_PIN_Pin|TEMP_HUMIDITY_SIGNAL2_PIN_Pin|TEMP_HUMIDITY_SIGNAL3_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin */
  GPIO_InitStruct.Pin = LIGHT4_PIN_Pin|WIFI_POWER_CONTROL_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin PBPin 
                           PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = LIGHT1_PIN_Pin|USB_EN_PIN_Pin|STEPPER12_STEP_PIN_Pin|STEPPER12_EN_PIN_Pin 
                          |STEPPER11_DIR_PIN_Pin|STEPPER11_STEP_PIN_Pin|STEPPER11_EN_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin PBPin 
                           PBPin */
  GPIO_InitStruct.Pin = BATTERY_CONTROL_PIN_Pin|OUTAGE_DETECT_PIN_Pin|FAN12_FEEDBACK_PIN_Pin|FAN11_FEEDBACK_PIN_Pin 
                          |FAN2_FEEDBACK_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PJPin PJPin PJPin PJPin 
                           PJPin PJPin PJPin PJPin 
                           PJPin */
  GPIO_InitStruct.Pin = LED_Y_PIN_Pin|LED_R_PIN_Pin|LED_G_PIN_Pin|HEATER6_PIN_Pin 
                          |HEATER5_PIN_Pin|STEPPER15_DIR_PIN_Pin|STEPPER15_STEP_PIN_Pin|STEPPER15_EN_PIN_Pin 
                          |STEPPER14_DIR_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

  /*Configure GPIO pins : PGPin PGPin PGPin PGPin 
                           PGPin */
  GPIO_InitStruct.Pin = FAN11_PIN_Pin|FAN10_PIN_Pin|ETHERNET_POWER_CONTROL_PIN_Pin|ETHERNET_RESET_PIN_Pin 
                          |STEPPER12_DIR_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : PJPin PJPin PJPin PJPin 
                           PJPin PJPin PJPin */
  GPIO_InitStruct.Pin = FAN10_FEEDBACK_PIN_Pin|LIMIT15_PIN_Pin|LIMIT14_PIN_Pin|LIMIT13_PIN_Pin 
                          |LIMIT12_PIN_Pin|LIMIT11_PIN_Pin|LIMIT10_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

  /*Configure GPIO pins : PHPin PHPin PHPin PHPin 
                           PHPin PHPin PHPin */
  GPIO_InitStruct.Pin = FAN9_FEEDBACK_PIN_Pin|FAN8_FEEDBACK_PIN_Pin|FAN7_FEEDBACK_PIN_Pin|FAN6_FEEDBACK_PIN_Pin 
                          |FAN5_FEEDBACK_PIN_Pin|FAN4_FEEDBACK_PIN_Pin|FAN3_FEEDBACK_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin PDPin PDPin 
                           PDPin PDPin PDPin PDPin 
                           PDPin */
  GPIO_InitStruct.Pin = FAN1_FEEDBACK_PIN_Pin|MATERIAL1_CHK_PIN_Pin|MATERIAL2_CHK_PIN_Pin|MATERIAL3_CHK_PIN_Pin 
                          |MATERIAL4_CHK_PIN_Pin|MATERIAL5_CHK_PIN_Pin|MATERIAL6_CHK_PIN_Pin|LIMIT16_PIN_Pin 
                          |SD_EXIST_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PKPin PKPin PKPin */
  GPIO_InitStruct.Pin = LIMIT9_PIN_Pin|LIMIT8_PIN_Pin|LIMIT7_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

  /*Configure GPIO pins : PGPin PGPin PGPin PGPin 
                           PGPin PGPin PGPin */
  GPIO_InitStruct.Pin = LIMIT6_PIN_Pin|LIMIT5_PIN_Pin|LIMIT4_PIN_Pin|LIMIT3_PIN_Pin 
                          |LIMIT2_PIN_Pin|LIMIT1_PIN_Pin|ETHERNET_INT_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : PD0 PD1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PKPin PKPin PKPin PKPin 
                           PKPin */
  GPIO_InitStruct.Pin = STEPPER14_STEP_PIN_Pin|STEPPER14_EN_PIN_Pin|STEPPER13_DIR_PIN_Pin|STEPPER13_STEP_PIN_Pin 
                          |STEPPER13_EN_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);
	
/* USER CODE BEGIN 2 */

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_8|GPIO_PIN_10, GPIO_PIN_SET);

  /*Configure GPIO pins : PG8 PG10 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	
/* USER CODE END 2 */

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
