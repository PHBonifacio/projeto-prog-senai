/*
 * crc.h
 *
 * Created: 27/10/2018 11:35:29
 *  Author: PH
 */ 


#ifndef CRC_H_
#define CRC_H_

void CRC16_Start();
task_state_t Calc_CRC16_Array(uint8_t* array, uint16_t size, uint16_t* result);




#endif /* CRC_H_ */