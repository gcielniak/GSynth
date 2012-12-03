#pragma once

#include "ChipSynth.h"

namespace Synth
{
	//lookup table for distortion settings
	unsigned char distortion_type[] = {0xA0,0x00,0x00,0x80,0x80,0x60,0x40,0xC0,0xC0,0xF0};
	unsigned char dist_flag[] = {0x00,0x01,0x10,0x01,0x10,0x00,0x00,0x00,0x00,0x00};
	unsigned char dist_block[] = {0x00,0xF0,0x0F,0xF0,0x0F,0x00,0x00,0x00,0x00,0x00};

	char mute7B[5] = {1, 0, 0, -1, 2};
	char sharp7B[5] = {0, -1, 1, 0, 1};
	char dist7AC[15] = {1,0,0,1,0,2,1,0,0,2,1,0,1,0,0};

	//voice mask for different oscillator settings
	//the flag is set depending on the oscillator number and frequency divider
	//3210 - indicates a particular voice (or voices)
	//a - indicates freq_28 being active on that chip
	//b - indicates freq_114 being active on that chip
	//c_d - c: 8/16 bit, d: divide by 1, 28, 114
	//--ba3210
	unsigned char voice_mask[28][4] = {
		//x
		{0x11,0x12,0x14,0x18},
		{0x21,0x22,0x24,0x28},
		{0x01,0xFF,0x04,0xFF},
		//X
		{0xFF,0x13,0xFF,0x1C},
		{0xFF,0x23,0xFF,0x2C},
		{0xFF,0x03,0xFF,0x0C},
		//xx
		{0x15,0x1A,0xFF,0xFF},
		{0x15,0xFF,0xFF,0xFF},
		{0x25,0x2A,0xFF,0xFF},
		{0x25,0xFF,0xFF,0xFF},
		{0x15,0xFF,0xFF,0xFF},
		{0x25,0xFF,0xFF,0xFF},
		{0x05,0xFF,0xFF,0xFF},
		//Xx
		{0xFF,0x1B,0xFF,0xFF},
		{0xFF,0x2B,0xFF,0xFF},
		{0xFF,0x1B,0xFF,0xFF},
		{0xFF,0x2B,0xFF,0xFF},
		//xX
		{0xFF,0x1E,0xFF,0xFF},
		{0xFF,0x1E,0xFF,0xFF},
		{0xFF,0x2E,0xFF,0xFF},
		{0xFF,0x2E,0xFF,0xFF},
		//XX
		{0xFF,0x1F,0xFF,0xFF},
		{0xFF,0x1F,0xFF,0xFF},
		{0xFF,0x2F,0xFF,0xFF},
		{0xFF,0x2F,0xFF,0xFF},
		{0xFF,0x1F,0xFF,0xFF},
		{0xFF,0x2F,0xFF,0xFF},
		{0xFF,0x0F,0xFF,0xFF},
	};

