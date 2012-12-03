/** USART.c
 *  Collection of functions related to communication through USART.
 *
 *  \author Grzegorz Cielniak
 *  \date 07/09/2011
 */ 

#include "USART.h"

extern void USARTUserTask(uint8_t data);

uint8_t	usart_rx_buffer[USART_RX_BUFFER_SIZE],
	usart_rx_start = 0,
	usart_rx_end = 0,
	usart_rx_length = 0,
	usart_rx_status;

uint8_t	usart_tx_buffer[USART_TX_BUFFER_SIZE],
	usart_tx_start = 0,
	usart_tx_end = 0,
	usart_tx_length = 0,
	usart_tx_pgm_length = 0;
const uint8_t *usart_tx_pgm = 0;

void USARTInit()
{
	UBRR1H = (BAUD_PRESCALE >> 8);
	UBRR1L = BAUD_PRESCALE;
	
	//receiver & transmitter enable + interrupts
	UCSR1B = _BV(RXCIE1) | _BV(RXEN1) | _BV(TXEN1);

	#ifdef URSEL
		UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);
	#else
		UCSR1C |= (1 << UCSZ10) | (1 << UCSZ11);
	#endif
}

void USARTTask()
{
	//process the rx usart data
	if (usart_rx_length)
	{
		USARTUserTask(usart_rx_buffer[usart_rx_start++]);
		usart_rx_start %= USART_RX_BUFFER_SIZE;
		usart_rx_length--;
	}

	//send the tx usart data
	if (usart_ready())
	{
		if (usart_tx_pgm_length)
		{
			UDR1 = pgm_read_byte(usart_tx_pgm++);
			usart_tx_pgm_length--;			
		}
		else if (usart_tx_length)
		{
			UDR1 = usart_tx_buffer[usart_tx_start++];
			usart_tx_start %= USART_TX_BUFFER_SIZE;
			usart_tx_length--;
		}
	}
}

//*** RX Section ***//
// data received interrupt
// this interrupt takes ~ 23 cycles
ISR(USART1_RX_vect)
{
	usart_rx_status = UCSR1A; // todo - transmission errors
	if (usart_rx_length < USART_RX_BUFFER_SIZE)
	{
		usart_rx_buffer[usart_rx_end++] = UDR1;
		usart_rx_end %= USART_RX_BUFFER_SIZE;
		usart_rx_length++;	
	}
}

//*** TX Section ***//
void USARTSendByte(uint8_t data)
{
	if (usart_tx_length < USART_TX_BUFFER_SIZE)
	{
		usart_tx_buffer[usart_tx_end++] = data;
		usart_tx_end %= USART_TX_BUFFER_SIZE;
		usart_tx_length++;
	}			
}

//return the remaining free space in the TX buffer
uint8_t USARTTXSpace() { return (USART_TX_BUFFER_SIZE - usart_tx_length); }

void USARTSend(uint8_t d1, uint8_t d2, uint8_t d3)
{
	switch (d1 >> 4)
	{
	case 0x8: case 0x9: case 0xA: case 0xB: case 0xE:
		if (USARTTXSpace() >= 3)
		{
			USARTSendByte(d1);
			USARTSendByte(d2);
			USARTSendByte(d3);
		}			
		break;
	case 0xC: case 0xD:
		if (USARTTXSpace() >= 2)
		{
			USARTSendByte(d1);
			USARTSendByte(d2);
		}			
		break;
	case 0xF:
		USARTSendByte(d1);
		break;
	default: 
		break;
	}
}

void USARTSendSysex(unsigned char *data, unsigned char length)
{
	if (USARTTXSpace() >= (length + 3))
	{
		USARTSendByte(0xF0);
		USARTSendByte(VENDOR_ID);		

		for (uint8_t i = 0; i < length; i++)
			USARTSendByte(*data++);
		
		USARTSendByte(0xF7);
	}
}

void USARTSendP(const uint8_t *data, unsigned char length)
{
	if (!usart_tx_pgm_length && !usart_tx_length)
	{
		usart_tx_pgm = data;
		usart_tx_pgm_length = length;
	}
}
