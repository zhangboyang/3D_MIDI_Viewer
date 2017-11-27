#include "pch.h"

void MIDIData::LoadMIDIFile(const std::string filename)
{
	for (int ch = 0; ch < MAXCHANNEL; ch++) {
		data[ch].clear();
	}
	for (int ch = 0; ch < MAXCHANNEL; ch++) {
		data[ch].push_back(std::make_pair(rand() % 10, std::make_pair(ch, ch + 3 + rand() % 5)));
	}
}