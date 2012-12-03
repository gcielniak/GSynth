/** \file PokeyChip.h
 *  POKEY Chip IO functions.
 *
 * <table>
 * <caption>Pokey Registers</caption>
 * <tr><td>Register</td><td>Name</td><td>Description</td></tr>
 * <tr><td>0x00</td><td>ADUF1</td><td>frequency for Osc1</td></tr>
 * <tr><td>0x01</td><td>ADUC1</td><td>distortion and volume for Osc1</td></tr>
 * <tr><td>0x02</td><td>ADUF2</td><td>frequency for Osc2</td></tr>
 * <tr><td>0x03</td><td>ADUC2</td><td>distortion and volume for Osc2</td></tr>
 * <tr><td>0x04</td><td>ADUF3</td><td>frequency for Osc3</td></tr>
 * <tr><td>0x05</td><td>ADUC3</td><td>distortion and volume for Osc3</td></tr>
 * <tr><td>0x06</td><td>ADUF4</td><td>frequency for Osc4</td></tr>
 * <tr><td>0x07</td><td>ADUC4</td><td>distortion and volume for Osc4</td></tr>
 * <tr><td>0x08</td><td>AUDCTL</td><td>oscillator mode</td></tr>
 * <tr><td>0x09</td><td>STIMER</td><td>reset timers</td></tr>
 * <tr><td>0x0F</td><td>SKCTL</td><td>chip reset</td></tr>
 * </table>
 *
 * Details:
 * - AUDFx: output frequency = F_IN/(2*(AUDF + M)), F_IN can be F_OSC, F_OSC/28, F_OSC/114 depending on AUDCTL, M = 1 if F_IN != F_OSC
 * otherwise, M = 4 for 8-bit setting and M = 7 for 16-bit setting
 * - AUDCx, D7D6D5D4: distortion mode, D3D2D1D0 - volume
 * - STIMER: write any value
 * - SKCTL: D1D0 set to 00
 *
 *
 *
 *  \author Grzegorz Cielniak
 *  \date 07/09/2011
 */ 

#ifndef _POKEYCHIP_H_
#define _POKEYCHIP_H_

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "SynthConfig.h"

#define MAX_VOICES (MAX_CHIPS*4)

/// Initialise all sound chips.
void ChipInit();

/// Reset the specified chip.
void ChipReset(uint8_t chip);

/// Write a data byte to the specified chip and address.
void ChipWrite(uint8_t chip, uint16_t address, uint8_t data);

/// Read a data byte from the specified chip and address.
uint8_t ChipRead(uint8_t chip, uint16_t address);

/// Read a data byte from the register cache.
uint8_t ChipReadCache(uint8_t chip, uint16_t address);

/// Set volume for the selected voice.
/// 
/// The maximum volume is equal to 128 (0x80).
void VoiceVolume(uint8_t voice, uint8_t value);

/// Set note for the selected voice.
/// 
/// The note value is specified in 1/128th of a semitone which simplifies the implementation.
/// For example to set a note corresponding to the 60th midi note use VoiceNote(0, 60<<7).
void VoiceNote(uint8_t voice, int16_t note);

// --- POKEY section ---

#ifndef DOXYGEN_SKIP

#define AUDF1 0x00
#define AUDF2 0x02
#define AUDF3 0x04
#define AUDF4 0x06
#define AUDC1 0x01
#define AUDC2 0x03
#define AUDC3 0x05
#define AUDC4 0x07
#define AUDCTL 0x08
#define STIMER 0x09
#define SKCTL 0x0F

#define CLK_PORT PINB
#define CLK_PIN 7
#define SetAddress(address) {PORTB &= 0x8F; PORTB |= (address<<4)&0x70; PORTC &= 0xBF;	PORTC |= ((address<<4)&0x80)>>1;}
#define SetData(data) {	PORTD &= 0x0C; PORTD |= (data&0x03) | ((data<<2)&0xF0);	PORTE &= 0xBB; PORTE |= ((data&0x40)>>4) | ((data&0x80)>>1);}
#define ChipSelect(chip) (PORTF &= ~(_BV(chip)<<4))
#define AllChipOff() (PORTF |= 0xF0)
#define	RW_Write() (PORTF &= ~_BV(1))
#define	RW_Read() (PORTF |= _BV(1))
#define DataPortIn() { DDRD &= ~(_BV(0)|_BV(1)|_BV(4)|_BV(5)|_BV(6)|_BV(7)); DDRE &= ~(_BV(2) | _BV(6));}
#define DataPortOut() {	DDRD |= _BV(0)|_BV(1)|_BV(4)|_BV(5)|_BV(6)|_BV(7); DDRE |= _BV(2) | _BV(6); }
#define ReadDataPort() ((PIND&0x03) | ((PIND&0xF0)>>2) | ((PINE&0x04)<<4) | ((PINE&0x40)<<1))

#endif // DOXYGEN_SKIP

/// \brief A structure holding chip number and AUDFx register address corresponding to a single voice.
struct PokeyVoice
{
	uint8_t chip;
	uint8_t address;	
};
typedef struct PokeyVoice PokeyVoice;

/// \brief A structure holding Pokey specific instrument parameters.
struct POKEYInstrument
{
	uint8_t osc_config;
	uint8_t distortion;
	uint16_t filter;
};
typedef struct POKEYInstrument POKEYInstrument;

/// Set the oscillator mode - POKEY specific function.
void POKEY_OscMode(uint8_t voice, uint8_t value);

/// Set the distortion mode - POKEY specific function.
void POKEY_Distortion(uint8_t voice, uint8_t value);

/// Set the filter mode - POKEY specific function.
void POKEY_Filter(uint8_t voice, uint16_t value);

#endif // _POKEYCHIP_H_
