#include "MidiSynth.h"
#include <string.h>

uint8_t program[MAX_MIDI_CHANNELS];
uint16_t bank[MAX_MIDI_CHANNELS];
uint16_t nrpn[MAX_MIDI_CHANNELS];
uint16_t rpn[MAX_MIDI_CHANNELS];
uint16_t param_data[MAX_MIDI_CHANNELS];

uint8_t volume[MAX_MIDI_CHANNELS];
uint8_t expression[MAX_MIDI_CHANNELS];
uint8_t channel_volume[MAX_MIDI_CHANNELS];

int16_t pitchwheel[MAX_MIDI_CHANNELS];
int8_t coarse_tuning[MAX_MIDI_CHANNELS];
int16_t fine_tuning[MAX_MIDI_CHANNELS];
int16_t channel_pitch[MAX_MIDI_CHANNELS];

Key key[MAX_MIDI_CHANNELS][MAX_KEYS];
Instrument instrument[MAX_MIDI_CHANNELS];

#ifdef POKEY_SYNTH
void ChipSynthInit()
{
	ChipInit();
}
void VoiceRelease(uint8_t channel, uint8_t voice) {}
uint8_t VoiceAssign(uint8_t channel) { return 0xFF; }
void ChipProgramChange(uint8_t channel) {}
void ChipSetNrpn(uint8_t channel, uint16_t type, uint16_t value) {}
#endif

//set of dummy functions - to be implemented for each sound chip
#ifndef POKEY_SYNTH
#ifndef AY_SYNTH

uint8_t VoiceAssign(uint8_t channel) { return 0xFF; }
void VoiceRelease(uint8_t channel, uint8_t voice) {}
void ChipSynthInit() {}
void ChipProgramChange(uint8_t channel) {}
void ChipSetNrpn(uint8_t channel, uint16_t type, uint16_t value) {}
void VoiceVolume(uint8_t voice, uint8_t volume) {};
void VoiceNote(uint8_t voice, int16_t note) {};
#endif
#endif

void SynthInit()
{
	for (uint8_t i = 0; i < MAX_MIDI_CHANNELS; i++)
	for (uint8_t j = 0; j < MAX_KEYS; j++)
	{
		key[i][j].note = 0xFF;
		key[i][j].voice = 0xFF;
	}

	ResetAllControllers();
	
	for (uint8_t i = 0; i < MAX_MIDI_CHANNELS; i++)
	{
		program[i] = 0x7F;
		ProgramChange(i, 0);

		Volume(i, 0x7F);
		FineTuning(i, 0x40, 0x00);
		CoarseTuning(i, 0x40);
	}		

	ChipSynthInit();
}

void NoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
	Key *k = &key[channel][0];
	
	for (uint8_t i = 0; i < MAX_KEYS; i++)
	{
		if (k->note == note)
		{
			if (k->voice != 0xFF)
			{
				VoiceVolume(k->voice, 0);
				VoiceRelease(channel, k->voice);
			}

			for (uint8_t j = i; j < MAX_KEYS-1; j++)
			{
				*k  = *(k+1);
				k++;
			}
			
			k->note = 0xFF;
			k->voice = 0xFF;
			break;
		}
		k++;
	}
}

void NoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
	if (velocity)
	{
		Key *k = &key[channel][MAX_KEYS-1];
		
		//handle the last key before erasure 
		if (k->note != 0xFF)
		{
			VoiceVolume(k->voice, 0);
			VoiceRelease(channel, k->voice);
		}

		//propagate keys in the memory
		for (uint8_t i = MAX_KEYS-1; i > 0; i--)
		{
			*k = *(k-1);
			k--;
		}

		//assign the first key
		k->note = note;
		k->velocity = velocity;
		k->voice = VoiceAssign(channel);

		if (k->voice != 0xFF)
		{
			VoiceNote(k->voice, channel_pitch[channel]+(note << 7));
			VoiceVolume(k->voice, channel_volume[channel]*velocity >> 7);
		}
	}
	else
		NoteOff(channel, note, velocity);
}

/* Not implemented
void KeyAftertouch(uint8_t channel, uint8_t note, uint8_t value) {}
*/

