/** \file USB.h
 *  Collection of functions related to communication through USB.
 *
 *  \author Grzegorz Cielniak
 *  \date 07/09/2011
 */ 

#ifndef _USB_H_
#define _USB_H_

#include <avr/io.h>
#include <avr/pgmspace.h>

//! Length of the USB RX buffer - should be a power of 2 for speed reasons
#define USB_RX_BUFFER_SIZE 8
//! Length of the USB TX buffer - should be a power of 2 for speed reasons
#define USB_TX_BUFFER_SIZE 8

#ifndef DOXYGEN_SKIP

#define VENDOR_ID 0x7D

#define EVENT_LENGTH 4

#endif //DOXYGEN_SKIP

/** \brief Initialise USB.
 *
 */
void USBInit();

/** \brief General USB task.
 *
 *  Receive and send messages.
 */
void USBTask();

/** \brief Send a single MIDI byte
 *
 *  Basic function used by other send commands. The function pushes a single byte into FIFO queue
 *  that is later used by the send routine.
 */
void USBSendByte(uint8_t);

/** \brief Send a short MIDI command
 *
 *  If the command is shorter than three bytes the remaining bytes are ignored.
 */
void USBSend(uint8_t, uint8_t, uint8_t);

/** \brief Send a SYSEX command.
 *
 *  This command automatically appends 0xF0, VENDOR_ID and 0xF7 bytes.
 */
void USBSendSysex(uint8_t*, uint8_t);

/** \brief Send a command from FLASH.
 *
 *  Send a command stored in the FLASH memory. The function checks if other commands
 *  are not being sent at the same time.
 */
void USBSendP(const uint8_t*, uint8_t);

#endif /* _USB_H_ */
