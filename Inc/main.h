/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define STEPPER8_DIR_PIN_Pin GPIO_PIN_2
#define STEPPER8_DIR_PIN_GPIO_Port GPIOE
#define STEPPER8_STEP_PIN_Pin GPIO_PIN_3
#define STEPPER8_STEP_PIN_GPIO_Port GPIOE
#define STEPPER8_EN_PIN_Pin GPIO_PIN_4
#define STEPPER8_EN_PIN_GPIO_Port GPIOE
#define STEPPER7_DIR_PIN_Pin GPIO_PIN_5
#define STEPPER7_DIR_PIN_GPIO_Port GPIOE
#define STEPPER7_STEP_PIN_Pin GPIO_PIN_6
#define STEPPER7_STEP_PIN_GPIO_Port GPIOE
#define STEPPER7_EN_PIN_Pin GPIO_PIN_8
#define STEPPER7_EN_PIN_GPIO_Port GPIOI
#define STEPPER_M6_DIR_PIN_Pin GPIO_PIN_13
#define STEPPER_M6_DIR_PIN_GPIO_Port GPIOC
#define STEPPER_M6_STEP_PIN_Pin GPIO_PIN_9
#define STEPPER_M6_STEP_PIN_GPIO_Port GPIOI
#define STEPPER_M6_EN_PIN_Pin GPIO_PIN_10
#define STEPPER_M6_EN_PIN_GPIO_Port GPIOI
#define STEPPER_M5_DIR_PIN_Pin GPIO_PIN_11
#define STEPPER_M5_DIR_PIN_GPIO_Port GPIOI
#define STEPPER_M5_STEP_PIN_Pin GPIO_PIN_0
#define STEPPER_M5_STEP_PIN_GPIO_Port GPIOF
#define STEPPER_M5_EN_PIN_Pin GPIO_PIN_1
#define STEPPER_M5_EN_PIN_GPIO_Port GPIOF
#define STEPPER_M4_DIR_PIN_Pin GPIO_PIN_2
#define STEPPER_M4_DIR_PIN_GPIO_Port GPIOF
#define STEPPER_M4_STEP_PIN_Pin GPIO_PIN_12
#define STEPPER_M4_STEP_PIN_GPIO_Port GPIOI
#define STEPPER_M4_EN_PIN_Pin GPIO_PIN_13
#define STEPPER_M4_EN_PIN_GPIO_Port GPIOI
#define STEPPER_M3_DIR_PIN_Pin GPIO_PIN_14
#define STEPPER_M3_DIR_PIN_GPIO_Port GPIOI
#define STEPPER_M3_STEP_PIN_Pin GPIO_PIN_3
#define STEPPER_M3_STEP_PIN_GPIO_Port GPIOF
#define STEPPER_M3_EN_PIN_Pin GPIO_PIN_4
#define STEPPER_M3_EN_PIN_GPIO_Port GPIOF
#define STEPPER_M2_DIR_PIN_Pin GPIO_PIN_5
#define STEPPER_M2_DIR_PIN_GPIO_Port GPIOF
#define STEPPER_M2_STEP_PIN_Pin GPIO_PIN_6
#define STEPPER_M2_STEP_PIN_GPIO_Port GPIOF
#define STEPPER_M2_EN_PIN_Pin GPIO_PIN_7
#define STEPPER_M2_EN_PIN_GPIO_Port GPIOF
#define STEPPER_M1_DIR_PIN_Pin GPIO_PIN_8
#define STEPPER_M1_DIR_PIN_GPIO_Port GPIOF
#define STEPPER_M1_STEP_PIN_Pin GPIO_PIN_9
#define STEPPER_M1_STEP_PIN_GPIO_Port GPIOF
#define STEPPER_M1_EN_PIN_Pin GPIO_PIN_10
#define STEPPER_M1_EN_PIN_GPIO_Port GPIOF
#define ADC1_TEMP1_PIN_Pin GPIO_PIN_1
#define ADC1_TEMP1_PIN_GPIO_Port GPIOC
#define ADC1_TEMP2_PIN_Pin GPIO_PIN_2
#define ADC1_TEMP2_PIN_GPIO_Port GPIOC
#define ADC1_TEMP3_PIN_Pin GPIO_PIN_3
#define ADC1_TEMP3_PIN_GPIO_Port GPIOC
#define ADC1_TEMP4_PIN_Pin GPIO_PIN_0
#define ADC1_TEMP4_PIN_GPIO_Port GPIOA
#define ADC1_TEMP5_PIN_Pin GPIO_PIN_1
#define ADC1_TEMP5_PIN_GPIO_Port GPIOA
#define ADC1_TEMP6_PIN_Pin GPIO_PIN_2
#define ADC1_TEMP6_PIN_GPIO_Port GPIOA
#define TEMP_HUMIDITY_SIGNAL1_PIN_Pin GPIO_PIN_2
#define TEMP_HUMIDITY_SIGNAL1_PIN_GPIO_Port GPIOH
#define TEMP_HUMIDITY_SIGNAL2_PIN_Pin GPIO_PIN_3
#define TEMP_HUMIDITY_SIGNAL2_PIN_GPIO_Port GPIOH
#define TEMP_HUMIDITY_SIGNAL3_PIN_Pin GPIO_PIN_4
#define TEMP_HUMIDITY_SIGNAL3_PIN_GPIO_Port GPIOH
#define LIGHT4_PIN_Pin GPIO_PIN_7
#define LIGHT4_PIN_GPIO_Port GPIOA
#define LIGHT3_PIN_Pin GPIO_PIN_4
#define LIGHT3_PIN_GPIO_Port GPIOC
#define LIGHT2_PIN_Pin GPIO_PIN_5
#define LIGHT2_PIN_GPIO_Port GPIOC
#define LIGHT1_PIN_Pin GPIO_PIN_0
#define LIGHT1_PIN_GPIO_Port GPIOB
#define BATTERY_CONTROL_PIN_Pin GPIO_PIN_1
#define BATTERY_CONTROL_PIN_GPIO_Port GPIOB
#define OUTAGE_DETECT_PIN_Pin GPIO_PIN_2
#define OUTAGE_DETECT_PIN_GPIO_Port GPIOB
#define BEEP1_PIN_Pin GPIO_PIN_15
#define BEEP1_PIN_GPIO_Port GPIOI
#define LED_Y_PIN_Pin GPIO_PIN_0
#define LED_Y_PIN_GPIO_Port GPIOJ
#define LED_R_PIN_Pin GPIO_PIN_1
#define LED_R_PIN_GPIO_Port GPIOJ
#define LED_G_PIN_Pin GPIO_PIN_2
#define LED_G_PIN_GPIO_Port GPIOJ
#define HEATER6_PIN_Pin GPIO_PIN_3
#define HEATER6_PIN_GPIO_Port GPIOJ
#define HEATER5_PIN_Pin GPIO_PIN_4
#define HEATER5_PIN_GPIO_Port GPIOJ
#define HEATER4_PIN_Pin GPIO_PIN_11
#define HEATER4_PIN_GPIO_Port GPIOF
#define HEATER3_PIN_Pin GPIO_PIN_12
#define HEATER3_PIN_GPIO_Port GPIOF
#define HEATER2_PIN_Pin GPIO_PIN_13
#define HEATER2_PIN_GPIO_Port GPIOF
#define HEATER1_PIN_Pin GPIO_PIN_14
#define HEATER1_PIN_GPIO_Port GPIOF
#define FAN12_PIN_Pin GPIO_PIN_15
#define FAN12_PIN_GPIO_Port GPIOF
#define FAN11_PIN_Pin GPIO_PIN_0
#define FAN11_PIN_GPIO_Port GPIOG
#define FAN10_PIN_Pin GPIO_PIN_1
#define FAN10_PIN_GPIO_Port GPIOG
#define FAN9_PIN_Pin GPIO_PIN_7
#define FAN9_PIN_GPIO_Port GPIOE
#define FAN8_PIN_Pin GPIO_PIN_8
#define FAN8_PIN_GPIO_Port GPIOE
#define FAN7_PIN_Pin GPIO_PIN_9
#define FAN7_PIN_GPIO_Port GPIOE
#define FAN6_PIN_Pin GPIO_PIN_10
#define FAN6_PIN_GPIO_Port GPIOE
#define FAN5_PIN_Pin GPIO_PIN_11
#define FAN5_PIN_GPIO_Port GPIOE
#define FAN4_PIN_Pin GPIO_PIN_12
#define FAN4_PIN_GPIO_Port GPIOE
#define FAN3_PIN_Pin GPIO_PIN_13
#define FAN3_PIN_GPIO_Port GPIOE
#define FAN2_PIN_Pin GPIO_PIN_14
#define FAN2_PIN_GPIO_Port GPIOE
#define FAN1_PIN_Pin GPIO_PIN_15
#define FAN1_PIN_GPIO_Port GPIOE
#define FAN12_FEEDBACK_PIN_Pin GPIO_PIN_10
#define FAN12_FEEDBACK_PIN_GPIO_Port GPIOB
#define FAN11_FEEDBACK_PIN_Pin GPIO_PIN_11
#define FAN11_FEEDBACK_PIN_GPIO_Port GPIOB
#define FAN10_FEEDBACK_PIN_Pin GPIO_PIN_5
#define FAN10_FEEDBACK_PIN_GPIO_Port GPIOJ
#define FAN9_FEEDBACK_PIN_Pin GPIO_PIN_6
#define FAN9_FEEDBACK_PIN_GPIO_Port GPIOH
#define FAN8_FEEDBACK_PIN_Pin GPIO_PIN_7
#define FAN8_FEEDBACK_PIN_GPIO_Port GPIOH
#define FAN7_FEEDBACK_PIN_Pin GPIO_PIN_8
#define FAN7_FEEDBACK_PIN_GPIO_Port GPIOH
#define FAN6_FEEDBACK_PIN_Pin GPIO_PIN_9
#define FAN6_FEEDBACK_PIN_GPIO_Port GPIOH
#define FAN5_FEEDBACK_PIN_Pin GPIO_PIN_10
#define FAN5_FEEDBACK_PIN_GPIO_Port GPIOH
#define FAN4_FEEDBACK_PIN_Pin GPIO_PIN_11
#define FAN4_FEEDBACK_PIN_GPIO_Port GPIOH
#define FAN3_FEEDBACK_PIN_Pin GPIO_PIN_12
#define FAN3_FEEDBACK_PIN_GPIO_Port GPIOH
#define FAN2_FEEDBACK_PIN_Pin GPIO_PIN_12
#define FAN2_FEEDBACK_PIN_GPIO_Port GPIOB
#define USB_EN_PIN_Pin GPIO_PIN_13
#define USB_EN_PIN_GPIO_Port GPIOB
#define FAN1_FEEDBACK_PIN_Pin GPIO_PIN_8
#define FAN1_FEEDBACK_PIN_GPIO_Port GPIOD
#define MATERIAL1_CHK_PIN_Pin GPIO_PIN_9
#define MATERIAL1_CHK_PIN_GPIO_Port GPIOD
#define MATERIAL2_CHK_PIN_Pin GPIO_PIN_10
#define MATERIAL2_CHK_PIN_GPIO_Port GPIOD
#define MATERIAL3_CHK_PIN_Pin GPIO_PIN_11
#define MATERIAL3_CHK_PIN_GPIO_Port GPIOD
#define MATERIAL4_CHK_PIN_Pin GPIO_PIN_12
#define MATERIAL4_CHK_PIN_GPIO_Port GPIOD
#define MATERIAL5_CHK_PIN_Pin GPIO_PIN_13
#define MATERIAL5_CHK_PIN_GPIO_Port GPIOD
#define MATERIAL6_CHK_PIN_Pin GPIO_PIN_14
#define MATERIAL6_CHK_PIN_GPIO_Port GPIOD
#define LIMIT16_PIN_Pin GPIO_PIN_15
#define LIMIT16_PIN_GPIO_Port GPIOD
#define LIMIT15_PIN_Pin GPIO_PIN_6
#define LIMIT15_PIN_GPIO_Port GPIOJ
#define LIMIT14_PIN_Pin GPIO_PIN_7
#define LIMIT14_PIN_GPIO_Port GPIOJ
#define LIMIT13_PIN_Pin GPIO_PIN_8
#define LIMIT13_PIN_GPIO_Port GPIOJ
#define LIMIT12_PIN_Pin GPIO_PIN_9
#define LIMIT12_PIN_GPIO_Port GPIOJ
#define LIMIT11_PIN_Pin GPIO_PIN_10
#define LIMIT11_PIN_GPIO_Port GPIOJ
#define LIMIT10_PIN_Pin GPIO_PIN_11
#define LIMIT10_PIN_GPIO_Port GPIOJ
#define LIMIT9_PIN_Pin GPIO_PIN_0
#define LIMIT9_PIN_GPIO_Port GPIOK
#define LIMIT8_PIN_Pin GPIO_PIN_1
#define LIMIT8_PIN_GPIO_Port GPIOK
#define LIMIT7_PIN_Pin GPIO_PIN_2
#define LIMIT7_PIN_GPIO_Port GPIOK
#define LIMIT6_PIN_Pin GPIO_PIN_2
#define LIMIT6_PIN_GPIO_Port GPIOG
#define LIMIT5_PIN_Pin GPIO_PIN_3
#define LIMIT5_PIN_GPIO_Port GPIOG
#define LIMIT4_PIN_Pin GPIO_PIN_4
#define LIMIT4_PIN_GPIO_Port GPIOG
#define LIMIT3_PIN_Pin GPIO_PIN_5
#define LIMIT3_PIN_GPIO_Port GPIOG
#define LIMIT2_PIN_Pin GPIO_PIN_6
#define LIMIT2_PIN_GPIO_Port GPIOG
#define LIMIT1_PIN_Pin GPIO_PIN_7
#define LIMIT1_PIN_GPIO_Port GPIOG
#define EEPROM_SCL_Pin GPIO_PIN_6
#define EEPROM_SCL_GPIO_Port GPIOC
#define EEPROM_SDA_Pin GPIO_PIN_7
#define EEPROM_SDA_GPIO_Port GPIOC
#define WIFI_POWER_CONTROL_PIN_Pin GPIO_PIN_8
#define WIFI_POWER_CONTROL_PIN_GPIO_Port GPIOA
#define SD_EXIST_PIN_Pin GPIO_PIN_3
#define SD_EXIST_PIN_GPIO_Port GPIOD
#define STEPPER15_DIR_PIN_Pin GPIO_PIN_12
#define STEPPER15_DIR_PIN_GPIO_Port GPIOJ
#define STEPPER15_STEP_PIN_Pin GPIO_PIN_13
#define STEPPER15_STEP_PIN_GPIO_Port GPIOJ
#define STEPPER15_EN_PIN_Pin GPIO_PIN_14
#define STEPPER15_EN_PIN_GPIO_Port GPIOJ
#define STEPPER14_DIR_PIN_Pin GPIO_PIN_15
#define STEPPER14_DIR_PIN_GPIO_Port GPIOJ
#define ETHERNET_POWER_CONTROL_PIN_Pin GPIO_PIN_9
#define ETHERNET_POWER_CONTROL_PIN_GPIO_Port GPIOG
#define ETHERNET_RESET_PIN_Pin GPIO_PIN_10
#define ETHERNET_RESET_PIN_GPIO_Port GPIOG
#define ETHERNET_INT_PIN_Pin GPIO_PIN_11
#define ETHERNET_INT_PIN_GPIO_Port GPIOG
#define STEPPER14_STEP_PIN_Pin GPIO_PIN_3
#define STEPPER14_STEP_PIN_GPIO_Port GPIOK
#define STEPPER14_EN_PIN_Pin GPIO_PIN_4
#define STEPPER14_EN_PIN_GPIO_Port GPIOK
#define STEPPER13_DIR_PIN_Pin GPIO_PIN_5
#define STEPPER13_DIR_PIN_GPIO_Port GPIOK
#define STEPPER13_STEP_PIN_Pin GPIO_PIN_6
#define STEPPER13_STEP_PIN_GPIO_Port GPIOK
#define STEPPER13_EN_PIN_Pin GPIO_PIN_7
#define STEPPER13_EN_PIN_GPIO_Port GPIOK
#define STEPPER12_DIR_PIN_Pin GPIO_PIN_15
#define STEPPER12_DIR_PIN_GPIO_Port GPIOG
#define STEPPER12_STEP_PIN_Pin GPIO_PIN_5
#define STEPPER12_STEP_PIN_GPIO_Port GPIOB
#define STEPPER12_EN_PIN_Pin GPIO_PIN_6
#define STEPPER12_EN_PIN_GPIO_Port GPIOB
#define STEPPER11_DIR_PIN_Pin GPIO_PIN_7
#define STEPPER11_DIR_PIN_GPIO_Port GPIOB
#define STEPPER11_STEP_PIN_Pin GPIO_PIN_8
#define STEPPER11_STEP_PIN_GPIO_Port GPIOB
#define STEPPER11_EN_PIN_Pin GPIO_PIN_9
#define STEPPER11_EN_PIN_GPIO_Port GPIOB
#define STEPPER10_DIR_PIN_Pin GPIO_PIN_0
#define STEPPER10_DIR_PIN_GPIO_Port GPIOE
#define STEPPER10_STEP_PIN_Pin GPIO_PIN_1
#define STEPPER10_STEP_PIN_GPIO_Port GPIOE
#define STEPPER10_EN_PIN_Pin GPIO_PIN_4
#define STEPPER10_EN_PIN_GPIO_Port GPIOI
#define STEPPER9_DIR_PIN_Pin GPIO_PIN_5
#define STEPPER9_DIR_PIN_GPIO_Port GPIOI
#define STEPPER9_STEP_PIN_Pin GPIO_PIN_6
#define STEPPER9_STEP_PIN_GPIO_Port GPIOI
#define STEPPER9_EN_PIN_Pin GPIO_PIN_7
#define STEPPER9_EN_PIN_GPIO_Port GPIOI

/* USER CODE BEGIN Private defines */
#define _GET_PORT(IO)				(IO ## _GPIO_Port)
#define _GET_PIN(IO)				(IO ## _Pin)

#define GET_PORT(IO)				_GET_PORT(IO)
#define GET_PIN(IO)					_GET_PIN(IO)


#define WRITE(IO, v)           do {\
                                    HAL_GPIO_WritePin(PORT(IO), PIN(IO), v);  \
                                }while(0)

#define READ(IO)               (bool)HAL_GPIO_ReadPin(PORT(IO), PIN(IO))
/* USER CODE END Private defines */

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