void RefreshVolume(uint8_t channel)
{
	uint8_t cv = volume[channel]*expression[channel] >> 7;
	cv = cv*instrument[channel].volume >> 7;
	channel_volume[channel] = cv;

	Key *k = &key[channel][0];
	for (uint8_t i = 0; i < MAX_KEYS; i++)
	{
		if (k->voice != 0xFF)
			VoiceVolume(k->voice, cv*k->velocity >> 7);
		k++;
	}
}

void Volume(uint8_t channel, uint8_t value)
{
	if (volume[channel] != value)
	{
		if (value)
			++value;
		volume[channel] = value;
		RefreshVolume(channel);
	}
}

void Expression(uint8_t channel, uint8_t value)
{
	if (expression[channel] != value)
	{
		if (value)
			++value;
		expression[channel] = value;
		RefreshVolume(channel);
	}
}

void RefreshPitch(uint8_t channel)
{
	Instrument *instr =  &instrument[channel];
	
	int16_t cp = (int32_t)pitchwheel[channel]*instr->pitchwheel_range >> 13;
	cp += coarse_tuning[channel] << 7;
	cp += fine_tuning[channel] >> 6;
	cp += instr->coarse_tuning << 7;
	cp += instr->fine_tuning >> 6;
	channel_pitch[channel] = cp;

	Key *k = &key[channel][0];
	for (uint8_t i = 0; i < MAX_KEYS; i++)
	{
		if (k->voice != 0xFF)
			//VoiceNote(k->voice, cp+(k->note << 7));
		k++;
	}
}

void FineTuning(uint8_t channel, uint8_t value_msb, uint8_t value_lsb)
{	
	fine_tuning[channel] = ((value_msb << 7) | value_lsb) - 0x2000;
	if (fine_tuning[channel] > 0)
		++fine_tuning[channel];
	RefreshPitch(channel);
}

void CoarseTuning(uint8_t channel, uint8_t value)
{
	coarse_tuning[channel] = value - 0x40;
	RefreshPitch(channel);
}

/* Not implemented
void ChannelAftertouch(uint8_t channel, uint8_t value) {}
*/

void PitchWheel(uint8_t channel, uint8_t value_msb, uint8_t value_lsb)
{
	pitchwheel[channel] = ((value_msb << 7) | value_lsb) - 0x2000;
	if (pitchwheel[channel] > 0)
		++pitchwheel[channel];
	RefreshPitch(channel);
}

void BankLsb(uint8_t channel, uint8_t value)
{
	bank[channel] = (bank[channel] & ~0x7F) | value;
}

void BankMsb(uint8_t channel, uint8_t value)
{
	bank[channel] = (bank[channel] & 0x7F) | (value << 7);
}

inline void SetRpn(uint8_t channel, uint16_t type, uint16_t value)
{
	if (type == 0)
		InstPitchWheelRange(channel, value & 0x7F, value >> 7);
	else if (type == 1)
		FineTuning(channel, value & 0x7F, value >> 7);
	else if (type == 2)
		CoarseTuning(channel, value >> 7);
}

inline void SetNrpn(uint8_t channel, uint16_t type, uint16_t value)
{
	if (type == 0x11)
		InstPitchWheelRange(channel, value & 0x7F, value >> 7);
	else if (type == 0x20)
		InstVolume(channel, value >> 7);
	else if (type == 0x21)
		InstFineTuning(channel, value & 0x7F, value >> 7);
	else if (type == 0x22)
		InstCoarseTuning(channel, value >> 7);
	else
		ChipSetNrpn(channel, type, value);
}

void UpdateParamData(uint8_t channel)
{
	if (rpn[channel] != 0x3FFF)
		SetRpn(channel, rpn[channel], param_data[channel]);
	else if (nrpn[channel] != 0x3FFF)
		SetNrpn(channel, nrpn[channel], param_data[channel]);
}

void DataEntryLsb(uint8_t channel, uint8_t value)
{
	param_data[channel] = (param_data[channel] & ~0x7F) | value;
	UpdateParamData(channel);
}

void DataEntryMsb(uint8_t channel, uint8_t value)
{
	param_data[channel] = (param_data[channel] & 0x7F) | (value << 7);
	UpdateParamData(channel);
}

void DataInc(uint8_t channel)
{
	if (param_data[channel] < 0x3FFF)
		++param_data[channel];
	UpdateParamData(channel);
}

