// SynthLibApp.cpp : main project file.

#include "..\Synth\Synth.h"
#include "..\Synth\PokeySynth.h"
#include "..\Midi\DeviceConsole.h"

using namespace System;

void PrintHelp()
{
	Console::WriteLine("[I] - Synth Info");
	Console::WriteLine("[D] - Register Dump");
	Console::WriteLine("[U] - User Memory Dump");
}

int main(array<System::String ^> ^args)
{
	//	Synth::PokeySynth^ synth = gcnew Synth::PokeySynth();
	Synth::Synth^ synth = gcnew Synth::Synth();
	Midi::DeviceIn^ midi_in = gcnew Midi::DeviceIn();
	Midi::DeviceIn^ keyboard = gcnew Midi::DeviceIn();
	Midi::DeviceOut^ midi_out = gcnew Midi::DeviceOut();
	Midi::Monitor^ midi_monitor = gcnew Midi::Monitor();

	try
	{
		midi_in->Open("GCSynth 1.7");
		midi_out->Open("GCSynth 1.7");
		keyboard->Open("5- Legacy Keyboard MIDI In");
	}
	catch (Exception^ e) 
	{
		Console::WriteLine(e->Message); 
		return 0;
	}

	midi_in->Start();
	keyboard->Start();
	midi_in->Connect(synth);
	//	keyboard->Connect(synth);
	synth->Connect(midi_out);
	//	keyboard->Connect(midi_monitor);

	ConsoleKey key;

	synth->Init();

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
					Console::WriteLine("-------------------------------");
					Console::WriteLine(" Synth Info");
					Console::WriteLine("-------------------------------");
					Console::WriteLine("Synth name: {0}", synth->Name());
					Console::WriteLine("Clock divider: {0}", synth->ClockDivider());
					Console::WriteLine("Max Chips: {0}", synth->MaxChips());
					Console::WriteLine("Max Address: {0}", synth->MaxAddress());
					Console::WriteLine("Max User Address: {0}", synth->MaxAddressUserMem());
					Console::WriteLine("-------------------------------");
					break;
				case ConsoleKey::D:
					{
						int max_chips = synth->MaxChips();
						int max_address = synth->MaxAddress();

						for (int j = 0; j < (max_address*3+4); j++)
							Console::Write("-");
						Console::WriteLine();
						Console::WriteLine(" Register dump");
						for (int j = 0; j < (max_address*3+4); j++)
							Console::Write("-");
						Console::WriteLine();

						Console::Write("  | ");
						for (int j = 0; j < max_address; j++)
							Console::Write("{0:X2} ",j);
						Console::WriteLine();

						for (int j = 0; j < (max_address*3+4); j++)
							Console::Write("-");
						Console::WriteLine();

						for (int i = 0; i < max_chips; i++)
						{
							Console::Write("{0:X2}| ",i);
							for (int j = 0; j < max_address; j++)
								Console::Write("{0:X2} ", synth->Read(i,j));
							Console::WriteLine();
						}
						for (int j = 0; j < (max_address*3+4); j++)
							Console::Write("-");
						Console::WriteLine();
					}
					break;
				case ConsoleKey::U:
					{
						int max_address = synth->MaxAddressUserMem();
						int cols = 16;

						for (int j = 0; j < (cols*3+6); j++)
							Console::Write("-");
						Console::WriteLine();
						Console::WriteLine(" User Memory Dump");
						for (int j = 0; j < (cols*3+6); j++)
							Console::Write("-");
						Console::WriteLine();

						Console::Write("    | ");
						for (int j = 0; j < cols; j++)
							Console::Write("{0:X2} ",j);
						Console::WriteLine();

						for (int j = 0; j < (cols*3+6); j++)
							Console::Write("-");
						Console::WriteLine();

						for (int i = 0; i < max_address; i++)
						{
							if (i%16 == 0)
								Console::Write("{0:X4}| ",i);
							Console::Write("{0:X2} ", synth->ReadUserMem(i));
							if (i%16 == 15)
								Console::WriteLine();
						}
						for (int j = 0; j < (cols*3+6); j++)
							Console::Write("-");
						Console::WriteLine();
					}
					break;
				default:
					break;
				}
				PrintHelp();
			}
			catch (Exception^ exc) { Console::WriteLine(exc->Message); }
		}
	}

	return 0;
}

