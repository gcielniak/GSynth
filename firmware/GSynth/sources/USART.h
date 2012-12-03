/** \file USART.h
 *  Collection of functions related to communication through USART.
 *
 *  \author Grzegorz Cielniak
 *  \date 07/09/2011
 */ 

#ifndef _USART_H_
#define _USART_H_

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "MCU.h"

//! Length of the USART RX buffer - should be a power of 2 for speed reasons
#define USART_RX_BUFFER_SIZE 8
//! Length of the USART TX buffer - should be a power of 2 for speed reasons
#define USART_TX_BUFFER_SIZE 8

#ifndef DOXYGEN_SKIP

#define USART_BAUDRATE 31250
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1) 

#define VENDOR_ID 0x7D

#define usart_ready() (UCSR1A & (1<<UDRE1))

#endif //DOXYGEN_SKIP

/** \brief Initialise USART.
 *
 *  Set MIDI baud rate to 31250 bps, switch on the received and transmitter
 *  and enable RXReceived interrupt.
 */
void USARTInit();

/** \brief General USART task.
 *
 *  Receive and send messages.
 */
void USARTTask();

/** \brief Send a single MIDI byte
 *
 *  Basic function used by other send commands. The function pushes a single byte into FIFO queue
 *  that is later used by the send routine.
 */
void USARTSendByte(uint8_t data);

uint8_t USARTTXSpace();

/** \brief Send a short MIDI command
 *
 *  If the command is shorter than three bytes the remaining bytes are ignored.
 */
void USARTSend(uint8_t d1, uint8_t d2, uint8_t d3);

/** \brief Send a SYSEX command.
 *
 *  This command automatically append 0xF0, VENDOR_ID and 0xF7 bytes.
 */
void USARTSendSysex(unsigned char *data, unsigned char length);

/** \brief Send a command from FLASH.
 *
 *  Send a command stored in the FLASH memory. The function checks if other commands
 *  are not being sent at the same time.
 */
void USARTSendP(const uint8_t *data, unsigned char length);

#endif /* _USART_H_ */
