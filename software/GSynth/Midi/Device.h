#pragma once

#include <windows.h>
#pragma  comment (lib, "winmm.lib")

/// Midi library.
namespace Midi
{
	using namespace System;
	using namespace System::Runtime::InteropServices;

	/// A callback delegate used to broadcast messages between different MIDI modules.
	delegate void MidiEventHandler(array<unsigned char>^ message, long time_stamp);

	/// <summary>
	/// This is the base for for different midi modules. Implement the functionality of your module in the overridden Receive function.
	/// </summary>
	ref class Module abstract
	{
	public:
		/// Override this function to implement your own module.
		virtual void Receive(array<unsigned char>^ message, long time_stamp) {}

		/// Broadcast events to other modules.
		event MidiEventHandler^ MessageProcessed;

		/// Connect to other preceeding MIDI modules.
		void Connect(Module^ module)
		{
			MessageProcessed += gcnew MidiEventHandler(module, &Module::Receive);
		}
	};

	/// A generic midi device.
	ref class Device abstract : public Module
	{
	protected:
		bool is_open;
		int buffer_length;
		MIDIHDR *header;

	public:
		Device()
			: is_open(false), buffer_length(1024)
		{
			header = new MIDIHDR;
			header->lpData = new char[buffer_length];
			header->dwBufferLength = buffer_length;
			header->dwFlags = 0;
			header->dwBytesRecorded = 0;
		}

		~Device() { this->!Device(); }

		!Device()
		{
			delete header->lpData;
			delete header;
		}

		/// Open a port with the specified number.
		virtual void Open(int port_number) = 0;

		/// Open a port with the specified name.
		virtual void Open(String^ port_name) = 0;

		/// Check if the port is open.
		virtual bool IsOpen() { return is_open; }

		/// Close the port.
		virtual void Close() = 0;
	};

