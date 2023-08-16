#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
extern u8 dum;
extern u8 Rx_DMA_receive[100];  //100bytes
extern u8 Rx_DMA_receive_size ;
void uart_init(u32 bound);
extern uint16_t dataStorage[] ;
#endif


