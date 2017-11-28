#pragma once

class InvalidMIDIFileException {
};
class UnsupportedMIDIFileException : public InvalidMIDIFileException{
};

class MIDIBuffer {
private:
	std::shared_ptr<std::vector<unsigned char> > buffer;
	size_t offset;
	size_t length;
	size_t pointer = 0;
public:
	MIDIBuffer(std::shared_ptr<std::vector<unsigned char> > buffer, size_t offset, size_t length);
	std::unique_ptr<MIDIBuffer> SubBuffer(size_t length, bool advance);
	void Read(void *buffer, size_t length, bool advance);
	uint32_t ReadVLQ();
	bool Eof();
	size_t Length();
	void Dump();
};

class MIDIData {
	static const size_t MAXCHANNEL = 16;
	enum {
		TYPE_MThd = 0x4D546864,
		TYPE_MTrk = 0x4D54726B,
	};

	friend class MIDIObject;
private:
	std::vector<std::pair<int, std::pair<double, double> > > data[MAXCHANNEL]; // data[channel] = (note, (start_time, end_time))
	unsigned division;

public:
	void LoadMIDIFile(const std::string filename);
	void LoadTestSample();
	void LoadRandomTestSample();
private:
	void LoadMThd(std::unique_ptr<MIDIBuffer> mthd);
	void LoadMTrk(std::unique_ptr<MIDIBuffer> mtrk);
};
