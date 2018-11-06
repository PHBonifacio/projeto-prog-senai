#ifndef __USART_H__
#define __USART_H__

#include "defs.h"

void USART_Init(void);
void USART_Transmit(unsigned char data);
unsigned char USART_Receive();
task_state_t USART_StrTx(const char*);
void USART_rxIE(void); 
void USART_SM(void);

#define F_CPU 16000000UL					
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
#include <avr/io.h>
#include <util/delay.h>		
#include <avr/interrupt.h>			


#endif