
#include <avr/io.h>
#include <stdint.h>
#include "adc.h"

#ifdef ARDUINO
#if MEGA == ARDUINO
#define BUFFER_LENGTH 16

#elif UNO == ARDUINO
#define BUFFER_LENGTH 8

#else
#error "Modelo de placa nao reconhecido"

#endif

#else
#error "Arduino nao definido"

#endif

#define LOWER_CHANNEL 0
#define HIGHER_CHANNEL BUFFER_LENGTH - 1

typedef enum 
{
    READ,
    CONFIG,
    HANDLE
} adc_sm_status_t;

static uint16_t adc_array[BUFFER_LENGTH];

void ADC_Init(void)
{
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_read(uint8_t ch)
{
    // select the corresponding channel 0~7
    // ANDing with ’7' will always keep the value
    // of ‘ch’ between 0 and 7
    ch &= 0b00000111; // AND operation with 7
    ADMUX = (ADMUX & 0xF8) | ch;

    // start single convertion
    // write ’1' to ADSC
    ADCSRA |= (1 << ADSC);

    // wait for conversion to complete
    // ADSC becomes ’0' again
    // till then, run loop continuously
    while (ADCSRA & (1 << ADSC))
    {};

    return (ADCW);
}

uint8_t Get_ADC_Value(uint8_t ch, uint16_t * adc_value)
{
    uint8_t return_value = 0;

    if ((LOWER_CHANNEL < ch) && (ch > HIGHER_CHANNEL))
    {
        //(*adc_value) = adc_array[ch];
        return_value = 1;
    }

    return return_value;
}

void ADC_SM(void)
{
    static uint8_t channel = 0;
    static adc_sm_status_t sm_state = CONFIG;

    switch (sm_state)
    {
        case CONFIG:
            if(channel < 7)
            {
                uint8_t ch = channel;

                ch &= 0b00000111; // AND operation with 7
                ADMUX = (ADMUX & 0xF8) | ch;
            }

            ADCSRA |= (1 << ADSC);

            sm_state = READ;
        break;

        case READ:
            if (0 == (ADCSRA & (1 << ADSC)))
            {
				adc_array[channel] = (uint16_t)(ADCW);

                char  buffer[20];
                if(0 == channel)
                {
                    sprintf(buffer,  "ADC0  %4d", adc_array[0]);
                    DisplayLCD_Out(1, 1, (unsigned char *)buffer);
                }

                channel++;

                if (7 < channel)
                {
                    channel = 0;
                }

                sm_state = CONFIG;
            }
        break;  

        default:
            sm_state = CONFIG;
        break;
    }
}