#include "pch.h"


void MIDIPlayer::LoadMIDIFile(const std::string filename)
{
	printf("\nLoading MIDI Player ...\n\n");
	DWORD r;
	MCI_OPEN_PARMS mciOpenParms;
	mciOpenParms.lpstrDeviceType = (LPCSTR) MCI_DEVTYPE_SEQUENCER;
    mciOpenParms.lpstrElementName = filename.c_str();

	r = mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID | MCI_OPEN_ELEMENT, (DWORD_PTR) &mciOpenParms);
	if (r) {
		fail("Can't open MIDI device.");
    }

	dev = mciOpenParms.wDeviceID;
	
	MCI_SET_PARMS mciSetParms;
	mciSetParms.dwTimeFormat = MCI_FORMAT_MILLISECONDS;
	r = mciSendCommand(dev, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR) &mciSetParms);
	if (r) {
		printf("Can't set time format.\n");
	}
}
void MIDIPlayer::Play()
{
//return;
	DWORD r;
	r = mciSendCommand(dev, MCI_PLAY, 0, NULL);
	if (r) {
		printf("Can't play.\n");
	}
}
void MIDIPlayer::Pause()
{
	DWORD r;
	r = mciSendCommand(dev, MCI_PAUSE, 0, NULL);
	if (r) {
		printf("Can't pause.\n");
	}
}
void MIDIPlayer::Stop()
{
	DWORD r;
	r = mciSendCommand(dev, MCI_STOP, 0, NULL);
	if (r) {
		printf("Can't stop.\n");
	}
}
void MIDIPlayer::Seek(float time)
{
	MCI_SEEK_PARMS mciSeekParms;
	DWORD r;
	mciSeekParms.dwTo = (DWORD) (time * 1000.0f);
	r = mciSendCommand(dev, MCI_SEEK | MCI_WAIT, MCI_TO, (DWORD_PTR) &mciSeekParms);
	if (r) {
		printf("Can't seek.\n");
	}
}


void MIDIPlayer::Reset()
{
	Stop();
	Seek(0.0f);
}

//FIXME:
//mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);