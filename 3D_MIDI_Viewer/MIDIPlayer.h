#pragma once

class MIDIPlayer {
	MCIDEVICEID dev;
public:
	void LoadMIDIFile(const std::string filename);
	void Play();
	void Pause();
	void Stop();
	void Seek(float time);
	void Reset();
};