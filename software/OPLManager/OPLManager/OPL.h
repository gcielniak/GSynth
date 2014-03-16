#pragma once

namespace OPL
{
	using namespace System;
	using namespace System::IO;
	using namespace System::Collections::Generic;

	ref class Operator
	{
	public:
		bool amplitude_modulation;
		bool vibrato;
		bool envelope_type;
		bool key_scale_rate;
		int multiplier;
		int key_scale_level;
		int key_scaling;
		int total_level;
		int attack_rate;
		int decay_rate;
		int sustain_level;
		int release_rate;
		int waveform;
		int channel_output;
		int feedback;

		int Compare(Operator^ op)
		{
			int compare = 0;
			if (amplitude_modulation != op->amplitude_modulation)
				compare++;
			if (vibrato != op->vibrato)
				compare++;
			if (envelope_type != op->envelope_type)
				compare++;
			if (key_scale_rate != op->key_scale_rate)
				compare++;
			if (multiplier != op->multiplier)
				compare++;
			if (key_scale_level != op->key_scale_level)
				compare++;
			if (key_scaling != op->key_scaling)
				compare++;
			if (total_level != op->total_level)
				compare++;
			if (attack_rate != op->attack_rate)
				compare++;
			if (decay_rate != op->decay_rate)
				compare++;
			if (sustain_level != op->sustain_level)
				compare++;
			if (release_rate != op->release_rate)
				compare++;
			if (waveform != op->waveform)
				compare++;
			if (channel_output != op->channel_output)
				compare++;
			if (feedback != op->feedback)
				compare++;
			return compare;
		}
	};

