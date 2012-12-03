#include "PokeyChip.h"

#define OPTIMISE_ME

uint8_t register_cache[MAX_CHIPS][MAX_ADDRESS];
PokeyVoice voice_map[MAX_VOICES];
uint8_t osc_mode[MAX_VOICES];

//AUDF lookup tables for:
// base_frequency
const uint16_t key_freq_1[129] PROGMEM = {0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFD12,0xEEDD,0xE175,0xD4CD,0xC8DB,0xBD95,0xB2F0,0xA8E5,0x9F6A,0x9677,0x8E05,0x860C,0x7E85,0x776B,0x70B7,0x6A63,0x646A,0x5EC7,0x5975,0x546F,0x4FB1,0x4B38,0x46FF,0x4302,0x3F3F,0x3BB2,0x3858,0x352E,0x3231,0x2F60,0x2CB7,0x2A34,0x27D5,0x2598,0x237C,0x217E,0x1F9C,0x1DD6,0x1C28,0x1A93,0x1915,0x17AC,0x1658,0x1516,0x13E7,0x12C9,0x11BA,0x10BB,0x0FCB,0x0EE7,0x0E11,0x0D46,0x0C87,0x0BD3,0x0B28,0x0A88,0x09F0,0x0961,0x08DA,0x085A,0x07E2,0x0770,0x0705,0x06A0,0x0640,0x05E6,0x0591,0x0540,0x04F5,0x04AD,0x0469,0x042A,0x03ED,0x03B5,0x037F,0x034C,0x031D,0x02EF,0x02C5,0x029D,0x0277,0x0253,0x0231,0x0211,0x01F3,0x01D7,0x01BC,0x01A3,0x018B,0x0174,0x015F,0x014B,0x0138,0x0126,0x0115,0x0105,0x00F6,0x00E8,0x00DA,0x00CE,0x00C2,0x00B7,0x00AC,0x00A2,0x0098,0x008F,0x0087,0x007F,0x0078,0x0070,0x006A,0x0063,0x005D,0x0058,0x0052,0x004D,0x0049,0x0044};
// base_frequency/114
const uint16_t key_freq_2[129] PROGMEM = {0x0430,0x03F4,0x03BB,0x0385,0x0353,0x0323,0x02F6,0x02CB,0x02A3,0x027D,0x0259,0x0237,0x0217,0x01F9,0x01DD,0x01C2,0x01A9,0x0191,0x017A,0x0165,0x0151,0x013E,0x012C,0x011B,0x010B,0x00FC,0x00EE,0x00E1,0x00D4,0x00C8,0x00BD,0x00B2,0x00A8,0x009E,0x0096,0x008D,0x0085,0x007E,0x0076,0x0070,0x0069,0x0063,0x005E,0x0059,0x0053,0x004F,0x004A,0x0046,0x0042,0x003E,0x003B,0x0037,0x0034,0x0031,0x002E,0x002C,0x0029,0x0027,0x0025,0x0023,0x0021,0x001F,0x001D,0x001B,0x001A,0x0018,0x0017,0x0015,0x0014,0x0013,0x0012,0x0011,0x0010,0x000F,0x000E,0x000D,0x000C,0x000C,0x000B,0x000A,0x000A,0x0009,0x0008,0x0008,0x0007,0x0007,0x0006,0x0006,0x0006,0x0005,0x0005,0x0005,0x0004,0x0004,0x0004,0x0003,0x0003,0x0003,0x0003,0x0003,0x0002,0x0002,0x0002,0x0002,0x0002,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};
// base_frequency/28
const uint16_t key_freq_3[129] PROGMEM = {0x110F,0x101A,0x0F33,0x0E58,0x0D8A,0x0CC8,0x0C10,0x0B62,0x0ABF,0x0A24,0x0993,0x0909,0x0887,0x080D,0x0799,0x072C,0x06C5,0x0663,0x0607,0x05B1,0x055F,0x0512,0x04C9,0x0484,0x0443,0x0406,0x03CC,0x0395,0x0362,0x0331,0x0303,0x02D8,0x02AF,0x0288,0x0264,0x0242,0x0221,0x0202,0x01E5,0x01CA,0x01B0,0x0198,0x0181,0x016B,0x0157,0x0144,0x0131,0x0120,0x0110,0x0101,0x00F2,0x00E5,0x00D8,0x00CC,0x00C0,0x00B5,0x00AB,0x00A1,0x0098,0x0090,0x0088,0x0080,0x0079,0x0072,0x006B,0x0065,0x0060,0x005A,0x0055,0x0050,0x004C,0x0047,0x0043,0x003F,0x003C,0x0038,0x0035,0x0032,0x002F,0x002D,0x002A,0x0028,0x0025,0x0023,0x0021,0x001F,0x001D,0x001C,0x001A,0x0019,0x0017,0x0016,0x0014,0x0013,0x0012,0x0011,0x0010,0x000F,0x000E,0x000D,0x000D,0x000C,0x000B,0x000A,0x000A,0x0009,0x0009,0x0008,0x0008,0x0007,0x0007,0x0006,0x0006,0x0005,0x0005,0x0005,0x0004,0x0004,0x0004,0x0004,0x0003,0x0003,0x0003,0x0003,0x0002,0x0002,0x0002,0x0002,0x0002};

