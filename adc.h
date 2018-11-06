#ifndef ADC_H_
#define ADC_H_

#include "defs.h"


void ADC_Init(void);
void ADC_SM(void);
uint16_t adc_read(uint8_t ch);

uint8_t Get_ADC_Value(uint8_t channel, uint16_t * adc_value);

#endif 