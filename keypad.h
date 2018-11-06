/*
 * keypad.h
 *
 * Created: 29/05/2017 19:10:39
 *  Author: Fernando
 */ 


#ifndef KEYPAD_H_
#define KEYPAD_H_
#include <avr\io.h>

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int16_t read_LCD_buttons(unsigned int adc_key_in); 



#endif /* KEYPAD_H_ */