//lookup table for distortion settings
const uint8_t distortion_type[] PROGMEM = {0xA0,0x00,0x00,0x80,0x80,0x60,0x40,0xC0,0xF0};

//int8_t lfsr[15] = {0, 0, -1, 1, 0, -1, 0, 0, -1, 0, 0, -1, 0, -1, 1};
int8_t lfsr[15] = {2, 1, 0, -1, 1, 0, -1, -2, -3, 2, 1, 0, -1, 1, 0};
	
//lookup table for configuration masks - finding the voice settings
const uint8_t config_mask[28][4] PROGMEM = {
	{0x00,0x00},//a
	{0x11,0x11},//b
	{0x2F,0x2F},//c

	{0xF3,0xF3},//aa
	{0xF4,0xF4},//bb
	{0xF5,0xF5},//cc

	{0x00,0xFF},//a_a
	{0x0F,0xFF},//a_c
	{0x11,0xFF},//b_b
	{0x1F,0xFF},//b_c
	{0x2F,0xFF},//c_a
	{0x2F,0xFF},//c_b
	{0x2F,0xFF},//c_c

	{0xF3,0xFF},//aa_a
	{0xF4,0xFF},//bb_b
	{0xF5,0xFF},//cc_a
	{0xF5,0xFF},//cc_b

	{0xF0,0xFF},//a_aa
	{0xF0,0xFF},//a_cc
	{0xF1,0xFF},//b_bb
	{0xF1,0xFF},//b_cc

	{0xF3,0xFF},//aa_aa
	{0xF3,0xFF},//aa_cc
	{0xF4,0xFF},//bb_bb
	{0xF4,0xFF},//bb_cc
	{0xF5,0xFF},//cc_aa
	{0xF5,0xFF},//cc_bb
	{0xF5,0xFF} //cc_cc
};

