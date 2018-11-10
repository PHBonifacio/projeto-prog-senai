/*
 * crc.c
 *
 * Created: 27/10/2018 11:35:53
 *  Author: PH
 */ 

#include <stdint.h>
#include <stdio.h>
#include "defs.h"
#include "crc.h"

static uint16_t crc16 = 0;

void CRC16_Start()
{
    crc16 = 0;
}

task_state_t Calc_CRC16_Byte(uint8_t byte)
{
    task_state_t result = RUNNING;

    static uint8_t j = 0;
    uint16_t test = 0;

    test = ((uint16_t)byte) << (j + 8);
    test ^= crc16;
    test &= 0x8000;

    if (test)
    {
        crc16 = (uint16_t)(crc16 << 1);
        crc16 = (uint16_t)(crc16 ^ 0x1021);
    }
    else
    {
        crc16 = (uint16_t)(crc16 << 1);
    }

    j++;

    if(j >= 8)
    {
        result = FINISHED;
        j = 0;
    }

    return result;
}

task_state_t Calc_CRC16_Array(uint8_t* array, uint16_t size, uint16_t* result)
{
    task_state_t tsk_result = RUNNING;
    static uint16_t index = 0;

    if(index < size)
    {
        task_state_t crc_byte_calc = Calc_CRC16_Byte(array[index]);
        
        if(FINISHED == crc_byte_calc)
        {
            index++;
        }
    }
    else
    {
        index = 0;
        tsk_result = FINISHED;
        *result = crc16;
    }

    return tsk_result;
}