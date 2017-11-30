#pragma once

class MIDIPlayer {
	IGraphBuilder *pGraph;
	IMediaControl *pControl;
	IMediaEvent *pEvent;
	IMediaSeeking *pSeek;
public:
	void LoadMIDIFile(const std::string filename);
	void Play();
	void Pause();
	void Stop();
	float Seek(float time);
	float Tell();
	void Reset();
};