//lookup tables for AUDCTL register settings
const uint8_t audctl_mask_off[28][4] PROGMEM = {
	{0x55,0x13,0x2D,0x0B},//a
	{0x54,0x12,0x2C,0x0A},//b
	{0x14,0x00,0x0C,0x00},//c

	{0x00,0x47,0x00,0x27},//aa
	{0x00,0x46,0x00,0x26},//bb
	{0x00,0x06,0x00,0x06},//cc

	{0x79,0x19,0x00,0x00},//a_a
	{0x59,0x00,0x00,0x00},//a_c
	{0x78,0x18,0x00,0x00},//b_b
	{0x58,0x00,0x00,0x00},//b_c
	{0x39,0x00,0x00,0x00},//c_a
	{0x38,0x00,0x00,0x00},//c_b
	{0x18,0x00,0x00,0x00},//c_c

	{0x00,0x4D,0x00,0x00},//aa_a
	{0x00,0x4C,0x00,0x00},//bb_b
	{0x00,0x0D,0x00,0x00},//cc_a
	{0x00,0x0C,0x00,0x00},//cc_b

	{0x00,0x75,0x00,0x00},//a_aa
	{0x00,0x55,0x00,0x00},//a_cc
	{0x00,0x74,0x00,0x00},//b_bb
	{0x00,0x54,0x00,0x00},//b_cc

	{0x00,0x65,0x00,0x00},//aa_aa
	{0x00,0x45,0x00,0x00},//aa_cc
	{0x00,0x64,0x00,0x00},//bb_bb
	{0x00,0x44,0x00,0x00},//bb_cc
	{0x00,0x25,0x00,0x00},//cc_aa
	{0x00,0x24,0x00,0x00},//cc_bb
	{0x00,0x04,0x00,0x00} //cc_cc
};

const uint8_t audctl_mask_on[28][4] PROGMEM = {
	{0x00,0x00,0x00,0x00},//a
	{0x01,0x01,0x01,0x01},//b 
	{0x40,0x00,0x20,0x00},//c

	{0x00,0x10,0x00,0x08},//aa
	{0x00,0x11,0x00,0x09},//bb
	{0x00,0x50,0x00,0x28},//cc

	{0x04,0x02,0x00,0x00},//a_a 
	{0x24,0x00,0x00,0x00},//a_c 
	{0x05,0x03,0x00,0x00},//b_b 
	{0x25,0x00,0x00,0x00},//b_c 
	{0x44,0x00,0x00,0x00},//c_a 
	{0x45,0x00,0x00,0x00},//c_b 
	{0x64,0x00,0x00,0x00},//c_c

	{0x00,0x12,0x00,0x00},//aa_a
	{0x00,0x13,0x00,0x00},//bb_b
	{0x00,0x52,0x00,0x00},//cc_a
	{0x00,0x53,0x00,0x00},//cc_b

	{0x00,0x0A,0x00,0x00},//a_aa
	{0x00,0x2A,0x00,0x00},//a_cc
	{0x00,0x0B,0x00,0x00},//b_bb
	{0x00,0x2B,0x00,0x00},//b_cc

	{0x00,0x1A,0x00,0x00},//aa_aa
	{0x00,0x3A,0x00,0x00},//aa_cc
	{0x00,0x1B,0x00,0x00},//bb_bb
	{0x00,0x3B,0x00,0x00},//bb_cc
	{0x00,0x5A,0x00,0x00},//cc_aa
	{0x00,0x5B,0x00,0x00},//cc_bb
	{0x00,0x7A,0x00,0x00} //cc_cc
};

void ChipInit()
{
	//clock signal
	DDRB |= _BV(7);

	//clock timer
	//timer 0, clock wave generator
	TCCR0A = _BV(COM0A0) | _BV(WGM01);
	TCCR0B |= _BV(CS00);
	OCR0A = 3;

	//port direction settings
	DataPortOut();

	//chip select port
	DDRF |= _BV(4)|_BV(5)|_BV(6)|_BV(7);

	//address port
	DDRB |= _BV(4)|_BV(5)|_BV(6);
	DDRC |= _BV(6);

	//rw signal port
	DDRF |= _BV(1);

	//initial port values
	AllChipOff();

	RW_Write();

	for (uint8_t chip = 0; chip < MAX_CHIPS; chip++)
		ChipReset(chip);

	for (uint8_t i = 0; i < MAX_VOICES; i++)
	{
		voice_map[i].chip = i/4;
		voice_map[i].address = (i%4)*2;
		POKEY_OscMode(i, 0);
		POKEY_Distortion(i, 0);
	}
}

