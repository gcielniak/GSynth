/** \file Comm.h
 *  A collection of functions related to MIDI communication.
 *
 *  Functions defined in this module should be revised - mixing of two streams (USB/USART) is technically difficult
 *  and not critical.
 *
 *  \author Grzegorz Cielniak
 *  \date 07/09/2011
 */ 

#ifndef _COMM_H_
#define _COMM_H_

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "USART.h"
#include "USB.h"

#define MAX_MESSAGE_LENGTH 8

enum CommMode
{
	USART_ONLY,
	USB_ONLY,
	BOTH_PARALLEL,
	BOTH_MIX
};
typedef enum CommMode CommMode;

void USARTUserTask(uint8_t data);
void USBUserTask(uint8_t data);

/** \brief Set the communication mode.
 */
void SetCommMode(CommMode mode);

/** \brief Get the current communication mode.
 */
CommMode GetCommMode();

/** \brief Send a single MIDI byte
 *
 *  Basic function used by other send commands. The function pushes a single byte into FIFO queue
 *  that is later used by the send routine.
 */
void SendByte(uint8_t data);

/** \brief Send a short MIDI command
 *
 *  If the command is shorter than three bytes the remaining bytes are ignored.
 */
void Send(uint8_t d1, uint8_t d2, uint8_t d3);

/** \brief Send a SYSEX command.
 *
 *  This command automatically appends 0xF0, VENDOR_ID and 0xF7 bytes.
 */
void SendSysex(unsigned char *data, unsigned char length);

/** \brief Send a command from FLASH.
 *
 *  Send a command stored in the FLASH memory. The function checks if other commands
 *  are not being sent at the same time.
 */
void SendP(const uint8_t *data, unsigned char length);

#endif /* _COMM_H_ */
