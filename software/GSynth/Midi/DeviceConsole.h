#pragma once

#include "Device.h"

namespace Midi
{
	enum PCKey
	{
		ESC = 27,
		PAGE_UP = 33,
		PAGE_DOWN = 34,
		HOME = 36
	};

	/// A basic console monitor.
	ref class Monitor : public Module
	{
	protected:
		long prev_time;

	public:
		Monitor()
			: prev_time(0) {}

		/// Prints out the timestamp and the message content in hex format.
		virtual void Receive(array<unsigned char>^ message, long time_stamp) override
		{
			Console::Write("{0,4} ", time_stamp-prev_time);
			prev_time = time_stamp;
			for (int i = 0; i < message->Length; i++)
				Console::Write("{0:X2} ",message[i]);
			Console::WriteLine();
		}
	};

	/// An enhanced console monitor.
	ref class MonitorEx : public Monitor
	{
	public:
		virtual void Receive(array<unsigned char>^ message, long time_stamp) override
		{
			Console::Write("{0,4} ",time_stamp-prev_time);
			prev_time = time_stamp;
			if (message->Length)
			{
				switch (message[0] & 0xF0)
				{
				case 0x80:
					Console::Write("Note Off "); break;
				case 0x90:
					Console::Write("Note On  "); break;
				case 0xA0:
					Console::Write("Key Pressure "); break;
				case 0xB0:
					Console::Write("Control Change "); break;
				case 0xC0:
					Console::Write("Program Change "); break;
				case 0xD0:
					Console::Write("Channel Pressure "); break;
				case 0xE0:
					Console::Write("Pitch Wheel "); break;
				case 0xF0:
					Console::Write("System Common "); break;
				default: break;
				}
				for (int i = 0; i < message->Length; i++)
					Console::Write("{0:X2} ",message[i]);
			}
			Console::WriteLine();
		}
	};

	unsigned char midi_notes[128] =	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,61,0,0,66,70,0,73,75,0,78,60,0,62,64,
		75,0,61,63,0,66,68,70,0,73,63,63,60,78,62,64,
		0,0,55,52,51,64,0,54,56,72,58,0,61,59,57,74,
		76,60,65,49,67,71,53,62,50,69,48,77,47,79,68,0,
		0,0,55,52,51,64,0,54,56,72,58,0,61,59,57,74,
		76,60,65,49,67,71,53,62,50,69,48,77,47,79,0,0
	};

	/// Simple console midi keyboard.
	ref class Keyboard : public Module
	{
		DWORD* event_counter;
		HANDLE input_handle;
		INPUT_RECORD* input_record;
		DWORD* number_read;
		array<unsigned char>^ message;
		int channel;
		int time_stamp;
		array<unsigned char>^ key_pressed;
		int octave;

	public:
		Keyboard()
		{
			message = gcnew array<unsigned char>(3);
			input_handle = GetStdHandle(STD_INPUT_HANDLE);
			input_record = new INPUT_RECORD;
			event_counter = new DWORD;
			number_read = new DWORD;
			channel = 0;
			time_stamp = 0;
			octave = 0;
			key_pressed = gcnew array<unsigned char>(128);
			key_pressed->Clear(key_pressed, 0, key_pressed->Length);
		}

		bool Process(int end_key)
		{
			GetNumberOfConsoleInputEvents(input_handle, event_counter);

			if (*event_counter)
				time_stamp = ::GetTickCount();

			while ((*event_counter)-- > 0)
			{
				ReadConsoleInput(input_handle, input_record, 1, number_read);

				if (input_record->EventType == KEY_EVENT)
				{
					int key = input_record->Event.KeyEvent.uChar.AsciiChar;

					if (!key && input_record->Event.KeyEvent.bKeyDown) //virtual key
					{
						switch ((PCKey)input_record->Event.KeyEvent.wVirtualKeyCode)
						{
						case ESC:
							if (octave < 3)	octave++; break;
						case PAGE_UP:
							if (octave > -3) octave--; break;
						case PAGE_DOWN:
							octave = 0;	break;
						}
						return true;					
					}

					if (key == end_key) // end key
						return false;

					if (key == -93)	key = 63; //pound key

					if (midi_notes[key])
						if (input_record->Event.KeyEvent.bKeyDown)
						{
							if (!key_pressed[key])
							{
								key_pressed[key] = 1;

								message[0] = 0x90|channel;
								message[1] = midi_notes[key] + octave*12;
								message[2] = 0x7F;

								MessageProcessed(message, time_stamp);
							}
						}
						else
						{
							if (key_pressed[key])
							{
								key_pressed[key] = 0;

								message[0] = 0x80|channel;
								message[1] = midi_notes[key] + octave*12;
								message[2] = 0x7F;

								MessageProcessed(message, time_stamp);
							}
						}
				}

			}
			return true;
		}
	};
}
