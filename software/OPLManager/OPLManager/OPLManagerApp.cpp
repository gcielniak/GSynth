#include "OPL.h"

using namespace System;
using namespace System::Collections::Generic;

int main(array<System::String ^> ^args)
{
	String^ system_path = "H:\\Greg\\Downloads\\AdlibTracker2_SDL_2.4.13\\instr\\";

	array<String^>^ files = IO::Directory::GetFiles(String::Concat(system_path,"1198SBI\\"));

	List<OPL::Instrument^>^ instrument_list = gcnew List<OPL::Instrument^>();

	try
	{
		for each (String^ var in files)
		{
			instrument_list->AddRange(OPL::Instrument::ReadFile(var));
		}

		instrument_list->AddRange(OPL::Instrument::ReadFile(String::Concat(system_path,"$gmopl.ibk")));

		instrument_list->AddRange(OPL::Instrument::ReadFile(String::Concat(system_path,"1SOPRANO\\SOPRANO1.cmf")));
	}
	catch (Exception^ exc)
	{
		Console::WriteLine(exc->Message);
	}

	for each (OPL::Instrument^ inst in instrument_list)
		inst->Display();

	for each (OPL::Instrument^ inst1 in instrument_list)
	for each (OPL::Instrument^ inst2 in instrument_list)
		if (inst1->Compare(inst2) == 0)
			Console::WriteLine("{0} == {1}",inst1->name, inst2->name);

	return 0;
}