	//lookup tables for AUDCTL register settings
	unsigned char audctl_mask_off[28][4] = {
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

	unsigned char audctl_mask_on[28][4] = {
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

	//lookup table for configuration masks - finding the voice settings
	unsigned char config_mask[28][2] = {
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

	ref class PokeySynth : ChipSynth
	{
		const static int MAX_CHIPS = 3;
		const static int MAX_ADDRESS = 0x10;
		const static int MAX_VOICES = MAX_CHIPS*4;
		static const double chip_freq = 2000000.0;
		const static unsigned char AUDCTL = 0x08;
		const static unsigned char AUDF3 = 0x04;
		const static unsigned char AUDF4 = 0x06;

		ref struct PokeyVoice
		{
			int chip;
			int address;	
		};

		ref struct POKEYInstrument
		{
			array<int>^ distortion;
			array<int>^ osc_mode;
			array<int>^ filter;
		};
		typedef ref struct POKEYInstrument POKEYInstrument;

		array<int>^ voices;
		array<int, 2>^ register_cache;
		array<PokeyVoice^>^ voice_map;
		array<unsigned char>^ voice_dist;
		array<POKEYInstrument^>^ pokey_instrument;
		array<int>^ osc_mode;
		array<unsigned char>^ chip_mask;
		array<unsigned char>^ ext_mask;
		array<unsigned char>^ dist_mask;

	public:
		PokeySynth()
		{
			voices = gcnew array<int>(MAX_VOICES);
			osc_mode = gcnew array<int>(MAX_VOICES);
			chip_mask = gcnew array<unsigned char>(MAX_CHIPS);
			ext_mask = gcnew array<unsigned char>(MAX_CHIPS);
			dist_mask = gcnew array<unsigned char>(MAX_CHIPS);
			register_cache = gcnew array<int, 2>(MAX_CHIPS,MAX_ADDRESS);
			voice_dist = gcnew array<unsigned char>(MAX_VOICES);
			voice_map = gcnew array<PokeyVoice^>(MAX_VOICES);
			for (int i = 0; i < MAX_VOICES; i++)
				voice_map[i] = gcnew PokeyVoice();

			pokey_instrument = gcnew array<POKEYInstrument^>(MAX_MIDI_CHANNELS);
			for (int i = 0; i < MAX_MIDI_CHANNELS; i++)
			{
				pokey_instrument[i] = gcnew POKEYInstrument();
				for (int j = 0; j < MAX_LAYERS; j++)
				{
					pokey_instrument[i]->osc_mode = gcnew array<int>(MAX_LAYERS);
					pokey_instrument[i]->distortion = gcnew array<int>(MAX_LAYERS);
					pokey_instrument[i]->filter = gcnew array<int>(MAX_LAYERS);
				}
			}
		}

		virtual void Init() override
		{
			ChipSynth::Init();

			//pokey
			for (int i = 0; i < MAX_VOICES; i++)
			{
				voices[i] = 0;
				voice_dist[i] = 0;
				voice_map[i]->chip = i/4;
				voice_map[i]->address = (i%4)*2;
			}

			for (int i = 0; i < MAX_CHIPS; i++)
				for (int j = 0; j < MAX_ADDRESS; j++)
					Write(i,j,0);

			//reset
			for (int i = 0; i < MAX_CHIPS; i++)
				Write(i,0x0F,3);

			for (int i = 0; i < MAX_CHIPS; i++)
				chip_mask[i] = 0x80;

			//instrument
			for (int i = 0; i < MAX_MIDI_CHANNELS; i++)
			{
				for (int j = 0; j < MAX_LAYERS; j++)
				{
					active_layer = j;
					pokey_instrument[i]->distortion[j] = NOT_ASSIGNED;
					pokey_instrument[i]->osc_mode[j] = NOT_ASSIGNED;
					pokey_instrument[i]->filter[j] = NOT_ASSIGNED;
					Distortion(i, 0);
					OscMode(i, 0);
					Filter(i, 0);
				}
			}

			active_layer = 0;
		}

		virtual void Receive(array<unsigned char>^ _message, long time_stamp) override
		{
			ChipSynth::Receive(_message, time_stamp);

			if (_message->Length == 3)
			{
				int channel = _message[0] & 0x0F;
				int command = _message[0] >> 4;

				switch (command)
				{
				case 0xB:
					switch (_message[1])
					{
					case 0x0E: Distortion(channel, _message[2]); break;
					case 0x0F: OscMode(channel, _message[2]); break;
					case 0x10: Filter(channel, _message[2]<<7); break;
					default: break;
					}
					break;
				default: break;
				}
			}
		}

		virtual int VoiceAssign(int channel, int layer) override
		{
			//chip mask: 1_ba3210
			for (int i = 0; i < MAX_VOICES; i++)
			{
				unsigned char chip = voice_map[i]->chip;
				unsigned char osc = voice_map[i]->address/2;
				unsigned char v_mask = voice_mask[pokey_instrument[channel]->osc_mode[layer]][osc];

				//check if the voice mask is ok for a given oscillator setting
				if (!(v_mask & chip_mask[chip]) && !(dist_mask[chip] & (dist_flag[pokey_instrument[channel]->distortion[layer]] << osc)))
				{
					//update the chip mask: xor a/b oscillator flag of the voice mask to get opposite values (only if a or b is set)
					if (v_mask & 0x30)
					{
						chip_mask[chip] |= v_mask ^ 0x30;
						ext_mask[chip] |= v_mask & 0x0F;//indicates that a/b voice is active
					}				
					else
						chip_mask[chip] |= v_mask;

					//update the distortion mask
					dist_mask[chip] = dist_block[pokey_instrument[channel]->distortion[layer]] | (dist_flag[pokey_instrument[channel]->distortion[layer]] << osc);

					return i;
				}
			}

			return NOT_ASSIGNED;
		}

		virtual void VoiceRelease(int channel, int layer, int voice) override
		{
			if (voice < MAX_VOICES)
			{
				unsigned char chip = voice_map[voice]->chip;
				unsigned char osc = voice_map[voice]->address/2;
				unsigned char v_mask = voice_mask[pokey_instrument[channel]->osc_mode[layer]][osc];

				//update the chip mask
				chip_mask[chip] &= ~(v_mask & 0x0F);

				//update the ext mask
				if (v_mask & 0x30)
					ext_mask[chip] &= ~(v_mask & 0x0F);

				//reset ab flags if none of the ab voices is active
				if (!ext_mask[chip])
					chip_mask[chip] &= 0x8F;

				//rest dist mask
				dist_mask[chip] &= ~(dist_flag[pokey_instrument[channel]->distortion[layer]] << osc);
				if ((dist_mask[chip] & ~dist_block[pokey_instrument[channel]->distortion[layer]]) == 0)
					dist_mask[chip] = 0;
			}
		}

		virtual void VoiceVolume(int voice, int volume) override
		{
			unsigned char v = register_cache[voice_map[voice]->chip,voice_map[voice]->address+1]&0xF0;
			v += volume/8;
			Write(voice_map[voice]->chip,voice_map[voice]->address+1,v);
		}

		virtual void SetInstrument(int channel, int layer, int voice) override
		{
			//set osc config
			POKEY_OscMode(voice, pokey_instrument[channel]->osc_mode[layer]);
			//set distortion
			POKEY_Distortion(voice, pokey_instrument[channel]->distortion[layer]);
			//set filter
			POKEY_Filter(voice, pokey_instrument[channel]->filter[layer]);
		}

		virtual void VoiceNote(int voice,  int note) override
		{
			if (voice >= MAX_VOICES) return;

			if (note < 0) note = 0;
			else if (note > (127<<7)) note = 127<<7;

			unsigned char mask = config_mask[osc_mode[voice]][voice_map[voice]->address>>2];
			if ((voice_map[voice]->address == 0) || (voice_map[voice]->address == 4))
				mask >>= 4;
			else
				mask &= 0x0F;

			int value;
			int diff = 0;
			double clk_freq;

			double midi_freq = a_freq*Math::Pow(2,(double)(note-(69<<7))/(12<<7));

			switch (mask)
			{
			case 0: case 3:
				clk_freq = chip_freq/28;
				diff = 1;
				break;
			case 1: case 4:
				clk_freq = chip_freq/114;
				diff = 1;
				break;
			case 2:
				clk_freq = chip_freq;
				diff = 4;
				break;
			case 5:
				clk_freq = chip_freq;
				diff = 7;
				break;
			default:
				return;
			}

			value = (int)Math::Round(clk_freq/(2*midi_freq))-diff;

			switch (voice_dist[voice])
			{
			case 1:
			case 5:
				if (((value + diff) % 31) == 0)
					++value;
				break;
			case 2:
				if ((mask != 2) && (mask != 5))
				{
					if ((((value+diff) % 31) == 0) || (((value+diff) % 73) == 0))
					{
						++value;			
						if ((((value+diff) % 31) == 0) || (((value+diff) % 73) == 0))
							++value;			
					}
				}
				else
				{
					if ((((value+diff) % 7) == 0) || (((value+diff) % 31) == 0) || (((value+diff) % 73) == 0))
					{
						++value;			
						if ((((value+diff) % 7) == 0) || (((value+diff) % 31) == 0) || (((value+diff) % 73) == 0))
						{
							++value;			
							if ((((value+diff) % 7) == 0) || (((value+diff) % 31) == 0) || (((value+diff) % 73) == 0))
								++value;			
						}
					}
				}
				break;
			case 4:
				if ((mask != 2) && (mask != 5))
				{
					if ((((value+diff) % 73) == 0))
						++value;			
				}
				else
				{
					if ((((value+diff) % 7) == 0) || (((value+diff) % 73) == 0))
					{
						++value;			
						if ((((value+diff) % 7) == 0) || (((value+diff) % 73) == 0))
							++value;
					}
				}
				break;
			case 6:
				if (((value + diff) % 31) == 0)
					++value;
				value += dist7AC[(value + diff ) % 15];
				if (((value + diff) % 31) == 0)
					++value;
				value += dist7AC[(value + diff ) % 15];
				break;
			case 7:
				if ((mask != 1) && (mask != 4))
				{
					value += dist7AC[(value + diff ) % 15];
				}
				else
				{
					if ((voice_map[voice]->address == 6) || (voice_map[voice]->address == 2))
						value += sharp7B[value % 5];
					else
						value += mute7B[value % 5];
				}
				break;
			case 8:
				if ((mask != 1) && (mask != 4))
				{
					value += dist7AC[(value + diff ) % 15];
				}
				else
				{
					if ((voice_map[voice]->address == 6) || (voice_map[voice]->address == 2))
						value += mute7B[value % 5];
					else
						value += sharp7B[value % 5];
				}
				break;
			default:
				break;
			}

			if (mask < 3)
			{
				if (value > 0xFF) value = 0xFF;
				Write(voice_map[voice]->chip, voice_map[voice]->address, value);
			}
			else
			{
				if (value > 0xFFFF) value = 0xFFFF;
				Write(voice_map[voice]->chip, voice_map[voice]->address, value>>8);
				Write(voice_map[voice]->chip, voice_map[voice]->address-2, value);
			}
		}

		virtual void AllVoicesOff() override
		{
			for (int i = 0; i < MAX_VOICES; i++)
				VoiceVolume(i, 0);
		}

		virtual void Write(int chip, int address, int data) override
		{
			if ((chip < MAX_CHIPS) && (address < MAX_ADDRESS))
			{
				register_cache[chip,address] = data;
				Sleep(1);
				Synth::Write(chip, address, data);
			}
		}

		void OscMode(int channel, int value)
		{
			if ((value < 28) && (pokey_instrument[channel]->osc_mode[active_layer] != value))
			{
				ChannelRelease(channel);
				pokey_instrument[channel]->osc_mode[active_layer] = value;
			}
		}

		void Distortion(int channel, int value) 
		{
			if ((value < 10) && (pokey_instrument[channel]->distortion[active_layer] != value))
			{
				ChannelRelease(channel);
				pokey_instrument[channel]->distortion[active_layer] = value;
			}
		}

		void Filter(int channel, int value)
		{
			if (pokey_instrument[channel]->filter[active_layer] != value)
			{
				pokey_instrument[channel]->filter[active_layer] = value;

				for (int i = 0; i < MAX_KEYS; i++)
					if (keys[i]->channel == channel)
						 if (keys[i]->voice[active_layer] != NOT_ASSIGNED)
								POKEY_Filter(keys[i]->voice[active_layer], value);
			}
		}

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
		void POKEY_OscMode(int voice, int value)
		{
			if (value < 28)
			{
				osc_mode[voice] = value;
				unsigned char audctl = register_cache[voice_map[voice]->chip,AUDCTL];
				unsigned char address = voice_map[voice]->address>>1;
				audctl &= ~audctl_mask_off[value][address];
				audctl |= audctl_mask_on[value][address];
				Write(voice_map[voice]->chip, AUDCTL, audctl);
			}
		}

		/// The square wave sound in Pokey can be distorted in various way:
		/// - 0: pure sound AUDFx/2
		/// - 1: 17/5/AUDFx
		///			16C: every 31 value from 24 is dead
		/// - 2: 9/5/AUDFx
		/// - 3: 17/AUDFx
		/// - 4: 9/AUDFx
		/// - 5: 5/AUDFx/2
		/// - 6: 4/5/AUDFx
		/// - 7: 4/AUDFx
		/// - 8: Volume Direct
		void POKEY_Distortion(int voice, int value)
		{
			if (value < 10)
			{
				voice_dist[voice] = value;
				Write(voice_map[voice]->chip, voice_map[voice]->address+1, (register_cache[voice_map[voice]->chip,voice_map[voice]->address+1]&0x0F) | distortion_type[value]);

				unsigned char audctl = register_cache[voice_map[voice]->chip,AUDCTL];
				if ((value == 2) || (value == 4))
					audctl |= 0x80;
				else
					audctl &= ~0x80;
				Write(voice_map[voice]->chip, AUDCTL, audctl);
			}
		}
		/// Value of the filter is set only if the oscillator is set to a filter mode.
		/// If the filter is 8-bit then the value is rescaled down to 8-bit.
		void POKEY_Filter(int voice, int value)
		{
			if (voice_map[voice]->address == 0)
			{
				if ((osc_mode[voice] >= 6) && (osc_mode[voice] < 13))
					Write(voice_map[voice]->chip, AUDF3, value>>8);
			}
			else if (voice_map[voice]->address == 2)
			{
				if ((osc_mode[voice] == 6) || (osc_mode[voice] == 8) || ((osc_mode[voice] >= 13) && (osc_mode[voice] < 28)))
				{
					Write(voice_map[voice]->chip, AUDF4, value>>8);
					if (osc_mode[voice] >= 17)
						Write(voice_map[voice]->chip, AUDF3, value);
				}
			}
		}

		virtual void ProgramChange(int channel, int value) override
		{
			if (program[channel] != value)
			{
				ChipSynth::ProgramChange(channel, value);

				switch (program[channel])
				{
				case 0:
					active_layer = 0;
					OscMode(channel, 0);
					break;
				default:
					break;
				}
			}
		}
	};
}
