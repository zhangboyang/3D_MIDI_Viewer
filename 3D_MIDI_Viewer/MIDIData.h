#pragma once

class MIDIData {
	static const size_t MAXCHANNEL = 16;
	friend class MIDIObject;
private:
	std::vector<std::pair<int, std::pair<int, int> > > data[MAXCHANNEL];
public:
	void LoadMIDIFile(const std::string filename);
};
