#include "usart.h"
#include "defs.h"
#include "crc.h"

#define BUFFER_LENGTH 256
#define USART_BUFFER_MASK 0xFF

#define SOF     0XC7
#define ESC     0X77
#define OFFSET  0X80
#define EOF     0X7C

typedef enum 
{
    READ,
    CRC_CALC,
    HANDLE
} usart_sm_status_t;

typedef struct
{
    uint8_t data[BUFFER_LENGTH];
    uint16_t p_next_empty;
    uint16_t p_next_send;
} data_t;

volatile data_t buffer = {{0}, 0, 0};

typedef enum
{
    BUFFER_EMPTY,
    BUFFER_OK,
    BUFFER_FULL,
    BUFFER_ERROR
} buffer_status_t;

buffer_status_t Buffer_Add_Data(volatile data_t *buffer, uint8_t data)
{
    uint16_t next_index = (buffer->p_next_empty + 1) & USART_BUFFER_MASK;
    if (next_index == buffer->p_next_send)
    {
        return BUFFER_FULL;
    }
    buffer->data[buffer->p_next_empty] = data;
    buffer->p_next_empty = next_index;
    return BUFFER_OK;
}

buffer_status_t Buffer_Read(volatile data_t *buffer, uint8_t *data)
{
    if (buffer->p_next_send == buffer->p_next_empty)
    {
        return BUFFER_EMPTY;

    }
    *data = buffer->data[buffer->p_next_send];
    buffer->p_next_send = (buffer->p_next_send + 1) & USART_BUFFER_MASK;
    return BUFFER_OK;
}

void USART_Init()
{
    UBRR0H = (uint8_t)(MYUBRR >> 8); // Ajuste da transmissão em 9600 bps
    UBRR0L = (uint8_t)(MYUBRR);
    UCSR0B = (uint8_t)((1 << RXEN0) | (1 << TXEN0)); // Habilita transmissão e recepção
    UCSR0C = (uint8_t)(3 << UCSZ00);                 // Modo assincrono, 8 bits, 1 stop, sem paridade
    UCSR0A &= (uint8_t)(~((1 << UDRE0)));
}

void USART_Transmit(unsigned char data)
{
    while (!(UCSR0A & (1 << UDRE0))); // Espera o dado ser enviado
    UDR0 = data; // Envia o dado pela porta serial
}

unsigned char USART_Receive()
{
    return UDR0;
}

task_state_t USART_StrTx(const char *ptr)
{
    static uint16_t i = 0;
    task_state_t state = IDLE;

    /*  checa se terminou de transmitir o ultimo byte */
    if ((UCSR0A & (1 << UDRE0)))
    {
        /*  checa se chegou no final da string */
        if ('\0' != ptr[i])
        {
            UDR0 = ptr[i++];
            state = RUNNING;
        }
        else
        {
            i = 0;
            state = FINISHED;
        }
    }

    return state;
}

void USART_rxIE()
{
    UCSR0B |= (1 << RXCIE0);
}

void USART_SM(void)
{
    static usart_sm_status_t status_sm_rx = READ;
    static uint8_t internal_buffer[20];
    static uint8_t buffer_index = 0;
    static uint8_t flag_escape = 0;
    uint8_t rd = 0;
    uint16_t crc_calc = 0;
    
    switch(status_sm_rx)
    {
        case READ:
            if (BUFFER_OK == Buffer_Read(&buffer, &rd))
            {
				//USART_Transmit(rd);
                switch(rd)
                {
                    case SOF:
                        buffer_index = 0;
                        internal_buffer[buffer_index++] = rd;
                        flag_escape = 0;
                    break;

                    case ESC:
                        if (0 < buffer_index)
                        {
                            flag_escape = 1;
                        }
                    break;

                    case EOF:        
                        if (0 < buffer_index)
                        {                
                            internal_buffer[buffer_index++] = rd;
                            CRC16_Start();
                            status_sm_rx = CRC_CALC;
                        }
                    break;

                    default:
                        if (0 < buffer_index)
                        {
                            if(1 == flag_escape)
                            {
                                flag_escape = 1;
                                rd -= OFFSET;
                            }
                            internal_buffer[buffer_index++] = rd;
                        }
                    break;                        
                }
                
            }
        break;

        case CRC_CALC:
            if(FINISHED == Calc_CRC16_Array(&internal_buffer[1], \
				(buffer_index - 4), &crc_calc))
            {
                uint16_t crc_rec = 0;

                crc_rec = internal_buffer[buffer_index - 3];
                crc_rec <<= 8;
                crc_rec |= internal_buffer[buffer_index - 2];
                
                if(crc_rec == crc_calc)
                {
                    status_sm_rx = HANDLE;
                }
                else
                {
                    buffer_index = 0;
                    status_sm_rx = READ;
                }
            }
        break;

        case HANDLE:
            for (uint8_t i = 0; i < buffer_index; i++)
			{
				USART_Transmit(internal_buffer[i]);
			}
            buffer_index = 0;
			status_sm_rx = READ;
        break;

        default:
            status_sm_rx = READ;
        break;
    }
}

ISR(USART0_RX_vect)
{
    char res;
    res = UDR0;
    Buffer_Add_Data(&buffer, res);
}