void ChipReset(uint8_t chip)
{
	for (uint8_t reg = 0; reg < MAX_ADDRESS; reg++)
	{
		register_cache[chip][reg] = 0xFF;
		ChipWrite(chip, reg, 0x00);
	}

	ChipWrite(chip, SKCTL, 3);
	ChipWrite(chip, AUDC1, 0xA0);
	ChipWrite(chip, AUDC2, 0xA0);
	ChipWrite(chip, AUDC3, 0xA0);
	ChipWrite(chip, AUDC4, 0xA0);
}

/// To write to a POKEY chip the following procedure is required:
/// - set write signal (switched on by default)
/// - set address bus
/// - set data bus
/// - set chip
/// - wait for address and data to be written: the worst case scenario = 16 cycles
/// - set off all chips
///
/// Timing: 118 cycles in O2.
/// Manual optimization (OPTIMIZE_ME): 94 cycles. With 2 MHz clock this gives around 5.9 us.
void ChipWrite(uint8_t chip, uint16_t address, uint8_t data)
{
	if ((chip < MAX_CHIPS) && (address < MAX_ADDRESS))
	{
	#ifdef OPTIMISE_ME
		uint8_t x;
		uint8_t y;

		//SetAddress(address);
		x = PORTB;
		x &= 0x8F;
		y = address<<4;
		y &= 0x70;
		x |= y;
		PORTB = x;

		x = PORTC;
		x &= 0xBF;
		y = (address<<4);
		y &= 0x80;
		y >>= 1;
		x |= y;
		PORTC = x;

		//SetData(data);
		x = PORTD;
		x &= 0x0C;
		y = data&0x03;
		x |= y;
		y = data<<2;
		y &= 0xF0;
		x |= y;
		PORTD = x;
		
		x = PORTE;
		x &= 0xBB;
		y = data&0x40;
		y = y >> 4;
		x |= y;
		y = data&0x80;
		y = y >> 1;
		x |= y;
		PORTE = x;

	#else
		SetAddress(address);
		SetData(data);
	#endif

		ChipSelect(chip);

		//takes around 12 cycles
		register_cache[chip][address] = data;

		//add 4 empty cycles to be on the safe side
		asm volatile ("nop\n nop\n nop\n nop\n"::);

		//switch all chips off
		AllChipOff();
	}
}

/// Pokey does not allow for reading the value of its registers.
/// Therefore this function implements reading from the register cache.
uint8_t ChipRead(uint8_t chip, uint16_t address)
{
	if ((chip < MAX_CHIPS) && (address < MAX_ADDRESS))
		return register_cache[chip][address];
	else
		return 0;

/* Reading Pokey registers returns values for IO ports etc.
// See the specification document for more details.
	
	if ((chip < MAX_CHIPS) && (address < MAX_ADDRESS))
	{
		RW_Read();

		DataPortIn();

		SetAddress(address);

		ChipSelect(chip);

		while (bit_is_clear(CLK_PORT,CLK_PIN)) ;
		while (bit_is_set(CLK_PORT,CLK_PIN)) ;
		while (bit_is_clear(CLK_PORT,CLK_PIN)) ;
		while (bit_is_set(CLK_PORT,CLK_PIN)) ;

		register_cache[chip][address] = ReadDataPort();

		AllChipOff();

		DataPortOut();

		RW_Write();
	
		return register_cache[chip][address];
	}

	return 0;
*/
}

uint8_t ChipReadCache(uint8_t chip, uint16_t address)
{
	if ((chip < MAX_CHIPS) && (address < MAX_ADDRESS))
		return register_cache[chip][address];
	else
		return 0;
}