	///Generic midi out device.
	/// API functions not implemented:
	///  - midiOutCacheDrumPatches
	///  - midiOutCachePatches
	///  - midiOutGetVolume
	///  - midiOutSetVolume
	ref class DeviceOut : public Device
	{
	public:
		DeviceOut()
		{
			device_handle = new HMIDIOUT;
		}

		~DeviceOut() { this->!DeviceOut(); }

		!DeviceOut()
		{
			Close();
			delete device_handle;
		}

		/// Send the received message.
		virtual void Receive(array<unsigned char>^ message, long time_stamp) override
		{
			Send(message);
		}

		/// Send a packed midi message.
		void Send(DWORD packet)
		{
			MMRESULT result = midiOutShortMsg(*device_handle, packet);
			if (result != MMSYSERR_NOERROR)
				throw gcnew Exception("DeviceOut::Send, " + GetErrorText(result));
		}

		/// Send a Sysex message
		/// \todo Fix the issue with the blocking devices.
		void SendSysex(array<unsigned char>^ message)
		{
			int nr_buffers = message->Length/buffer_length + 1;

			MMRESULT result;

			//do in a loop for messages longer than the buffer_size (1024 by default)
			for (int i = 0; i < nr_buffers; i++)
			{
				if (i == nr_buffers - 1)
					header->dwBufferLength = message->Length % buffer_length;
				else
					header->dwBufferLength = buffer_length;

				Marshal::Copy(message, i*buffer_length, IntPtr(header->lpData), header->dwBufferLength);

				result = midiOutPrepareHeader(*device_handle, header, sizeof(MIDIHDR)); 
				if (result != MMSYSERR_NOERROR)
					throw gcnew Exception("DeviceOut::SendSysex, " + GetErrorText(result));

				result = midiOutLongMsg(*device_handle, header, sizeof(MIDIHDR));
				if (result != MMSYSERR_NOERROR)
					throw gcnew Exception("DeviceOut::SendSysex, " + GetErrorText(result));

				// Unprepare the buffer and MIDIHDR.
				// That's where some devices get stuck
				while ((result = midiOutUnprepareHeader(*device_handle, header, sizeof(MIDIHDR))) != MMSYSERR_NOERROR)
					if (result != MIDIERR_STILLPLAYING)
						throw gcnew Exception("DeviceOut::SendSysex, " + GetErrorText(result));
			}
		}

		/// Send three bytes of the midi mesage. If there is less bytes set other to 0.
		void Send(unsigned char data_1, unsigned char data_2, unsigned char data_3)
		{
			DWORD packet = 0;
			packet |= data_1;
			packet |= data_2<<8;
			packet |= data_3<<16;

			Send(packet);
		}

		/// Generic send.
		void Send(array<unsigned char>^ message)
		{
			if (message->Length && (message[0] == 0xF0))
				SendSysex(message);
			else
			{
				if (message->Length == 1)
					Send(message[0]);
				else if (message->Length == 2)
					Send(message[0] | (message[1]<<8));
				else if (message->Length == 3)
					Send(message[0] | (message[1]<<8) | (message[2]<<16));
				else
					throw gcnew Exception("DeviceOut::Send, wrong message format.");
			}
		}

		/// Open a port with the specified number. -1 for midi mapper.
		virtual void Open(int port_number) override
		{
			if (is_open)
				throw gcnew Exception("DeviceOut::Open, The device is already open.");

			MIDIOUTCAPS device_caps;

			MMRESULT result = midiOutGetDevCaps(port_number, &device_caps, sizeof(MIDIOUTCAPS));
			if (result != MMSYSERR_NOERROR)
				throw gcnew Exception("DeviceOut::Open, " + GetErrorText(result));

			result = midiOutOpen(device_handle, port_number, 0, 0, CALLBACK_NULL);
			if (result != MMSYSERR_NOERROR)
				throw gcnew Exception("DeviceOut::Open, " + GetErrorText(result));

			is_open = true;
		}

		/// Open a port with the specified name.
		virtual void Open(String^ port_name) override
		{
			int pcount = PortCount();

			for (int i = 0; i < pcount; i++)
			{
				if (String::Compare(port_name, PortName(i)) == 0)
				{
					try {
						Open(i);
						return;
					}
					catch (Exception^) {}
				}
			}
			throw gcnew Exception("DeviceOut::Open, Could not find the specified port: " + port_name);
		}

		/// Close the port.
		virtual void Close() override
		{
			if (is_open)
			{
				MMRESULT result = midiOutReset(*device_handle);
				if (result != MMSYSERR_NOERROR)
					throw gcnew Exception("DeviceOut::Close, " + GetErrorText(result));

				result = midiOutClose(*device_handle);			
				if (result != MMSYSERR_NOERROR)
					throw gcnew Exception("DeviceOut::Close, " + GetErrorText(result));

				is_open = false;
			}
		}

		/// Get name of the specified port.
		static String^ PortName(int port_nr)
		{
			MIDIOUTCAPS device_caps;

			MMRESULT result = midiOutGetDevCaps(port_nr, &device_caps, sizeof(MIDIOUTCAPS));

			if (result != MMSYSERR_NOERROR)
				throw gcnew Exception("DeviceOut::PortName, " + GetErrorText(result));

			return gcnew String(device_caps.szPname);
		}

		/// Get number of ports.
		static int PortCount()
		{
			return midiOutGetNumDevs();
		}

		HMIDIOUT *DeviceHandle() { return device_handle; }

		///Get device capabilities
		static String^ GetCaps(int pnumber)
		{
			MIDIOUTCAPS device_caps;

			MMRESULT result = midiOutGetDevCaps(pnumber, &device_caps, sizeof(MIDIOUTCAPS));
			if (result != MMSYSERR_NOERROR)
				throw gcnew Exception("DeviceOut::PrintCaps, " + GetErrorText(result));

			String^ s = String::Format("Manufacturer {0}\n", device_caps.wMid);
			s += String::Format("Product      {0}\n", device_caps.wPid);
			s += String::Format("Version      {0}.{0}\n", device_caps.vDriverVersion>>8,  device_caps.vDriverVersion&0xFF);
			s += String::Format("Name         {0}\n", gcnew String(device_caps.szPname));
			s += String::Format("Technology   {0}\n", GetTechnology(device_caps.wTechnology));
			s += String::Format("Voices       {0}\n", device_caps.wVoices);
			s += String::Format("Notes        {0}\n", device_caps.wNotes);
			if (device_caps.wChannelMask == 0xFFFF)
				s += String::Format("Mask         all channels\n");
			else
				s += String::Format("Mask         {0:X}\n", device_caps.wChannelMask);
			s += String::Format("Support:\n{0}", GetSupport(device_caps.dwSupport));

			return s;
		}

	private:
		HMIDIOUT *device_handle;

		///get error text
		static String^ GetErrorText(MMRESULT error)
		{
			WCHAR text[MAXERRORLENGTH];

			MMRESULT result = midiOutGetErrorText(error, text, MAXERRORLENGTH);

			if (result != MMSYSERR_NOERROR)
				return gcnew String("DeviceOut::GetErrorText, Unspecified error.");
			else
				return gcnew String(text);
		}

		static String^ GetTechnology(WORD tech)
		{
			switch (tech)
			{
				case MOD_MIDIPORT:
					return gcnew String("MIDI hardware port");
				case MOD_SYNTH:
					return gcnew String("Synthesizer");
				case MOD_SQSYNTH:
					return gcnew String("Square wave synthesizer");
				case MOD_FMSYNTH:
					return gcnew String("FM synthesizer");
				case MOD_MAPPER:
					return gcnew String("Microsoft MIDI mapper");
				case MOD_WAVETABLE:
					return gcnew String("Hardware wavetable synthesizer");
				case MOD_SWSYNTH:
					return gcnew String("Software synthesizer");
			}

			return gcnew String("");
		}

		static String^ GetSupport(DWORD mask)
		{
			String^ s = gcnew String("");

			if (mask & MIDICAPS_VOLUME)
				s += " volume control\n";
			if (mask & MIDICAPS_LRVOLUME)
				s += " separate left and right volume control\n";
			if (mask & MIDICAPS_CACHE)
				s += " patch caching\n";
			if (mask & MIDICAPS_STREAM)
				s += " midiStreamOut function\n";

			if (!String::Compare(s,""))
				s = " none\n";

			return s;
		}
	};

