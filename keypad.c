/*
 * keypad.c
 *
 * Created: 29/05/2017 19:08:20
 *  Author: Fernando
 */ 
#include "keypad.h"

int16_t read_LCD_buttons(unsigned int adc_key_in)
{              

    if (adc_key_in > 1000) return btnNONE; 

    if (adc_key_in < 50)   return btnRIGHT;  
    if (adc_key_in < 250)  return btnUP; 
    if (adc_key_in < 450)  return btnDOWN; 
    if (adc_key_in < 650)  return btnLEFT; 
    if (adc_key_in < 850)  return btnSELECT;  

    return btnNONE;           
}