#ifdef POKEY_SYNTH
/// Scale down the volume value to 4 bit and update POKEY register.
/// Timing: 36 + 94 (ChipWrite) = 130 cycles in O2 and OPTIMIZE_ME.
void VoiceVolume(uint8_t voice, uint8_t value)
{
	if (voice < MAX_VOICES)
	{
		uint8_t chip = voice/4;
		uint8_t reg = (voice % 4)*2 + 1; 
		value /= 8;
		value |= (register_cache[chip][reg] & 0xF0);
		ChipWrite(chip, reg, value);
	}
}

/// Set the frequency divider. The function first calculates the closest whole note and then
/// finds a corresponding register value in LUT. If the note has a remainder then a proportion of 
/// a difference between the note and the next one is added.
/// There is a separate LUT for three different base frequency and oscillator modes:
/// - 0: 8-bit base_frequency/28
/// - 1: 8-bit base_frequency/114
/// - 2: 8-bit base_frequency
/// - 3: 16-bit base_frequency/28
/// - 4: 16-bit base_frequency/114
/// - 5: 16-bit base_frequency
///
/// Timing for 0/1: 100 + 94 (ChipWrite) = 194 cycles in O2 and OPTIMIZE_ME.
/// Timing for 0/1 and remainder: 135 + 94 (ChipWrite) = 229 cycles in O2 and OPTIMIZE_ME.
/// Timing for 3: 97 + 2*94 (ChipWrite) = 285 cycles in O2 and OPTIMIZE_ME.
/// Timing for 3 and remainder: 129 + 2*94 (ChipWrite) = 317 cycles in O2 and OPTIMIZE_ME.

void VoiceNote(uint8_t voice,  int16_t note)
{
	if (voice >= MAX_VOICES) return;

	if (note < 0) note = 0;
	else if (note > (127<<7)) note = 127<<7;

	PokeyVoice *v = &voice_map[voice];

	uint8_t mask = pgm_read_byte(&config_mask[osc_mode[voice]][v->address>>2]);
	if ((v->address == 0) || (v->address == 4))
		mask >>= 4;
	else
		mask &= 0x0F;

	const uint16_t* note_address;
	switch (mask)
	{
	case 0: case 3:
		note_address = &key_freq_3[note>>7];
		break;
	case 1: case 4:
		note_address = &key_freq_2[note>>7];
		break;
	case 2: case 5:
		note_address = &key_freq_1[note>>7];
		break;
	default:
		return;
	}

	uint16_t value = pgm_read_word(note_address);
	
	uint8_t reminder = note & 0x7F;
	if (reminder)
		value -= ((value - pgm_read_word(note_address+1))*reminder)>>7;

	if (mask < 3)
	{
		if (value > 0xFF) value = 0xFF;
		ChipWrite(v->chip, v->address, (uint8_t)(value));
	}
	else
	{
		if ((register_cache[v->chip][v->address+1] & 0xF0) == 0xC0)
		{
			uint8_t i = value % 15;
			value += lfsr[i];
			ChipWrite(v->chip, v->address, (uint8_t)(value>>8));
			ChipWrite(v->chip, v->address-2, (uint8_t)(value));
			ChipWrite(v->chip, 9, 0);
		}
		else
		{
			ChipWrite(v->chip, v->address, (uint8_t)(value>>8));
			ChipWrite(v->chip, v->address-2, (uint8_t)(value));
		}
	}
}

#endif

