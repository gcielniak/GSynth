// SynthLibApp.cpp : main project file.

#include "..\Midi\Device.h"
#include "..\Midi\DeviceConsole.h"

using namespace System;

int main(array<System::String ^> ^args)
{
	Midi::DeviceIn^ d_in = gcnew Midi::DeviceIn();
	Midi::DeviceOut^ d_out = gcnew Midi::DeviceOut();
	bool direct_connect = false;
	Midi::DeviceIn d_in2;
	Midi::Monitor^ monitor = gcnew Midi::Monitor();

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
		else if (!String::Compare(args[i],"-dc"))
		{
			direct_connect = true;
		}
		else if (!String::Compare(args[i],"-m"))
		{
			d_in->Connect(monitor);
		}
	}

	if (d_in->IsOpen() && d_out->IsOpen())
	{
		if (direct_connect)
			Midi::DirectConnect(d_in, d_out);
		else
			d_in->Connect(d_out);
		d_in->Start();
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