	void CALLBACK MidiInProc(HMIDIIN device_handle, UINT message, DWORD user_data, DWORD midi_data, DWORD time_stamp);

	///Generic midi in device.
	ref class DeviceIn : public Device
	{
		GCHandle gc_handle;
		HMIDIIN *device_handle;

	public:
		DeviceIn()
		{
			device_handle = new HMIDIIN;
			gc_handle = GCHandle::Alloc(this, GCHandleType::Weak);
		}

		//destructor
		~DeviceIn()	{ this->!DeviceIn(); }
		
		//finaliser
		!DeviceIn()
		{
			Close();
			delete device_handle;
			gc_handle.Free();
		}

		/// Get port's name
		static String^ PortName(int port_nr)
		{
			MIDIINCAPS device_caps;

			MMRESULT result = midiInGetDevCaps(port_nr, &device_caps, sizeof(MIDIINCAPS));

			if (result != MMSYSERR_NOERROR)
				throw gcnew Exception("DeviceIn::PortName, " + GetErrorText(result));

			return gcnew String(device_caps.szPname);
		}

		/// Get number of ports
		static int PortCount()
		{
			return midiInGetNumDevs();
		}

		/// Open the port with a specified number.
		virtual void Open(int port_number) override
		{
			if (is_open)
				throw gcnew Exception("DeviceIn::Open, The device is already opened.");

			MIDIINCAPS device_caps;

			MMRESULT result = midiInGetDevCaps(port_number, &device_caps, sizeof(MIDIINCAPS));
			if (result != MMSYSERR_NOERROR)
				throw gcnew Exception("DeviceIn::Open, " + GetErrorText(result));

			result = midiInOpen(device_handle, port_number, (DWORD_PTR)MidiInProc, (DWORD_PTR)GCHandle::ToIntPtr(gc_handle).ToPointer(), CALLBACK_FUNCTION);

			if (result != MMSYSERR_NOERROR)
				throw gcnew Exception("DeviceIn::Open, " + GetErrorText(result));

			result = midiInPrepareHeader(*device_handle, header, sizeof(MIDIHDR));
			if (result != MMSYSERR_NOERROR)
				throw gcnew Exception("DeviceIn::Open, " + GetErrorText(result));

			result = midiInAddBuffer(*device_handle, header, sizeof(MIDIHDR));
			if (result != MMSYSERR_NOERROR)
				throw gcnew Exception("DeviceIn::Open, " + GetErrorText(result));

			is_open = true;
		}

		/// Open the port with a specified name.
		virtual void Open(String^ port_name) override
		{
			int pcount = PortCount();

			for (int i = 0; i < pcount; i++)
			{
				if (String::Compare(port_name, PortName(i)) == 0)
				{
					try {
						Open(i);
						return;
					}
					catch (Exception^) {}
				}
			}

			throw gcnew Exception("DeviceIn::Open, Could not find the specified port: " + port_name);
		}

		/// Close the port.
		virtual void Close() override
		{
			if (is_open)
			{
				MMRESULT result = midiInReset(*device_handle);
				if (result != MMSYSERR_NOERROR)
					throw gcnew Exception("DeviceIn::Close, " + GetErrorText(result));

				Stop();

				while ((result = midiInUnprepareHeader(*device_handle, header, sizeof(MIDIHDR))) != MMSYSERR_NOERROR)
					if (result != MIDIERR_STILLPLAYING)
						throw gcnew Exception("DeviceIn::Close, " + GetErrorText(result));

				if (result != MMSYSERR_NOERROR)
					throw gcnew Exception("DeviceIn::Close, " + GetErrorText(result));

				result = midiInClose(*device_handle);

				if (result != MMSYSERR_NOERROR)
					throw gcnew Exception("DeviceIn::Close, " + GetErrorText(result));

				is_open = false;
			}
		}

		/// Start receiving messages.
		void Start()
		{
			if (is_open)
			{
				MMRESULT result = midiInStart(*device_handle);
				if (result != MMSYSERR_NOERROR)
					throw gcnew Exception("DeviceIn::Start, " + GetErrorText(result));
			}
		}

		/// Stop receiving messages.
		void Stop()
		{
			if (is_open)
			{
				MMRESULT result = midiInStop(*device_handle);
				if (result != MMSYSERR_NOERROR)
					throw gcnew Exception("DeviceIn::Stop, " + GetErrorText(result));
			}
		}

		/// Process the received sysex message.
		void ProcessLongMessage(long time_stamp)
		{
			if (header->dwBytesRecorded)
			{
				cli::array<unsigned char>^ message = gcnew cli::array<unsigned char> (header->dwBytesRecorded);

				Marshal::Copy(IntPtr(header->lpData), message, 0, message->Length);

				MMRESULT result = midiInAddBuffer(*device_handle, header, sizeof(MIDIHDR));
				if (result != MMSYSERR_NOERROR)
					throw gcnew Exception("DeviceIn::ProcessLongMessage, " + GetErrorText(result));

				MessageProcessed(message, time_stamp);
			}
		}

		/// Process the received short message.
		void ProcessMessage(DWORD packet, long time_stamp)
		{
			unsigned char status = packet & 0xF0;

			if ((status == 0x80) || (status == 0x90) || (status == 0xA0) || (status == 0xB0) || (status == 0xE0) || ((packet&0xFF) == 0xF1) || ((packet&0xFF) == 0xF2))
				MessageProcessed(gcnew array<unsigned char> {(unsigned char)packet, (unsigned char)(packet>>8), (unsigned char)(packet>>16)}, time_stamp);
			else if ((status == 0xC0) || (status == 0xD0) || ((packet&0xFF) == 0xF3))
				MessageProcessed(gcnew array<unsigned char> {(unsigned char)packet, (unsigned char)(packet>>8)}, time_stamp);
			else if ((packet&0xFF) >= 0xF4)
				MessageProcessed(gcnew array<unsigned char> {(unsigned char)packet}, time_stamp);
		}

		HMIDI *DeviceHandle() { return (HMIDI*)device_handle; }

		///Get device capabilities
		static String^ GetCaps(int port_number)
		{
			MIDIINCAPS device_caps;

			MMRESULT result = midiInGetDevCaps(port_number, &device_caps, sizeof(MIDIINCAPS));
			if (result != MMSYSERR_NOERROR)
				throw gcnew Exception("DeviceIn::PrintCaps, " + GetErrorText(result));

			String^ s = String::Format("Manufacturer {0}\n", device_caps.wMid);
			s += String::Format("Product      {0}\n", device_caps.wPid);
			s += String::Format("Version      {0}.{0}\n", device_caps.vDriverVersion>>8,  device_caps.vDriverVersion&0xFF);
			s += String::Format("Name         {0}\n", gcnew String(device_caps.szPname));

			return s;
		}

	private:
		///get error text
		static String^ GetErrorText(MMRESULT error)
		{
			WCHAR text[MAXERRORLENGTH];

			MMRESULT result = midiInGetErrorText(error, text, MAXERRORLENGTH);

			if (result != MMSYSERR_NOERROR)
				return gcnew String("DeviceIn::GetErrorText, Unspecified error.");
			else
				return gcnew String(text);
		}
	};