/// Pokey has an unusual way of configuring different oscillators.
/// It is possible to set base frequency divider and high-pass filter;  
/// however not for all oscillators. Not all setups make practical sense:
/// for example it is always better to use setup 5 than 3 or 4.
///
/// The following oscillator setups are possible:
/// - 0: 8-bit divider, base freq/28, filter none
/// - 1: 8-bit divider, base freq/114, filter none
/// - 2: 8-bit divider, base freq, filter none
/// - 3: 16-bit divider, base freq/28, filter none
/// - 4: 16-bit divider, base freq/114, filter none
/// - 5: 16-bit divider, base freq, filter none
/// - 6: 8-bit divider, base freq/28, filter 8-bit/28
/// - 7: 8-bit divider, base freq/28, filter 8-bit/1
/// - 8: 8-bit divider, base freq/114, filter 8-bit/114
/// - 9: 8-bit divider, base freq/114, filter 8-bit/1
/// - 10: 8-bit divider, base freq, filter 8-bit/28
/// - 11: 8-bit divider, base freq, filter 8-bit/114
/// - 12: 8-bit divider, base freq, filter 8-bit/1
/// - 13: 16-bit divider, base freq/28, filter 8-bit/28
/// - 14: 16-bit divider, base freq/114, filter 8-bit/114
/// - 15: 16-bit divider, base freq, filter 8-bit/28
/// - 16: 16-bit divider, base freq, filter 8-bit/114
/// - 17: 8-bit divider, base freq/28, filter 16-bit/28
/// - 18: 8-bit divider, base freq/28, filter 16-bit/1
/// - 19: 8-bit divider, base freq/114, filter 16-bit/114
/// - 20: 8-bit divider, base freq/114, filter 16-bit/1
/// - 21: 16-bit divider, base freq/28, filter 16-bit/28
/// - 22: 16-bit divider, base freq/28, filter 16-bit/1
/// - 23: 16-bit divider, base freq/114, filter 16-bit/114
/// - 24: 16-bit divider, base freq/114, filter 16-bit/1
/// - 25: 16-bit divider, base freq, filter 16-bit/28
/// - 26: 16-bit divider, base freq, filter 16-bit/114
/// - 27: 16-bit divider, base freq, filter 16-bit
void POKEY_OscMode(uint8_t voice, uint8_t value)
{
	if (value < 28)
	{
		osc_mode[voice] = value;
		PokeyVoice *v = &voice_map[voice];

		uint8_t audctl = register_cache[v->chip][AUDCTL];
		uint8_t address = v->address>>1;
		audctl &= ~pgm_read_byte(&audctl_mask_off[value][address]);
		audctl |= pgm_read_byte(&audctl_mask_on[value][address]);
//		if (register_cache[voice_map[voice].chip][AUDCTL] != audctl)
		ChipWrite(v->chip, AUDCTL, audctl);
	}
}

/// The square wave sound in Pokey can be distorted in various way:
/// - 0: pure sound AUDFx/2
/// - 1: 17/5/AUDFx 
/// - 2: 9/5/AUDFx
/// - 3: 17/AUDFx
/// - 4: 9/AUDFx
/// - 5: 5/AUDFx/2
/// - 6: 4/5/AUDFx
/// - 7: 4/AUDFx
/// - 8: Volume Direct
void POKEY_Distortion(uint8_t voice, uint8_t value)
{
	if (value < 9)
	{
		PokeyVoice *v = &voice_map[voice];
		ChipWrite(v->chip, v->address+1, (register_cache[v->chip][v->address+1]&0x0F) | pgm_read_byte(&distortion_type[value]));

		uint8_t audctl = register_cache[v->chip][AUDCTL];
		if ((value == 2) || (value == 4))
			audctl |= 0x80;
		else
			audctl &= ~0x80;
		ChipWrite(v->chip, AUDCTL, audctl);
	}
}

/// Value of the filter is set only if the oscillator is set to a filter mode.
/// If the filter is 8-bit then the value is rescaled down to 8-bit.
void POKEY_Filter(uint8_t voice, uint16_t value)
{
	uint8_t mode = osc_mode[voice];
	PokeyVoice *v = &voice_map[voice];

	if (v->address == 0)
	{
		if ((mode >= 6) && (mode < 13))
			ChipWrite(v->chip, AUDF3, value>>8);
	}
	else if (v->address == 2)
	{
		if ((mode == 6) || (mode == 8) || ((mode >= 13) && (mode < 28)))
		{
			ChipWrite(v->chip, AUDF4, value>>8);
			if (mode >= 17)
				ChipWrite(v->chip, AUDF3, value&0xFF);
		}
	}
}

