/*
 * timers.c
 *
 * Created: 04/11/2018 10:48:30
 *  Author: PH
 */
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "timers.h"

static volatile uint64_t timer_count = 0;

void Timer_Init(void)
{
	TCCR1B |= 1;
    TCNT1 = 0xC180;
    TIMSK1 |= (1 << TOIE1);
}

uint64_t Get_Timer_Count(void)
{
    return timer_count;
}

uint16_t Get_Timer_Count_Register(void)
{
    return TCNT1;
}

ISR(TIMER1_OVF_vect)
{
    timer_count++;
    TCNT1 = 0xC180;
}