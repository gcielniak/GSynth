#pragma once

#include "..\Synth\Synth.h"

namespace Synth
{
	using namespace System;

	ref class ChipSynth : Synth
	{
		ref struct Instrument
		{
			int pitchwheel_range;
			int poly;
			array<int>^ volume;
			array<int>^ coarse_tuning;
			array<int>^ fine_tuning;
		};
		typedef ref struct Instrument Instrument;

		array<int>^ pitchwheel;
		array<int>^ channel_pitch;
		array<int>^ volume;
		array<int>^ expression;
		array<int>^ channel_volume;
		array<int>^ bank;
		array<int>^ nrpn;
		array<int>^ rpn;
		array<int>^ param_data;
		array<int>^ fine_tuning;
		array<int>^ coarse_tuning;
		array<Instrument^>^ instrument;

	protected:
		ref struct Key
		{
			int note;
			int velocity;
			array<int>^ voice;
			int channel;
		};
		typedef ref struct Key Key;

		static const double a_freq = 440.0;
		const static int MAX_MIDI_CHANNELS = 16;
		const static int MAX_LAYERS = 4;
		const static int NOT_ASSIGNED = 0xFF;
		const static int MAX_KEYS = 10;
		array<Key^>^ keys;
		array<int>^ program;
		int active_layer;

	public:
		ChipSynth()
		{
			pitchwheel = gcnew array<int>(MAX_MIDI_CHANNELS);
			channel_pitch = gcnew array<int>(MAX_MIDI_CHANNELS);
			volume = gcnew array<int>(MAX_MIDI_CHANNELS);
			expression = gcnew array<int>(MAX_MIDI_CHANNELS);
			channel_volume = gcnew array<int>(MAX_MIDI_CHANNELS);
			bank = gcnew array<int>(MAX_MIDI_CHANNELS);
			nrpn = gcnew array<int>(MAX_MIDI_CHANNELS);
			rpn = gcnew array<int>(MAX_MIDI_CHANNELS);
			param_data = gcnew array<int>(MAX_MIDI_CHANNELS);
			fine_tuning = gcnew array<int>(MAX_MIDI_CHANNELS);
			coarse_tuning = gcnew array<int>(MAX_MIDI_CHANNELS);
			program = gcnew array<int>(MAX_MIDI_CHANNELS);
			instrument = gcnew array<Instrument^>(MAX_MIDI_CHANNELS);
			for (int i = 0; i < MAX_MIDI_CHANNELS; i++)
			{
				instrument[i] = gcnew Instrument();
				for (int j = 0; j < MAX_LAYERS; j++)
				{
					instrument[i]->volume = gcnew array<int>(MAX_LAYERS);
					instrument[i]->coarse_tuning = gcnew array<int>(MAX_LAYERS);
					instrument[i]->fine_tuning = gcnew array<int>(MAX_LAYERS);
				}
			}
			keys = gcnew array<Key^>(MAX_KEYS);
			for (int i = 0; i < MAX_KEYS; i++)
			{
				keys[i] = gcnew Key();
				keys[i]->voice = gcnew array<int>(MAX_LAYERS);
			}
		}

		virtual void Init() override
		{
			//key memory
			for (int i = 0; i < MAX_KEYS; i++)
			{
				keys[i]->channel = NOT_ASSIGNED;
				keys[i]->note = NOT_ASSIGNED;
				for (int j = 0; j < MAX_LAYERS; j++)
					keys[i]->voice[j] = NOT_ASSIGNED;
			}

			//helper variables
			for (int i = 0; i < MAX_MIDI_CHANNELS; i++)
			{
				channel_pitch[i] = 0;
				channel_volume[i] = 0x80;
			}

			//intruments
			for (int i = 0; i < MAX_MIDI_CHANNELS; i++)
				ResetInstrument(i);

			active_layer = 0;

			//controllers
			for (int i = 0; i < MAX_MIDI_CHANNELS; i++)
			{
				program[i] = NOT_ASSIGNED;
				ProgramChange(i, 0);
				PitchWheel(i, 0x40, 0x00); // centre
				Volume(i, 0x7F); //max channel volume
				FineTuning(i, 0x20, 0x00); // centre om cents
				CoarseTuning(i, 0x40); //centre in notes
			}

			ResetAllControllers();
		}

		virtual void Receive(array<unsigned char>^ _message, long time_stamp) override
		{
			if (_message->Length == 3)
			{
				int channel = _message[0] & 0x0F;
				int command = _message[0] >> 4;

				switch (command)
				{
				case 0x8:
					NoteOff(channel, _message[1], _message[2]); break;
				case 0x9:
					NoteOn(channel, _message[1], _message[2]); break;
				case 0xB:
					switch (_message[1])
					{
					case 0x00: BankMsb(channel, _message[2]); break;
					case 0x20: BankLsb(channel, _message[2]); break;
					case 0x26: DataEntryLsb(channel, _message[2]); break;
					case 0x06: DataEntryMsb(channel, _message[2]); break;
					case 0x60: DataInc(channel); break;
					case 0x61: DataDec(channel); break;
					case 0x62: NrpnLsb(channel, _message[2]); break;
					case 0x63: NrpnMsb(channel, _message[2]); break;
					case 0x64: RpnLsb(channel, _message[2]); break;
					case 0x65: RpnMsb(channel, _message[2]); break;
					case 0x78: AllSoundsOff(); break;
					case 0x79: ResetAllControllers(); break;
					case 0x7B: AllNotesOff(); break;
					case 0x07: Volume(channel, _message[2]); break;
					case 0x0B: Expression(channel, _message[2]); break;
					case 0x0C: CoarseTuning(channel, _message[2]); break;
					case 0x0D: FineTuning(channel, _message[2], 0); break;
					default: break;
					}
					break;
				case 0xE:
					PitchWheel(channel, _message[2], _message[1]); break;
				default: break;
				}
			}
		}

		virtual int VoiceAssign(int channel, int layer) { return NOT_ASSIGNED; }

		virtual void VoiceRelease(int channel, int layer, int voice) {}

		virtual void VoiceVolume(int voice, int volume) {}

		virtual void VoiceNote(int voice, int note) {}

		virtual void SetInstrument(int channel, int layer, int voice) {}

		void SetActiveLayer(int layer)
		{
			if (layer < MAX_LAYERS)
				active_layer = layer;
		}

		void ChannelRelease(int channel)
		{
			for (int i = 0; i < MAX_KEYS; i++)
			{
				if (keys[i]->channel == channel)
					for (int j = 0; j < MAX_LAYERS; j++)
						if (keys[i]->voice[j] != NOT_ASSIGNED)
						{
							VoiceVolume(keys[i]->voice[j], 0);
							VoiceRelease(channel, j, keys[i]->voice[j]);
						}
			}
		}

		void NoteOff(int channel, int note, int velocity)
		{
			for (int i = 0; i < MAX_KEYS; i++)
			{
				if (keys[i]->channel == channel)
				{
					bool mono_note = false;

					if (!instrument[channel]->poly)
					{
						for (int j = i+1; j < MAX_KEYS; j++)
						{
							if (keys[j]->channel == channel)
							{
								keys[j]->velocity = keys[i]->velocity;
								if (keys[j]->note != NOT_ASSIGNED)
								{
									for (int k = 0; k < MAX_LAYERS; k++)
									{
										keys[j]->voice[k] = keys[i]->voice[k];
										if (keys[j]->voice[k] != NOT_ASSIGNED)
										{
											int cp = channel_pitch[channel] + (instrument[channel]->coarse_tuning[k] << 7);
											cp += instrument[channel]->fine_tuning[k] >> 6;
											VoiceNote(keys[j]->voice[k], cp + (keys[j]->note<<7));
										}
									}
								}
								mono_note = true;
								break;
							}
						}
					}

					if (!mono_note)
					{
						for (int j = 0; j < MAX_LAYERS; j++)
						{
							if (keys[i]->voice[j] != NOT_ASSIGNED)
							{
								VoiceVolume(keys[i]->voice[j], 0);
								VoiceRelease(channel, j, keys[i]->voice[j]);
							}
						}
					}


					for (int j = i; j < MAX_KEYS-1; j++)
					{
						//keys[j] = keys[j+1];
						keys[j]->channel = keys[j+1]->channel;
						keys[j]->note = keys[j+1]->note;
						for (int k = 0; k < MAX_LAYERS; k++)
							keys[j]->voice[k] = keys[j+1]->voice[k];
						keys[j]->velocity = keys[j+1]->velocity;
					}
					keys[MAX_KEYS-1]->channel = NOT_ASSIGNED;
					keys[MAX_KEYS-1]->note = NOT_ASSIGNED;
					for (int j = 0; j < MAX_LAYERS; j++)
						keys[MAX_KEYS-1]->voice[j] = NOT_ASSIGNED;
					break;
				}
			}
		}

		void NoteOn(int channel, int note, int velocity)
		{
			if (!velocity)
				NoteOff(channel, note, velocity);
			else
			{
				//handle the last key;
				//proper handling would involve reassingning this voice back
				//in case voice = NOT_ASSIGNED
				for (int i = 0; i < MAX_LAYERS; i++)
				{
					if (keys[MAX_KEYS-1]->voice[i] != NOT_ASSIGNED)
					{
						VoiceVolume(keys[MAX_KEYS-1]->voice[i], 0);
						VoiceRelease(channel, i, keys[MAX_KEYS-1]->voice[i]);
					}
				}

				//move keys in memory
				for (int i = MAX_KEYS-1; i > 0; i--)
				{
					//	keys[i] = keys[i-1];
					keys[i]->channel = keys[i-1]->channel;
					keys[i]->note = keys[i-1]->note;
					for (int j = 0; j < MAX_LAYERS; j++)
						keys[i]->voice[j] = keys[i-1]->voice[j];
					keys[i]->velocity = keys[i-1]->velocity;
				}

				keys[0]->note = note;
				keys[0]->velocity = velocity;
				keys[0]->channel = channel;

				bool voice_mono = false;

				if (!instrument[channel]->poly)
				{
					//find if there is already a note assigned
					for (int j = 1; j < MAX_KEYS; j++)
					{
						if ((keys[j]->channel == channel) && (keys[j]->note != NOT_ASSIGNED))
						{
							for (int i = 0; i < MAX_LAYERS; i++)
							{
								if (instrument[channel]->volume[i])
								{
									keys[0]->voice[i] = keys[j]->voice[i];
									keys[j]->voice[i] = NOT_ASSIGNED;

									if (keys[0]->voice[i] != NOT_ASSIGNED)
									{
										int cp = channel_pitch[channel] + (instrument[channel]->coarse_tuning[i] << 7);
										cp += instrument[channel]->fine_tuning[i] >> 6;
										VoiceNote(keys[0]->voice[i], cp + (note<<7));
										int cv = channel_volume[channel]*instrument[channel]->volume[i] >> 7;
										VoiceVolume(keys[0]->voice[i], (cv*velocity)>>7);
									}
								}
							}
							voice_mono = true;
							break;
						}
					}
				}

				if (!voice_mono)
				{
					for (int i = 0; i < MAX_LAYERS; i++)
					{
						if (instrument[channel]->volume[i])
						{
							int voice = VoiceAssign(channel, i);

							keys[0]->voice[i] = voice;

							if (voice != NOT_ASSIGNED)
							{
								SetInstrument(channel, i, voice);
								int cp = channel_pitch[channel] + (instrument[channel]->coarse_tuning[i] << 7);
								cp += instrument[channel]->fine_tuning[i] >> 6;
								VoiceNote(voice, cp + (note<<7));
								int cv = channel_volume[channel]*instrument[channel]->volume[i] >> 7;
								VoiceVolume(voice, (cv*velocity)>>7);
							}
						}
					}
				}
			}
		}

		void RefreshVolume(int channel)
		{
			channel_volume[channel] = volume[channel]*expression[channel] >> 7;

			for (int i = 0; i < MAX_KEYS; i++)
				if ((keys[i]->channel == channel) && (keys[i]->note != NOT_ASSIGNED))
					for (int j = 0; j < MAX_LAYERS; j++)
						if (keys[i]->voice[j] != NOT_ASSIGNED)
						{
							int cv = channel_volume[channel]*instrument[channel]->volume[j] >> 7;
							VoiceVolume(keys[i]->voice[j], (cv*keys[i]->velocity)>>7);
						}
		}

		void Volume(int channel, int value)
		{
			if (volume[channel] != value)
			{
				if (value)
					++value;
				volume[channel] = value;
				RefreshVolume(channel);
			}
		}

		void Expression(int channel, int value)
		{
			if (expression[channel] != value)
			{
				if (value)
					++value;
				expression[channel] = value;
				RefreshVolume(channel);
			}
		}

		void FineTuning(int channel, unsigned int value_msb, unsigned int value_lsb)
		{	
			fine_tuning[channel] = ((value_msb << 7) | value_lsb) - 0x2000;
			if (fine_tuning[channel] > 0)
				++fine_tuning[channel];
			RefreshPitch(channel);
		}

		void CoarseTuning(int channel, unsigned int value)
		{
			coarse_tuning[channel] = value - 0x40;
			RefreshPitch(channel);
		}

		void RefreshPitch(int channel)
		{
			channel_pitch[channel] = pitchwheel[channel]*instrument[channel]->pitchwheel_range >> 13;
			channel_pitch[channel] += coarse_tuning[channel] << 7;
			channel_pitch[channel] += fine_tuning[channel] >> 6;

			for (int i = 0; i < MAX_KEYS; i++)
			{
				if ((keys[i]->channel == channel) && (keys[i]->note != NOT_ASSIGNED))
					for (int j = 0; j < MAX_LAYERS; j++)
						if (keys[i]->voice[j] != NOT_ASSIGNED)
						{
							int cp = channel_pitch[channel] + (instrument[channel]->coarse_tuning[j] << 7);
							cp += instrument[channel]->fine_tuning[j] >> 6;
							VoiceNote(keys[i]->voice[j], cp+(keys[i]->note << 7));
						}
			}
		}

		void PitchWheel(int channel, int value_msb, int value_lsb)
		{
			pitchwheel[channel] = ((value_msb << 7) | value_lsb) - 0x2000;
			if (pitchwheel[channel] > 0)
				++pitchwheel[channel];
			RefreshPitch(channel);
		}

		void BankLsb(int channel, unsigned int value)
		{
			bank[channel] = (bank[channel] & ~0x7F) | value;
		}

		void BankMsb(int channel, unsigned int value)
		{
			bank[channel] = (bank[channel] & 0x7F) | (value << 7);
		}

		void SetRpn(int channel, unsigned int type, unsigned int value)
		{
			switch (type)
			{
			case 0:
				InstPitchWheelRange(channel, value & 0x7F, value >> 7); break;
			case 1:
				FineTuning(channel, value & 0x7F, value >> 7); break;
			case 2:
				CoarseTuning(channel, value >> 7); break;
			default: break;
			}
		}

		void SetNrpn(int channel, unsigned int type, unsigned int value)
		{
			switch (type)
			{
			case 0x11:
				InstPitchWheelRange(channel, value & 0x7F, value >> 7); break;
			case 0x20:
				InstVolume(channel, value >> 7); break;
			case 0x21:
				InstFineTuning(channel, value & 0x7F, value >> 7); break;
			case 0x22:
				InstCoarseTuning(channel, value >> 7); break;
			default:
				//ChipSetNrpn(channel,type,value); 
				break;
			}
		}

		void UpdateParamData(int channel)
		{
			if (rpn[channel] != 0x3FFF)
				SetRpn(channel, rpn[channel], param_data[channel]);
			else if (nrpn[channel] != 0x3FFF)
				SetNrpn(channel, nrpn[channel], param_data[channel]);
		}

		void DataEntryLsb(int channel, unsigned int value)
		{
			param_data[channel] = (param_data[channel] & ~0x7F) | value;
			UpdateParamData(channel);
		}

		void DataEntryMsb(int channel, unsigned int value)
		{
			param_data[channel] = (param_data[channel] & 0x7F) | (value << 7);
			UpdateParamData(channel);
		}

		void DataInc(int channel)
		{
			if (param_data[channel] < 0x3FFF)
				++param_data[channel];
			UpdateParamData(channel);
		}

		void DataDec(int channel)
		{
			if (param_data[channel])
				--param_data[channel];
			UpdateParamData(channel);
		}

		void NrpnLsb(int channel, unsigned int value)
		{
			rpn[channel] = 0x3FFF;
			nrpn[channel] = (nrpn[channel] & ~0x7F) | value;
		}

		void NrpnMsb(int channel, unsigned int value)
		{
			rpn[channel] = 0x3FFF;
			nrpn[channel] = (nrpn[channel] & 0x7F) | (value << 7);
		}

		void RpnLsb(int channel, unsigned int value)
		{
			nrpn[channel] = 0x3FFF;
			rpn[channel] = (rpn[channel] & ~0x7F) | value;
		}

		void RpnMsb(int channel, unsigned int value)
		{
			nrpn[channel] = 0x3FFF;
			rpn[channel] = (rpn[channel] & 0x7F) | (value << 7);
		}

		virtual void AllVoicesOff() {};

		void AllSoundsOff()
		{
			//switch off all stuck notes etc.
			AllVoicesOff();

			AllNotesOff();
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
			for (int i = 0; i < MAX_MIDI_CHANNELS; i++)
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
			for (int i = 0; i < MAX_KEYS; i++)
				if (keys[i]->note != NOT_ASSIGNED)
					NoteOff(keys[i]->channel, keys[i]->note, 0);
		}

		void InstVolume(int channel, int value)
		{
			if (value) ++value;

			if (instrument[channel]->volume[active_layer] != value)
			{
				instrument[channel]->volume[active_layer] = value;
				RefreshVolume(channel);
			}
		}

		void InstPitchWheelRange(int channel, unsigned int value_msb, unsigned int value_lsb)
		{
			int value = (value_msb << 7) | value_lsb;

			if (instrument[channel]->pitchwheel_range != value)
			{
				instrument[channel]->pitchwheel_range = value;
				RefreshPitch(channel);		
			}
		}

		void InstFineTuning(int channel, unsigned int value_msb, unsigned int value_lsb)
		{
			int value = ((value_msb << 7) | value_lsb) - 0x2000;
			if (value)	++value;

			if (instrument[channel]->fine_tuning[active_layer] != value)
			{
				instrument[channel]->fine_tuning[active_layer] = value;
				RefreshPitch(channel);
			}
		}

		void InstCoarseTuning(int channel, int value)
		{
			value -= 0x40;

			if (instrument[channel]->coarse_tuning[active_layer] != value)
			{
				instrument[channel]->coarse_tuning[active_layer] = value;
				RefreshPitch(channel);
			}
		}

		virtual void ProgramChange(int channel, int value)
		{
			if (program[channel] != value)
			{
				program[channel] = value;

				LoadInstrument(channel, 0);
			}
		}

		void ResetInstrument(int channel)
		{
			int temp_al = active_layer;

			instrument[channel]->poly = 0xFF; //max polyphony
			InstPitchWheelRange(channel, 2, 0); // 2 semitones
			for (int j = 0; j < MAX_LAYERS; j++)
			{
				active_layer = j;
				InstVolume(channel, 0);
				InstFineTuning(channel, 0x20, 0x00);
				InstCoarseTuning(channel, 0x40);
			}

			active_layer = temp_al;
		}

		void LoadInstrument(int channel, int value)
		{
			ResetInstrument(channel);

			int temp_al = active_layer;

			switch (program[channel])
			{
			case 0:
				active_layer = 0;
				InstVolume(channel, 0x7F);
				break;
			default:
				break;
			}

			active_layer = temp_al;
		}

		void SaveInstrument(int channel, int value)
		{
		}
	};
}
