#include "RS485.h"
#include "ProtocolMng.h"

											
void RS485_TX(const uint8_t* buf,uint32_t length)
{
	  RS485_TX_EN();
    USART1_Write(buf, length);
}	

uint32_t RS485_RX(uint8_t * const buf, uint32_t length)
{
    return USART1_Read(buf, length);
}

