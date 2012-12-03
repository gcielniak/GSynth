/** \file MCU.h
*  MCU specific functions.
*
*  \author Grzegorz Cielniak
*  \date 07/09/2011
*/

#ifndef _MCU_H_
#define _MCU_H_

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#ifndef DOXYGEN_SKIP

//definitions to maintain compatibility with old MCUs
#ifndef UCSR1A
#	define UCSR1A UCSRA
#endif
#ifndef UDR1
#	define UDR1 UDR
#endif
#ifndef UDRE1
#	define UDRE1 UDRE
#endif
#ifndef UBRR1H
#	define UBRR1H UBRRH
#endif
#ifndef UBRR1L
#	define UBRR1L UBRRL
#endif
#ifndef UCSR1B
#	define UCSR1B UCSRB
#endif
#ifndef RXCIE1
#	define RXCIE1 RXCIE
#endif
#ifndef RXEN1
#	define RXEN1 RXEN
#endif
#ifndef TXEN1
#	define TXEN1 TXEN
#endif
#ifndef USART1_RX_vect
#	define USART1_RX_vect USART_RXC_vect
#endif
#ifndef OCR0A
#	define OCR0A OCR0
#endif
#ifndef TCCR0A
#	define TCCR0A TCCR0
#endif
#ifndef TCCR0B
#	define TCCR0B TCCR0
#endif
#ifndef COM0A0
#	define COM0A0 COM00
#endif

#endif //DOXYGEN_SKIP

/** \brief Defines the eeprom size.
*
*/
#define EEPROM_SIZE (E2END + 1)

/** \brief Get the sound chip oscillator frequency.
*
* The sound chip frequency = F_CPU/GetClockDiv()
*
*/
#define GetClockDiv() ((OCR0A+1)*2)

/** \brief Set the sound chip oscillator frequency.
*
* The sound chip frequency = F_CPU/SetClockDiv()
*
*/
#define SetClockDiv(value) (OCR0A = value/2-1)

/** \brief Initialise MCU.
*
* This function performs the following:
*  - disables JTAG interface (might colide with I/O ports in some configurations);
*  - disables Watchdog;
*  - sets the oscilator divider.
*
*/
void MCUInit();

#endif /* _MCU_H_ */