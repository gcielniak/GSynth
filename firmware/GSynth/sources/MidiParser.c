#include <avr/io.h>
#include "SynthConfig.h"
#include "MCU.h"
#include "Comm.h"
#include "UserMemory.h"
#include "MidiSynth.h"

#define AckMsg(value) (value & 0x40)
#define SynthResetMsg(value) ((value & 0x3F) == 0x00)
#define SynthNameMsg(value) (value == 0x41)
#define GetClockDivMsg(value) (value == 0x42)
#define SetClockDivMsg(value) ((value & 0x3F) == 0x03)
#define ChipResetMsg(value) ((value & 0x3F) == 0x04)
#define ChipReadMsg(value) (value == 0x45)
#define ChipWriteMsg(value) ((value & 0x3F) == 0x06)
#define ChipNrMsg(value) ((value & 0x3F) == 0x07)
#define ChipAddressMsg(value) ((value & 0x3F) == 0x08)
#define UserMemReadMsg(value) (value == 0x49)
#define UserMemWriteMsg(value) ((value & 0x3F) == 0x0A)
#define UserMemAddressMsg(value) ((value & 0x3F) == 0x0B)

void ProcessShort(uint8_t *message, uint8_t *message_length);
void ProcessSysex(uint8_t *message, uint8_t *length);

#ifdef POKEY_SYNTH
const uint8_t synth_name[] PROGMEM = {0xF0,0x7D,0x41,'P','O','K','E','Y','S','y','n','t','h',' ','1','.','7',0xF7};
#elif defined SID_SYNTH
const uint8_t synth_name[] PROGMEM = {0xF0,0x7D,0x41,'S','I','D','S','y','n','t','h',' ','1','.','7',0xF7};
#elif defined YM_SYNTH
const uint8_t synth_name[] PROGMEM = {0xF0,0x7D,0x41,'Y','M','S','y','n','t','h',' ','1','.','7',0xF7};
#elif defined AY_SYNTH
const uint8_t synth_name[] PROGMEM = {0xF0,0x7D,0x41,'A','Y','S','y','n','t','h',' ','1','.','7',0xF7};
#else
const uint8_t synth_name[] PROGMEM = {0xF0,0x7D,0x41,'G','S','y','n','t','h',' ','1','.','7',0xF7};
#endif

void ProcessMidiData(uint8_t data, uint8_t *message, uint8_t *message_length)
{
	if ((data < 0xF8) && (data > 0x7F)) //status byte - ignore real-time messages
	{
		message[0] = data;
		*message_length = 1;
	}
	else if (data < 0x80) //data byte
	{
		if (*message_length < MAX_MESSAGE_LENGTH)
		{
			message[(*message_length)++] = data;
			if (message[0] == 0xF0)
				ProcessSysex(message, message_length);
			else
				ProcessShort(message, message_length);
		}		
	}
}

void ProcessShort(uint8_t *message, uint8_t *message_length)
{
	uint8_t channel = message[0] & 0x0F;
	uint8_t command = message[0] >> 4;

	if (*message_length == 2)
	{
		if ((command) == 0xC)
		{
			ProgramChange(channel, message[1]);
			*message_length = 1;
		}
		else if ((command) == 0xD)
		{
;//			ChannelAftertouch(channel, message[1]);
			*message_length = 1;
		}
	}
	else if (*message_length == 3)
	{
		if (channel >= MAX_MIDI_CHANNELS) return;

		if (command == 0x8)
			NoteOff(channel, message[1], message[2]);
		else if (command == 0x9)
			NoteOn(channel, message[1], message[2]);
		else if (command == 0xA)
;//			KeyAftertouch(channel, message[1], message[2]);
		else if (command == 0xB)
		{
			if (message[1] == 0x07)
				Volume(channel, message[2]);
			else if (message[1] == 0x0B)
				Expression(channel, message[2]);
			else if (message[1] == 0x00)
				BankMsb(channel, message[2]);
			else if (message[1] == 0x20)
				BankLsb(channel, message[2]);
			else if (message[1] == 0x06)
				DataEntryMsb(channel, message[2]);
			else if (message[1] == 0x26)
				DataEntryLsb(channel, message[2]);
			else if (message[1] == 0x60)
				DataInc(channel);
			else if (message[1] == 0x61)
				DataDec(channel);
			else if (message[1] == 0x62)
				NrpnLsb(channel, message[2]);
			else if (message[1] == 0x63)
				NrpnMsb(channel, message[2]);
			else if (message[1] == 0x64)
				RpnLsb(channel, message[2]);
			else if (message[1] == 0x65)
				RpnMsb(channel, message[2]);
			else if (message[1] == 0x78)
				AllSoundsOff();
			else if (message[1] == 0x79)
				ResetAllControllers();
			else if (message[1] == 0x7B)
				AllNotesOff();
			else if (message[1] == 0x0A)
;//				FineTuning(channel, message[2], 0);
			else if (message[1] == 0x0B)
;//				CoarseTuning(channel, message[2]);
			else if (message[1] == 0x0C)
;//				InstFineTuning(channel, message[2], 0);
			else if (message[1] == 0x0D)
;//				InstCoarseTuning(channel, message[2]);
			else if (message[1] == 0x0E)
;//				InstPitchWheelRange(channel, message[2], 0);
		}
		else if (command == 0xE)
			PitchWheel(channel, message[2], message[1]);
		*message_length = 1;
	}
}

