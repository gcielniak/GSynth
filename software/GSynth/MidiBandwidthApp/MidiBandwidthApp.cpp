// SynthLibApp.cpp : main project file.

#include "..\Midi\Device.h"
#include "..\Midi\DeviceConsole.h"

using namespace System;

int main(array<System::String ^> ^args)
{
	Midi::DeviceIn^ d_in = gcnew Midi::DeviceIn();
	Midi::DeviceOut^ d_out = gcnew Midi::DeviceOut();

	for (int i = 0; i < args->Length; i++)
	{
		if (!String::Compare(args[i],"-l"))
		{
			Console::WriteLine("Midi In Devices:");
			for (int j = 0; j < Midi::DeviceIn::PortCount(); j++)
			{
				Console::Write("{0,2}: {1}", j, Midi::DeviceIn::PortName(j));
				//check availability
				try { d_in->Open(j); d_in->Close(); }
				catch (Exception^) { Console::Write(" - N/A"); }
				Console::WriteLine();
				Console::WriteLine(Midi::DeviceIn::GetCaps(j));
			}
			Console::WriteLine("");

			Console::WriteLine("Midi Out Devices:");
			for (int j = -1; j < Midi::DeviceOut::PortCount(); j++)
			{
				Console::Write("{0,2}: {1}", j, Midi::DeviceOut::PortName(j));
				//check availability
				try { d_out->Open(j); d_out->Close(); }
				catch (Exception^) { Console::Write(" - N/A"); }
				Console::WriteLine();
				Console::WriteLine(Midi::DeviceOut::GetCaps(j));
			}
			Console::WriteLine("");
		}
		else if (!String::Compare(args[i],"-di") && (i < (args->Length - 1)))
		{
			try { d_in->Open(int::Parse(args[++i])); }
			catch (Exception^) { Console::WriteLine("Could not open the specified midi in port.");	}
		}
		else if (!String::Compare(args[i],"-do") && (i < (args->Length - 1)))
		{
			try { d_out->Open(int::Parse(args[++i])); }
			catch (Exception^) { Console::WriteLine("Could not open the specified midi out port.");	}
		}
	}

	if (d_in->IsOpen() && d_out->IsOpen())
	{
		d_in->Connect(d_out);
		d_in->Start();
	}

	int iterations = 1000;

	if (d_out->IsOpen())
	{
		Console::WriteLine("Sending short message...");

		DWORD start = ::GetTickCount();

		for (int i = 0; i < iterations; i++)
			d_out->Send(0x90, 0x60, 0x7f);

		DWORD elapsed = ::GetTickCount() - start;
		Console::WriteLine("Time elapsed {0} ms, {1} B/s", elapsed, iterations*1000*3/elapsed);

		Console::WriteLine("Sending long message...");

		start = ::GetTickCount();

		int sysex_length = 1000;

		array<unsigned char>^ message = gcnew array<unsigned char>(sysex_length);

		message[0] = 0xF0;
		message[message->Length-1] = 0xF7;

		for (int i = 0; i < iterations/sysex_length; i++)
			d_out->Send(message);

		elapsed = ::GetTickCount() - start;
		Console::WriteLine("Time elapsed {0} ms, {1} B/s", elapsed, iterations*1000/elapsed);
	}

	ConsoleKey key;

	while (true)
	{
		if (Console::KeyAvailable)
		{
			key = Console::ReadKey(true).Key;
			if (key == ConsoleKey::Escape) break;
		}
	}

	return 0;
}
