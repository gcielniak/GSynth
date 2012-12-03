/*
 * GSynth.c
 *
 * Created: 31/08/2012 15:56:48
 *  Author: Greg
 */ 


#include "mcu.h"
#include "usart.h"
#include "usb.h"
#include "comm.h"
#include "usermemory.h"
#include "MidiSynth.h"

int main(void)
{
	//Initialise the mcu
	MCUInit();
	
	USARTInit();
	
	//Initialise the USB port
	USBInit();

	sei();
	
	SynthInit();
	
	//the main loop
    while(1)
    {
		//Send&Receive USB data
		USBTask();
		
		//Send&Receive USART data
		USARTTask();
		
		//Write data to user memory
		UserMemTask();
    }
}