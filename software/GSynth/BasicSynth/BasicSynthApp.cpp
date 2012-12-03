// SynthLibApp.cpp : main project file.

#include "..\Synth\PokeySynth.h"

using namespace System;

void PrintHelp()
{
}

char dist7AC[15] = {1,0,0,1,0,2,1,0,0,2,1,0,1,0,0};
char mute17[5] = {1, 0, 0, -1, -2};
char sharp17[5] = {0, -1, 1, 0, -1};

int main(array<System::String ^> ^args)
{
	ConsoleKey key;
	Synth::PokeySynth^ synth = gcnew Synth::PokeySynth();
	Midi::DeviceIn^ midi_in = gcnew Midi::DeviceIn();
	Midi::DeviceOut^ midi_out = gcnew Midi::DeviceOut();

	try
	{
		midi_in->Open("5- Legacy Keyboard MIDI In");
		midi_out->Open("GCSynth 1.7");
	}
	catch (Exception^ e) 
	{
		Console::WriteLine(e->Message); 
		return 0;
	}

	midi_in->Start();

	midi_in->Connect(synth);
	synth->Connect(midi_out);

	synth->Init();

	unsigned short v, value = 0, dist = 0, osc = 0;
	int diff = 4;

//	synth->Distortion(0,6);
//	synth->OscMode(0,2);

	while (true)
	{
		if (Console::KeyAvailable)
		{
			key = Console::ReadKey(true).Key;
			if (key == ConsoleKey::Escape) break;
			try
			{
				switch (key)
				{
				case ConsoleKey::I:
					++v;
					value = v;
					if (((value + diff) % 31) == 0)
						++value;
					value += dist7AC[(value + diff ) % 15];
					if (((value + diff) % 31) == 0)
						++value;
					value += dist7AC[(value + diff ) % 15];

//					value += sharp17[v % 5];
//					if (((v % 5) == 0) || (((v + 1) % 5) == 0) || (((v + 2) % 5) == 0)) {}	else
					{
						synth->Write(0,2,value>>8);
						synth->Write(0,0,value);
//						synth->Write(0,6,value>>8);
//						synth->Write(0,4,value);
						Console::WriteLine("{0}",value);
					}
					break;
				case ConsoleKey::K:
					--v;
					value = v;
					if (((value + diff) % 31) == 0)
						++value;
					value += dist7AC[(value + diff ) % 15];
					if (((value + diff) % 31) == 0)
						++value;
					value += dist7AC[(value + diff ) % 15];
//					value += sharp17[v % 5];
//					if (((v % 5) == 0) || (((v + 1) % 5) == 0) || (((v + 2) % 5) == 0)) {}	else
					{
						synth->Write(0,2,value>>8);
						synth->Write(0,0,value);
//						synth->Write(0,6,value>>8);
//						synth->Write(0,4,value);
						Console::WriteLine("{0}",value);
					}
					break;
				case ConsoleKey::W:
					++osc;
					synth->OscMode(0,osc);
					Console::WriteLine("OSC: {0}",osc);
					break;
				case ConsoleKey::S:
					--osc;
					synth->OscMode(0,osc);
					Console::WriteLine("OSC: {0}",osc);
					break;
				case ConsoleKey::E:
					++dist;
					synth->Distortion(0,dist);
					Console::WriteLine("D: {0}",dist);
					break;
				case ConsoleKey::D:
					--dist;
					synth->Distortion(0,dist);
					Console::WriteLine("D: {0}",dist);
					break;
				default:
					break;
				}
//				PrintHelp();
			}
			catch (Exception^ exc) { Console::WriteLine(exc->Message); }
		}
	}

	return 0;
}

