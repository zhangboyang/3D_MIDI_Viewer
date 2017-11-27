#include "pch.h"


int main(int argc, char *argv[])
{
	std::shared_ptr<MIDIData> mdata(new MIDIData);
	mdata->LoadMIDIFile("test.mid");


	GLGraphics::Instance()->InitGlut(&argc, argv);

	std::shared_ptr<MIDIObject> mobj(new MIDIObject);
	mobj->LoadData(mdata);

	GLGraphics::Instance()->LoadMIDIObject(mobj);

	mdata.reset();
	mobj.reset();

	GLGraphics::Instance()->Run();

	return 0;
}