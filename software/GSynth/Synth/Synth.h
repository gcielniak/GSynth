#pragma once

#include "..\Midi\Device.h"

namespace Synth
{
	using namespace System;

	ref class Synth : Midi::Module
	{
		int time_out;

		void SendSysexRequest(array<unsigned char>^ command)
		{
			message = nullptr;

			MessageProcessed(command, 0);

			DWORD start = ::GetTickCount();

			while ((int)(::GetTickCount() - start) < time_out)
				if (message != nullptr)
				{
					if ((message->Length > 3) && (message[0] == 0xF0) && (message[1] == 0x7D) && (message[message->Length-1] == 0xF7))
						return;
//					else
//						throw gcnew Exception("Wrong repsonse.");
				}

				throw gcnew Exception("Time out.");
		}

	protected:
		array<unsigned char>^ message;

	public:
		Synth() : time_out(1000) {}

		virtual void Init() {}

		virtual void Receive(array<unsigned char>^ _message, long time_stamp) override
		{
			if (message != nullptr) //indicates that there was no request call, just pass the message to the connected modules
				MessageProcessed(message, time_stamp);
			else
				message = _message;
		}

		void Reset()
		{
			MessageProcessed(gcnew cli::array<unsigned char> {0xF0, 0x7D, 0x00, 0xF7}, 0);
		}

		String^ Name()
		{
			SendSysexRequest(gcnew cli::array<unsigned char> {0xF0, 0x7D, 0x41, 0xF7});

			if ((message->Length > 4) && (message[2] == 0x41))
			{
				wchar_t* s = new wchar_t[message->Length-3];
				s[message->Length-4] = 0;
				for (int i = 0; i < message->Length-4; ++i)
					s[i] = message[i+3];
				return gcnew String(s);
			}
			throw gcnew Exception("Wrong response.");
		}

		int ClockDivider()
		{
			SendSysexRequest(gcnew cli::array<unsigned char> {0xF0, 0x7D, 0x42, 0xF7});

			if ((message->Length == 5) && (message[2] == 0x42))
				return message[3];
			throw gcnew Exception("Wrong response.");
		}

		void ClockDivider(int value)
		{
			MessageProcessed(gcnew cli::array<unsigned char> {0xF0, 0x7D, 0x03, value, 0xF7}, 0);
		}

		void Reset(int chip)
		{
			MessageProcessed(gcnew cli::array<unsigned char> {0xF0, 0x7D, 0x04, chip&0x7, 0xF7}, 0);
		}

		int Read(int chip, int address)
		{
			SendSysexRequest(gcnew cli::array<unsigned char> {0xF0, 0x7D, 0x45, ((chip&0x7)<<3)|((address>>7)&0x7), address&0x7F, 0xF7});

			if ((message->Length == 7) && (message[2] == 0x45))
				return (((message[3]&0x40)<<1)|message[5]);
			throw gcnew Exception("Wrong response.");
		}

		virtual void Write(int chip, int address, int data)
		{
			//			Console::WriteLine("{0:X} {1:X} {2:X}",chip,address,data);
			MessageProcessed(gcnew cli::array<unsigned char> {0xF0, 0x7D, 0x06, ((data&0x80)>>1)|((chip&0x7)<<3)|((address>>7)&0x7), address&0x7F, data&0x7F, 0xF7}, 0);
		}

		int MaxChips()
		{
			SendSysexRequest(gcnew cli::array<unsigned char> {0xF0, 0x7D, 0x47, 0xF7});

			if ((message->Length == 5) && (message[2] == 0x47))
				return message[3];
			throw gcnew Exception("Wrong response.");
		}

		int MaxAddress()
		{
			SendSysexRequest(gcnew cli::array<unsigned char> {0xF0, 0x7D, 0x48, 0xF7});

			if ((message->Length == 6) && (message[2] == 0x48))
				return ((message[3]<<7)|message[4]);
			throw gcnew Exception("Wrong response.");
		}

		int ReadUserMem(int address)
		{
			SendSysexRequest(gcnew cli::array<unsigned char> {0xF0, 0x7D, 0x49, (address>>7)&0x7F, address&0x7F, 0xF7});

			if ((message->Length == 7) && (message[2] == 0x49))
				return (((message[3]&0x40)<<1)|message[5]);
			throw gcnew Exception("Wrong response.");
		}

		virtual void WriteUserMem(int address, int data)
		{
			MessageProcessed(gcnew cli::array<unsigned char> {0xF0, 0x7D, 0x0A, ((data&0x80)>>1)|((address>>7)&0x3F), address&0x7F, data&0x7F, 0xF7}, 0);
		}

		int MaxAddressUserMem()
		{
			SendSysexRequest(gcnew cli::array<unsigned char> {0xF0, 0x7D, 0x4B, 0xF7});

			if ((message->Length == 6) && (message[2] == 0x4B))
				return ((message[3]<<7)|message[4]);
			throw gcnew Exception("Wrong response.");
		}
	};
}