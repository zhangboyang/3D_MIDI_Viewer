#include "pch.h"


void MIDIPlayer::LoadMIDIFile(const std::string filename)
{
	printf("\nLoading MIDI Player ...\n\n");
	CoInitialize(NULL);
	
	HRESULT hr;
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);
	if (FAILED(hr)) {
		fail("Can't create Filter Graph.");
	}
	hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
	if (FAILED(hr)) {
		fail("Can't query Media Control interface.");
	}
	hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
	if (FAILED(hr)) {
		fail("Can't query Media Event interface.");
	}
	hr = pGraph->QueryInterface(IID_IMediaSeeking, (void**)(&pSeek));
	if (FAILED(hr)) {
		fail("Can't query Media Seeking interface.");
	}
	
	hr = pGraph->RenderFile(cs2wcs(filename.c_str(), CP_ACP), NULL);
	if (FAILED(hr)) {
		fail("Can't render file.");
	}
}
void MIDIPlayer::Play()
{
	HRESULT hr = pControl->Run();
	if (FAILED(hr)) {
		printf("Can't play.\n");
	}
}
void MIDIPlayer::Pause()
{
	HRESULT hr = pControl->Pause();
	if (FAILED(hr)) {
		printf("Can't pause.\n");
	}
}
void MIDIPlayer::Stop()
{
	HRESULT hr = pControl->Stop();
	if (FAILED(hr)) {
		printf("Can't stop.\n");
	}
}
float MIDIPlayer::Seek(float time)
{
	float t = floor(time + eps);
	REFERENCE_TIME rtNow = ((unsigned long long) t) * 10000000;
	
	HRESULT hr = pSeek->SetPositions(&rtNow, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
	if (FAILED(hr)) {
		printf("Can't set position.\n");
	}
	return Tell();
}

float MIDIPlayer::Tell()
{
	REFERENCE_TIME rtNow;
	HRESULT hr = pSeek->GetCurrentPosition(&rtNow);
	if (FAILED(hr)) {
		printf("Can't get position.\n");
		rtNow = 0;
	}
	return rtNow / 10000000.0;
}
void MIDIPlayer::Reset()
{
	Stop();
	Seek(0.0f);
}


MIDIPlayer::~MIDIPlayer()
{
	SafeRelease(&pGraph);
	SafeRelease(&pControl);
	SafeRelease(&pEvent);
	SafeRelease(&pSeek);
}
