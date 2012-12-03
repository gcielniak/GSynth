/** \file MidiSynth.h
 *  Implementation of the generic MIDI synth.
 *
 *  \author Grzegorz Cielniak
 *  \date 07/09/2011
 */ 

#ifndef _MIDISYNTH_H_
#define _MIDISYNTH_H_

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "MCU.h"
#include "SynthConfig.h"

#ifdef POKEY_SYNTH
//#include "PokeySynth.h"
#include "PokeyChip.h"
#elif defined SID_SYNTH
#include "SidChip.h"
#elif defined YM_SYNTH
#include "YmChip.h"
#elif defined AY_SYNTH
#include "AySynth.h"
#endif

struct Key
{
	uint8_t note;
	uint8_t velocity;
	uint8_t voice;
};
typedef struct Key Key;

struct Instrument
{
	uint16_t pitchwheel_range;
	uint8_t volume;
	int8_t coarse_tuning;
	int16_t fine_tuning;
};
typedef struct Instrument Instrument;

void SynthInit();
void NoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
void NoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
//void KeyAftertouch(uint8_t channel, uint8_t note, uint8_t data);

void Volume(uint8_t channel, uint8_t value);
void Expression(uint8_t channel, uint8_t value);
void PitchWheelRange(uint8_t channel, uint8_t value_msb, uint8_t value_lsb);
void FineTuning(uint8_t channel, uint8_t value_msb, uint8_t value_lsb);
void CoarseTuning(uint8_t channel, uint8_t value);

void ProgramChange(uint8_t channel, uint8_t data);
//void ChannelAftertouch(uint8_t channel, uint8_t data);
void PitchWheel(uint8_t channel, uint8_t value_msb, uint8_t value_lsb);
void BankLsb(uint8_t channel, uint8_t data);
void BankMsb(uint8_t channel, uint8_t data);
void SetRpn(uint8_t channel, uint16_t type, uint16_t data);
void SetNrpn(uint8_t channel, uint16_t type, uint16_t data);
void DataEntryLsb(uint8_t channel, uint8_t data);
void DataEntryMsb(uint8_t channel, uint8_t data);
void DataInc(uint8_t channel);
void DataDec(uint8_t channel);
void NrpnLsb(uint8_t channel, uint8_t data);
void NrpnMsb(uint8_t channel, uint8_t data);
void RpnLsb(uint8_t channel, uint8_t data);
void RpnMsb(uint8_t channel, uint8_t data);
void AllSoundsOff();
void ResetAllControllers();
void AllNotesOff();

void InstVolume(uint8_t channel, uint8_t value);
void InstPitchWheelRange(uint8_t channel, uint8_t value_l, uint8_t value_h);
void InstFineTuning(uint8_t channel, uint8_t value_l, uint8_t value_h);
void InstCoarseTuning(uint8_t channel, uint8_t value);

#endif /* _MIDISYNTH_H_ */
