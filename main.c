/*
 * projeto.c
 *
 * Created: 24/10/2018 21:45:18
 * Author : PH
 */ 

#define SIZE_REC_BUFFER 10
#define F_CPU			16000000UL

#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "defs.h"
#include "usart.h"
#include "adc.h"
#include "lcd.h"
#include "timers.h"

int main(void)
{
	Timer_Init();
    USART_Init();
    USART_rxIE();
    
	sei();

    ADC_Init();
    DisplayLCD_Init();
    
    DisplayLCD_Out(1,1, (unsigned char *)"TESTE lcd");
    DisplayLCD_Out(2,0, (unsigned char *)"BOM DIA");
    _delay_ms(1000);
    
    while(FINISHED != USART_StrTx((const char *)"Teste\n"));
	while(FINISHED != USART_StrTx((const char *)"ola\n"));

    while (1) 
    {
		USART_SM();
        ADC_SM();
	}
}
