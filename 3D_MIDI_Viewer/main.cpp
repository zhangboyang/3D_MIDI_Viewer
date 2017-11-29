#include "pch.h"


int main(int argc, char *argv[])
{
	printf("3D MIDI Viewer by ZBY\n");
	printf("\n");

	char midfile[MAX_PATH] = {};
	if (argc >= 2) {
		strncpy(midfile, argv[1], sizeof(midfile) - 1);
	}
	
	#if _DEBUG
	strcpy(midfile, "../../test.mid");
	#endif

	if (!midfile[0]) {
		OPENFILENAME ofn;
		memset(&ofn, 0, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = midfile;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(midfile);
		ofn.lpstrFilter = "MIDI Files\0*.MID\0All Files\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = ".";
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (!GetOpenFileName(&ofn)) {
			strcpy(midfile, "");
		}
	}
	if (!midfile[0]) {
		fail("No MIDI file.");
	}

	printf("MIDI File:\n  %s\n\n", midfile);
	std::unique_ptr<MIDIPlayer> mplay(new MIDIPlayer);
	mplay->LoadMIDIFile(midfile);

	std::unique_ptr<MIDIData> mdata(new MIDIData);
	mdata->LoadMIDIFile(midfile);
	//mdata->LoadTestSample();
	//mdata->LoadRandomTestSample();


	GLGraphics::Instance()->SetMIDIName(midfile);
	GLGraphics::Instance()->InitGlut(&argc, argv);

	std::unique_ptr<MIDIObject> mobj(new MIDIObject);
	mobj->LoadData(std::move(mdata));

	GLGraphics::Instance()->BindMIDIPlayer(std::move(mplay));
	GLGraphics::Instance()->LoadMIDIObject(std::move(mobj));

	GLGraphics::Instance()->Run();

	return 0;
}