	/// Callback function.
	void CALLBACK MidiInProc(HMIDIIN device_handle, UINT message,
		DWORD user_data, DWORD midi_data, DWORD time_stamp)
	{
		DeviceIn^ device = (DeviceIn^)GCHandle::FromIntPtr(IntPtr((long)user_data)).Target;

		switch (message)
		{
		case MIM_LONGDATA:
			device->ProcessLongMessage(time_stamp);
			break;
		case MIM_DATA:
			device->ProcessMessage(midi_data,time_stamp);
			break;
		case MIM_ERROR:
			Console::WriteLine("DeviceIn::MidiInProc, MIM_ERROR");
			break;
		case MIM_LONGERROR:
			Console::WriteLine("DeviceIn::MidiInProc, MIM_LONGERROR");
			break;
		case MIM_MOREDATA:
			Console::WriteLine("DeviceIn::MidiInProc, MIM_MOREDATA");
			break;
		default: break;
		}
	}

	/// Get error text.
	static String^ GetErrorText(MMRESULT result)
	{
		switch (result)
		{
		case MIDIERR_NOTREADY:
			return gcnew String("The specified input device is already connected to an output device.");
		case MMSYSERR_INVALHANDLE:
			return gcnew String("The specified device handle is invalid.");
		default:
			return gcnew String("Unspecified error.");
		}
	}

	/// Directly connect two midi devices.
	static void DirectConnect(DeviceIn^ in, DeviceOut^ out)
	{
		MMRESULT result = midiConnect(*in->DeviceHandle(),*out->DeviceHandle(),0);

		if (result != MMSYSERR_NOERROR)
			throw gcnew Exception("DirectConnect, " + GetErrorText(result));
	}

	/// Disconnect two midi devices.
	static void DirectDisconnect(DeviceIn^ in, DeviceOut^ out)
	{
		MMRESULT result = midiDisconnect(*in->DeviceHandle(),*out->DeviceHandle(),0);

		if (result != MMSYSERR_NOERROR)
			throw gcnew Exception("DirectDisconnect, " + GetErrorText(result));
	}
}
