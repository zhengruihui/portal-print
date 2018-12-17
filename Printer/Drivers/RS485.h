#include "usart.h"



#define RS485_TX_EN()    do{ \
	                             HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_12, GPIO_PIN_SET); \
                               HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_SET); \
												       HAL_Delay(1); \
                            }while(0);



#define RS485_RX_EN()    do{ \
	                             HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_12, GPIO_PIN_RESET); \
                               HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_RESET); \
												       HAL_Delay(1); \
                            }while(0);

														
														
extern void RS485_TX(const uint8_t* buf,uint32_t length);
extern uint32_t RS485_RX(uint8_t * const buf, uint32_t length);
													