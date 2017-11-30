#include "pch.h"


void MIDIPlayer::LoadMIDIFile(const std::string filename)
{
	printf("\nLoading MIDI Player ...\n\n");
	CoInitialize(NULL);
	
	HRESULT hr;
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);

	hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
	hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
	hr = pGraph->QueryInterface(IID_IMediaSeeking, (void**)(&pSeek));
	
	hr = pGraph->RenderFile(cs2wcs(filename.c_str(), CP_ACP), NULL);


}
void MIDIPlayer::Play()
{
	pControl->Run();
}
void MIDIPlayer::Pause()
{
	pControl->Pause();
}
void MIDIPlayer::Stop()
{
	pControl->Stop();
}
float MIDIPlayer::Seek(float time)
{
	float t = time;
	REFERENCE_TIME rtNow = ((unsigned long long) t) * 10000000;
	
	pSeek->SetPositions(&rtNow, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
	return Tell();
}

float MIDIPlayer::Tell()
{
	REFERENCE_TIME rtNow;
	pSeek->GetCurrentPosition(&rtNow);
	return rtNow / 10000000.0;
}
void MIDIPlayer::Reset()
{
	Stop();
	Seek(0.0f);
}

//FIXME:
//mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);