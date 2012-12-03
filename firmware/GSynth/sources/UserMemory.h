/*! \file
 * UserMemory.h
 *
 * Created: 01/06/2011 22:53:46
 *  Author: Greg
 */ 


#ifndef _USER_MEMORY_H_
#define _USER_MEMORY_H_

#include <avr/interrupt.h>
#include <avr/eeprom.h>

#define USERMEM_BUFFER_SIZE 32

void UserMemTask();
void UserMemWrite(uint16_t address, uint8_t data);
uint8_t UserMemRead(uint16_t address);

struct UMData 
{
	uint16_t address;
	uint8_t data;	
};
typedef struct UMData UMData;

#endif /* _USER_MEMORY_H_ */