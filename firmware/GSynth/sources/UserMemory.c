#include "UserMemory.h"

UMData usermem_buffer[USERMEM_BUFFER_SIZE];

uint8_t	usermem_start = 0,
	usermem_end = 0,
	usermem_length = 0;

void UserMemTask()
{
	if (usermem_length && eeprom_is_ready())
	{
		cli();
		eeprom_write_byte((uint8_t*)usermem_buffer[usermem_start].address, usermem_buffer[usermem_start].data);
		sei();
		usermem_start++;
		usermem_start %= USERMEM_BUFFER_SIZE;		
		usermem_length--;
	}
}

void UserMemWrite(uint16_t address, uint8_t data)
{
	if (usermem_length < USERMEM_BUFFER_SIZE)
	{
		usermem_buffer[usermem_end].address = address;
		usermem_buffer[usermem_end++].data = data;
		usermem_end %= USERMEM_BUFFER_SIZE;
		usermem_length++;
	}
}

uint8_t UserMemRead(uint16_t address)
{
	eeprom_busy_wait();
	return eeprom_read_byte((uint8_t*)address);	
}