void DataDec(uint8_t channel)
{
	if (param_data[channel])
		--param_data[channel];
	UpdateParamData(channel);
}

void NrpnLsb(uint8_t channel, uint8_t value)
{
	rpn[channel] = 0x3FFF;
	nrpn[channel] = (nrpn[channel] & ~0x7F) | value;
}

void NrpnMsb(uint8_t channel, uint8_t value)
{
	rpn[channel] = 0x3FFF;
	nrpn[channel] = (nrpn[channel] & 0x7F) | (value << 7);
}

void RpnLsb(uint8_t channel, uint8_t value)
{
	nrpn[channel] = 0x3FFF;
	rpn[channel] = (rpn[channel] & ~0x7F) | value;
}

void RpnMsb(uint8_t channel, uint8_t value)
{
	nrpn[channel] = 0x3FFF;
	rpn[channel] = (rpn[channel] & 0x7F) | (value << 7);
}

void AllSoundsOff()
{
	AllNotesOff();

	for (uint8_t i = 0; i < MAX_MIDI_CHANNELS; i++)
	for (uint8_t j = 0; j < MAX_KEYS; j++)
		if (key[i][j].voice != 0xFF)
			VoiceVolume(key[i][j].voice, 0);
}

void ResetAllControllers()
{
/*
    Set Expression (#11) to 127
    Set Modulation (#1) to 0
    Set Pedals (#64, #65, #66, #67) to 0
    Set Registered and Non-registered parameter number LSB and MSB (#98-#101) to null value (127)
    Set pitch bender to center (64/0)
    Reset channel pressure to 0 
    Reset polyphonic pressure for all notes to 0.
    Do NOT reset Bank Select (#0/#32)
    Do NOT reset Volume (#7)
    Do NOT reset Pan (#10)
    Do NOT reset Program Change
    Do NOT reset Effect Controllers (#91-#95)
    Do NOT reset Sound Controllers (#70-#79) 
    Do NOT reset other channel mode messages (#120-#127)
    Do NOT reset registered or non-registered parameters.
*/	
	for (uint8_t i = 0; i < MAX_MIDI_CHANNELS; i++)
	{
		Expression(i, 0x7F);
		PitchWheel(i, 0x40, 0x00);
		RpnLsb(i, 0x7F);
		RpnMsb(i, 0x7F);
		NrpnLsb(i, 0x7F);
		NrpnMsb(i, 0x7F);
	}
}

void AllNotesOff()
{
	for (uint8_t i = 0; i < MAX_MIDI_CHANNELS; i++)
	for (uint8_t j = 0; j < MAX_KEYS; j++)
		if (key[i][j].note != 0xFF)
			NoteOff(i, key[i][j].note, 0);
}

void ProgramChange(uint8_t channel, uint8_t value)
{
	if (program[channel] != value)
	{
		program[channel] = value;

		instrument[channel].pitchwheel_range = 2<<7;
		instrument[channel].volume = 0x80;
		instrument[channel].coarse_tuning = 0;
		instrument[channel].fine_tuning = 0;

		ChipProgramChange(channel);

//		ReadInstrument(channel,value);
	}
}

void InstVolume(uint8_t channel, uint8_t value)
{
	if (value) ++value;
	
	if (instrument[channel].volume != value)
	{
		instrument[channel].volume = value;
		RefreshVolume(channel);
	}
}

void InstPitchWheelRange(uint8_t channel, uint8_t value_msb, uint8_t value_lsb)
{
	uint16_t value = (value_msb << 7) | value_lsb;
	
	if (instrument[channel].pitchwheel_range != value)
	{
		instrument[channel].pitchwheel_range = value;
		RefreshPitch(channel);		
	}
}

void InstFineTuning(uint8_t channel, uint8_t value_msb, uint8_t value_lsb)
{
	int16_t value = ((value_msb << 7) | value_lsb) - 0x2000;
	if (value)	++value;

	if (instrument[channel].fine_tuning != value)
	{
		instrument[channel].fine_tuning = value;
		RefreshPitch(channel);
	}
}

void InstCoarseTuning(uint8_t channel, uint8_t value)
{
	value -= 0x40;

	if (instrument[channel].coarse_tuning != value)
	{
		instrument[channel].coarse_tuning = value;
		RefreshPitch(channel);
	}
}