	ref class Instrument
	{
		///Read SBI (Sound Blaster Instrument) file
		static List<OPL::Instrument^>^ ReadFileSBI(BinaryReader^ reader)
		{
			List<OPL::Instrument^>^ instrument_list = gcnew List<OPL::Instrument^>();
			instrument_list->Add(gcnew Instrument(encoding->GetString(reader->ReadBytes(32)),  reader->ReadBytes(16)));
			return instrument_list;
		}

		///Read IBK (Instrument Bank) file
		static List<OPL::Instrument^>^ ReadFileIBK(BinaryReader^ reader)
		{
			array<unsigned char>^ settings = reader->ReadBytes(16*128);
			array<unsigned char>^ names = reader->ReadBytes(9*128);

			List<OPL::Instrument^>^ instrument_list = gcnew List<OPL::Instrument^>();

			for (int i = 0; i < 128; i++)
			{
				array<unsigned char>^ s = gcnew array<unsigned char>(16);
				array<unsigned char>^ n = gcnew array<unsigned char>(9);
				Array::Copy(settings, i*16, s, 0, 16);
				Array::Copy(names, i*9, n, 0, 9);
				instrument_list->Add(gcnew Instrument(encoding->GetString(n),s));
			}

			return instrument_list;
		}

		///Read CMF (Creative Music Format) file - only instruments
		static List<OPL::Instrument^>^ ReadFileCMF(BinaryReader^ reader)
		{
			array<unsigned char>^ version = reader->ReadBytes(2);
			int ver;
			if ((version[1] == 1) && (version[0] == 0))
				ver = 10;
			else if ((version[1] == 1) && (version[0] == 1))
				ver = 11;
			else
				throw gcnew Exception("Instrument::ReadFileCMF, wrong version number.");

			unsigned short instrument_offset = reader->ReadUInt16();

			if (0)
			{
				reader->ReadBytes(6);

				unsigned short title_offset = reader->ReadUInt16();
				unsigned short composer_offset = reader->ReadUInt16();
				unsigned short remarks_offset = reader->ReadUInt16();

				if (title_offset)
				{
					reader->BaseStream->Seek(title_offset, SeekOrigin::Begin);
					int length = 0;
					while (reader->ReadByte() != 0)
						++length;
					reader->BaseStream->Seek(title_offset, SeekOrigin::Begin);
					array<unsigned char>^ title = reader->ReadBytes(length);
					Console::WriteLine("Title: {0}",encoding->GetString(title));

				}

				if (composer_offset)
				{
					reader->BaseStream->Seek(composer_offset, SeekOrigin::Begin);
					int length = 0;
					while (reader->ReadByte() != 0)
						++length;
					reader->BaseStream->Seek(composer_offset, SeekOrigin::Begin);
					array<unsigned char>^ composer = reader->ReadBytes(length);
					Console::WriteLine("Composer: {0}",encoding->GetString(composer));

				}

				if (remarks_offset)
				{
					reader->BaseStream->Seek(remarks_offset, SeekOrigin::Begin);
					int length = 0;
					while (reader->ReadByte() != 0)
						++length;
					reader->BaseStream->Seek(remarks_offset, SeekOrigin::Begin);
					array<unsigned char>^ remarks = reader->ReadBytes(length);
					Console::WriteLine("Remarks: {0}",encoding->GetString(remarks));
				}
			}

			//go to the instrument count 
			reader->BaseStream->Seek(36, SeekOrigin::Begin);

			int instrument_count;

			if (ver == 10)
				instrument_count = reader->ReadByte();
			else
			{
				instrument_count = reader->ReadUInt16();
				//skip tempo
				reader->ReadBytes(2);
			}

			reader->BaseStream->Seek(instrument_offset, SeekOrigin::Begin);

			array<unsigned char>^ settings = reader->ReadBytes(16*instrument_count);

			List<OPL::Instrument^>^ instrument_list = gcnew List<OPL::Instrument^>();

			for (int i = 0; i < instrument_count; i++)
			{
				array<unsigned char>^ s = gcnew array<unsigned char>(16);
				Array::Copy(settings, i*16, s, 0, 16);
				instrument_list->Add(gcnew Instrument(String::Format("Instr{0}",i), s));
			}

			return instrument_list;
		}

	public:
		String^ name;
		int operator_connection;
		array<Operator^>^ operators;
		static Text::Encoding^ encoding = Text::Encoding::ASCII;

		Instrument(String^ _name, array<unsigned char>^ settings)
			: name(_name)
		{
			operators = gcnew array<Operator^> (2);
			for (int i = 0; i < operators->Length; i++)
				operators[i] = gcnew Operator;

			operators[0]->amplitude_modulation = ((settings[0] & 0x80) == 1);
			operators[0]->vibrato = ((settings[0] & 0x40) == 1);
			operators[0]->envelope_type = ((settings[0] & 0x20) == 1);
			operators[0]->key_scale_rate = ((settings[0] & 0x10) == 1);
			operators[0]->multiplier = (settings[0] & 0x0F);

			operators[1]->amplitude_modulation = ((settings[1] & 0x80) == 1);
			operators[1]->vibrato = ((settings[1] & 0x40) == 1);
			operators[1]->envelope_type = ((settings[1] & 0x20) == 1);
			operators[1]->key_scale_rate = ((settings[1] & 0x10) == 1);
			operators[1]->multiplier = (settings[1] & 0x0F);

			operators[0]->key_scale_level = (settings[2] >> 6);
			operators[0]->total_level = (settings[2] & 0x3F);

			operators[1]->key_scale_level = (settings[3] >> 6);
			operators[1]->total_level = (settings[3] & 0x3F);

			operators[0]->attack_rate = (settings[4] >> 4);
			operators[0]->decay_rate = (settings[4] & 0x0F);

			operators[1]->attack_rate = (settings[5] >> 4);
			operators[1]->decay_rate = (settings[5] & 0x0F);

			operators[0]->sustain_level = (settings[6] >> 4);
			operators[0]->release_rate = (settings[6] & 0x0F);

			operators[1]->sustain_level = (settings[7] >> 4);
			operators[1]->release_rate = (settings[7] & 0x0F);

			operators[0]->waveform = (settings[8] & 0x03);
			operators[1]->waveform = (settings[9] & 0x03);

			operators[0]->feedback = ((settings[10] >> 1) & 0x07);

			operator_connection = (settings[10] & 0x01);
		}

		int Compare(Instrument^ instrument)
		{
			int compare = 0;
			if (operator_connection != instrument->operator_connection)
				compare++;
			for (int i = 0; i < operators->Length; i++)
				compare += operators[i]->Compare(instrument->operators[i]);
			return compare;
		}

		static List<OPL::Instrument^>^ ReadFile(String^ file_name)
		{
			//determine the format
			BinaryReader^ reader = gcnew BinaryReader(File::Open(file_name, FileMode::Open));
			array<unsigned char>^ signature = reader->ReadBytes(4);
			List<OPL::Instrument^>^ instrument_list;

			if (signature[0] == 'S' && signature[1] == 'B' && signature[2] == 'I' && signature[3] == 0x1A)
			{
				instrument_list = ReadFileSBI(reader);
			}
			else if (signature[0] == 'I' && signature[1] == 'B' && signature[2] == 'K' && signature[3] == 0x1A)
			{
				instrument_list = ReadFileIBK(reader);
			}
			else if (signature[0] == 'C' && signature[1] == 'T' && signature[2] == 'M' && signature[3] == 'F')
			{
				instrument_list = ReadFileCMF(reader);
			}
			else
				throw gcnew Exception("Instrument::ReadFile, unrecognised file header.");

			reader->Close();

			return instrument_list;
		}

		void Display()
		{
			Console::WriteLine("Instrument: {0}", name);
			Console::WriteLine(" operators: {0}", operators->Length);
			Console::WriteLine(" connection: {0}", operator_connection);
			for (int i = 0; i < operators->Length; i++)
			{
				Console::WriteLine(" Operator {0}", i);
				Console::WriteLine("   AM: {0}", operators[i]->amplitude_modulation);
				Console::WriteLine("   VB: {0}", operators[i]->vibrato);
				Console::WriteLine("   ET: {0}", operators[i]->envelope_type);
				Console::WriteLine("   KR: {0}", operators[i]->key_scale_rate);
				Console::WriteLine("   MT: {0}", operators[i]->multiplier);
				Console::WriteLine("   KL: {0}", operators[i]->key_scale_level);
				Console::WriteLine("   TL: {0}", operators[i]->total_level);
				Console::WriteLine("   AR: {0}", operators[i]->attack_rate);
				Console::WriteLine("   DR: {0}", operators[i]->decay_rate);
				Console::WriteLine("   SL: {0}", operators[i]->sustain_level);
				Console::WriteLine("   RR: {0}", operators[i]->release_rate);
				Console::WriteLine("   WF: {0}", operators[i]->waveform);
				Console::WriteLine("   FD: {0}", operators[i]->feedback);
			}
		}
	};

}
