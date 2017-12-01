#pragma once

class MIDIPlayer {
	IGraphBuilder *pGraph = nullptr;
	IMediaControl *pControl = nullptr;
	IMediaEvent *pEvent = nullptr;
	IMediaSeeking *pSeek = nullptr;
public:
	~MIDIPlayer();
	void LoadMIDIFile(const std::string filename);
	void Play();
	void Pause();
	void Stop();
	float Seek(float time);
	float Tell();
	void Reset();
};