void ProcessSysex(uint8_t *message, uint8_t *message_length)
{
	if ((*message_length > 2) && (message[1] == VENDOR_ID))
	{
		uint8_t reply_sysex_length = 0;

		if (*message_length == 3)
		{
			//chip reset
			if (SynthResetMsg(message[2]))
			{
				SynthInit();
				if (AckMsg(message[2]))	reply_sysex_length = 1;
			}
			else if (SynthNameMsg(message[2]))
			{
				SendP(synth_name, sizeof(synth_name));
			}
			else if (GetClockDivMsg(message[2]))
			{
				message[3] = GetClockDiv();
				reply_sysex_length = 2;
			}
			else if (ChipNrMsg(message[2]))
			{
				message[3] = MAX_CHIPS;
				reply_sysex_length = 2;
			}
			else if (ChipAddressMsg(message[2]))
			{
				message[3] = (MAX_ADDRESS<<7)&0x7F;
				message[4] = MAX_ADDRESS&0x7F;
				reply_sysex_length = 3;
			}
			else if (UserMemAddressMsg(message[2]))
			{
				message[3] = (EEPROM_SIZE>>7)&0x7F;
				message[4] = EEPROM_SIZE&0x7F;
				reply_sysex_length = 3;
			}
		}
		else if (*message_length == 4)
		{
			if (ChipResetMsg(message[2]))
			{
				ChipReset(message[3]&0x7);
				if (AckMsg(message[2]))	reply_sysex_length = 2;
			}
			else if (SetClockDivMsg(message[2]))
			{
				SetClockDiv(message[3]);
				if (AckMsg(message[2]))	reply_sysex_length = 2;
			}
		}			
		else if (*message_length == 5)
		{
			if (ChipReadMsg(message[2]))
			{
				message[5] = ChipRead((message[3]>>3)&0x7, ((message[3]&0x7)<<7)|message[4]);
				message[3] |= (message[5]&0x80)>>1;
				message[5] &= 0x7F;
				reply_sysex_length = 4;
			}
			else if (UserMemReadMsg(message[2]))
			{
				message[5] = UserMemRead(((message[3]&0x3F)<<7)|message[4]);
				message[3] |= (message[5]&0x80)>>1;
				message[5] &= 0x7F;
				reply_sysex_length = 4;
			}
		}			
		else if (*message_length == 6)
		{
			if (ChipWriteMsg(message[2]))
			{
				ChipWrite((message[3]>>3)&0x7, ((message[3]&0x7)<<7)|message[4], ((message[3]<<1)&0x80)|message[5]);
				if (AckMsg(message[2]))	reply_sysex_length = 4;
			}
			else if (UserMemWriteMsg(message[2]))
			{
				UserMemWrite(((message[3]&0x3F)<<7)|message[4], ((message[3]<<1)&~0x7F)|message[5]);
				if (AckMsg(message[2]))	reply_sysex_length = 4;
			}
		}			
			
		if (reply_sysex_length)
			SendSysex(&message[2], reply_sysex_length);
	}
}
