/*
* MCUInterface.c
*
* Created: 30/05/2011 17:46:58
*  Author: Greg
*/

#include "MCU.h"

void MCUInit()
{
	/* disable JTAG to allow corresponding pins to be used */
	// note the JTD bit must be written twice within 4 clock cycles to disable JTAG
	// you must also set the IVSEL bit at the same time, which requires IVCE to be set first
	// port pull-up resistors are enabled - PUD(Pull Up Disable) = 0
	MCUCR = (1 << JTD) | (1 << IVCE) | (0 << PUD);
	MCUCR = (1 << JTD) | (0 << IVSEL) | (0 << IVCE) | (0 << PUD);
	
	/* Disable watchdog if enabled by bootloader/fuses */
	#ifdef MCUSR
	MCUSR &= ~(1 << WDRF);
	#endif
	wdt_disable();

	/* Disable clock division */
	#ifdef clock_prescale_get
	clock_prescale_set(clock_div_1);
	#endif
}
