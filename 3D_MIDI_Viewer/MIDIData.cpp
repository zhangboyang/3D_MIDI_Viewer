#include "pch.h"


MIDIBuffer::MIDIBuffer(std::shared_ptr<std::vector<unsigned char> > buffer, size_t offset, size_t length) : buffer(buffer), offset(offset), length(length)
{
}

std::unique_ptr<MIDIBuffer> MIDIBuffer::SubBuffer(size_t length, bool advance)
{
	if (length > this->length - pointer) {
		throw InvalidMIDIFileException();
	}
	std::unique_ptr<MIDIBuffer> ret(new MIDIBuffer(buffer, this->offset + pointer, length));

	if (advance) pointer += length;
	return ret;
}

void MIDIBuffer::Read(void *buffer, size_t length, bool advance)
{
	if (pointer + length < pointer || pointer + length > this->length) {
		throw InvalidMIDIFileException();
	}

	if (buffer) memcpy(buffer, this->buffer->data() + offset + pointer, length);
	if (advance) pointer += length;
}

uint32_t MIDIBuffer::ReadVLQ()
{
	uint32_t r = 0;
	for (int i = 0; i < 4; i++) {
		unsigned char b;
		Read(&b, 1, 1);
		int flag = b >> 7;
		if (flag) {
			if (i == 3) {
				throw InvalidMIDIFileException();
			}
			b &= 0x7F;
		}
		r = (r << 7) | b;
		if (!flag) {
			break;
		}
	}
	return r;
}

bool MIDIBuffer::Eof()
{
	return pointer >= length;
}

size_t MIDIBuffer::Length()
{
	return length;
}
void MIDIBuffer::Dump()
{
	printf(" ");
	for (size_t i = 0; i < length; i++) {
		unsigned ch = (*buffer)[offset + i];
		printf(" %02X", ch);
	}
	printf("\n  ");
	for (size_t i = 0; i < length; i++) {
		unsigned ch = (*buffer)[offset + i];
		printf("%c", isprint(ch) ? ch : '.');
	}
	printf("\n");
}

void MIDIData::LoadMIDIFile(const std::string filename)
{	
	for (auto &cdata: data) cdata.clear();
	bar.clear();
	tempo_map.clear();
	max_time = 0.0;
	max_raw_time = 0;
	max_end_of_track = 0.0;

	std::shared_ptr<std::vector<unsigned char> > fdata_buffer(new std::vector<unsigned char>);

	FILE *fp = fopen(filename.c_str(), "rb");
	if (!fp) {
		fail("Can't open MIDI file.\n");
	}
	char buf[MAXLINE];
	int len;
	while ((len = fread(buf, 1, sizeof(buf), fp)) > 0) {
		fdata_buffer->insert(fdata_buffer->end(), buf, buf + len);
	}
	fclose(fp);

	try {
		std::unique_ptr<MIDIBuffer> fdata(new MIDIBuffer(fdata_buffer, 0, fdata_buffer->size()));
		fdata_buffer.reset();


		std::vector<unsigned char> buf;

		struct chunk_hdr {
			uint32_t type;
			uint32_t length;
		};

		int trkid = 0;
		while (!fdata->Eof()) {
			chunk_hdr h;

			fdata->Read(&h, sizeof(h), 1);
			printf("MIDI Chunk: %.4s ", (char *) &h.type);
			h.type = be32toh(h.type);
			h.length = be32toh(h.length);
			printf("%08X %08X\n", h.type, h.length);

			std::unique_ptr<MIDIBuffer> cdata = fdata->SubBuffer(h.length, 1);

			switch (h.type) {
			case TYPE_MThd:
				LoadMThd(std::move(cdata));
				break;
			case TYPE_MTrk:
				LoadMTrk(std::move(cdata), ++trkid);
				break;
			default:
				printf(" Unknown MIDI Chunk.\n");
			}
		}
		
		CalcBars();

		printf(" End of all tracks: %fs\n", max_end_of_track);
	} catch (UnsupportedMIDIFileException) {
		fail("Unsupported MIDI file.");
	} catch (InvalidMIDIFileException) {
		fail("Invalid MIDI file.");
	}
}

void MIDIData::LoadMThd(std::unique_ptr<MIDIBuffer> mthd)
{
	struct mthd_data {
		uint16_t format;
		uint16_t tracks;
		uint16_t division;
	};

	mthd_data h;
	mthd->Read(&h, sizeof(h), 1);

	h.format = be16toh(h.format);
	h.tracks = be16toh(h.tracks);
	h.division = be16toh(h.division);

	printf(" Format: %04X\n", (unsigned) h.format);
	printf(" Tracks: %04X\n", (unsigned) h.tracks);
	printf(" Division: %04X\n", (unsigned) h.division);

	division = h.division;

	//system("pause");

	if (h.format == 0) {
		if (h.tracks != 1) {
			throw InvalidMIDIFileException();
		}
	} else if (h.format == 2) {
		printf(" MIDI format 2 is not supported!\n");
		throw UnsupportedMIDIFileException();
	}
	if (!((h.division >> 15) == 0)) {
		printf(" SMTPE is not supported!\n");
		throw UnsupportedMIDIFileException();
	}
}
void MIDIData::LoadMTrk(std::unique_ptr<MIDIBuffer> mtrk, int trkid)
{
	unsigned cur_tempo = 500000;
	unsigned nn = 4, dd = 4;
	if (trkid == 1) {
		tempo_map[0] = std::make_pair(cur_tempo, 0.0);
	}


	double cur_time = 0;
	std::map<std::pair<unsigned, unsigned>, double> k; // <channel, key> -> start time

	auto DoSetTempo = [&](unsigned new_tempo) {
		cur_tempo = new_tempo;
	};



	unsigned char last_status_byte = 0;

	unsigned long long raw_time = 0;
	
	while (!mtrk->Eof()) {
		unsigned long long raw_delta_time = mtrk->ReadVLQ();
		raw_time += raw_delta_time;

		if (trkid == 1) {
			double delta_time = raw_delta_time * cur_tempo / 1000000.0 / division;
			cur_time += delta_time;
		} else {
			auto it = tempo_map.upper_bound(raw_time);
			it--;
			DoSetTempo(it->second.first);
			cur_time = it->second.second + (raw_time - it->first) * cur_tempo / 1000000.0 / division;
		}
		
		max_time = std::max(max_time, cur_time);
		max_raw_time = std::max(max_raw_time, raw_time);

		unsigned char status_byte;
		mtrk->Read(&status_byte, 1, 0);
		if (status_byte >> 7) {
			last_status_byte = status_byte;
			mtrk->Read(nullptr, 1, 1);
		} else {
			status_byte = last_status_byte;
		}
		switch (status_byte) {
		case 0xF0: case 0xF7: { // Sysex Event
			unsigned char sysex_hdr[2] = { status_byte };
			mtrk->Read(sysex_hdr + 1, sizeof(sysex_hdr) - 1, 0);
			//printf("Sysex Event:\n"); mtrk->SubBuffer(sizeof(sysex_hdr) + sysex_hdr[1], 0)->Dump();
			mtrk->Read(nullptr, sizeof(sysex_hdr) - 1 + sysex_hdr[1], 1);
			break;
		}
		case 0xFF: { // Meta Event
			unsigned char meta_hdr[3] = { status_byte };
			mtrk->Read(meta_hdr + 1, sizeof(meta_hdr) - 1, 0);
			switch (meta_hdr[1]) {
			case 0x2F: {
				if (meta_hdr[2] != 0x00) {
					throw InvalidMIDIFileException();
				}
				mtrk->Read(nullptr, sizeof(meta_hdr) - 1, 1);
				printf(" End of this track: %fs\n", cur_time);
				max_end_of_track = std::max(max_end_of_track, cur_time);
				break;
			}
			case 0x51: { // set tempo
				if (meta_hdr[2] != 0x03) {
					throw InvalidMIDIFileException();
				}
				mtrk->Read(nullptr, sizeof(meta_hdr) - 1, 1);
				unsigned char byte_val;
				unsigned new_tempo = 0;
				for (int i = 0; i < 3; i++) {
					mtrk->Read(&byte_val, 1, 1);
					new_tempo = (new_tempo << 8) | byte_val;
				}
				
				DoSetTempo(new_tempo);
				if (trkid == 1) {
					tempo_map[raw_time] = std::make_pair(new_tempo, cur_time);
				}

				printf(" (%-6.2fs) Set Tempo: %u\n", cur_time, cur_tempo);
				//system("pause");

				break;
			}
			case 0x58: {
				if (meta_hdr[2] != 0x04) {
					throw InvalidMIDIFileException();
				}
				mtrk->Read(nullptr, sizeof(meta_hdr) - 1, 1);
				unsigned char ts[4];
				mtrk->Read(&ts, sizeof(ts), 1);
				nn = ts[0];
				dd = 1 << ts[1];
				if (trkid == 1) {
					ts_map[raw_time] = std::make_pair(nn, dd);
				}
				printf(" (%-6.2fs) Time Signature: %u/%u\n", cur_time, nn, dd);
				break;
			}
			default:
				//printf("Meta Event:\n"); mtrk->SubBuffer(sizeof(meta_hdr) - 1 + meta_hdr[2], 0)->Dump();
				mtrk->Read(nullptr, sizeof(meta_hdr) - 1 + meta_hdr[2], 1);
			}
			break;
		}
		default:
			switch (status_byte >> 4) {
			case 0x8:
			case 0x9: {
				unsigned char note_data[3] = { status_byte };
				mtrk->Read(note_data + 1, sizeof(note_data) - 1, 1);
				//printf("%llu Note %s: %02X %02X %02X\n", raw_time, (status_byte >> 4) == 0x8 ? "off" : "on", (unsigned) note_data[0], (unsigned) note_data[1], (unsigned) note_data[2]);
				//system("pause");
				unsigned channel = status_byte & 0xF;
				unsigned notekey = note_data[1];
				unsigned velocity = note_data[2];

				if ((status_byte >> 4) == 0x8 || velocity == 0) {
					auto it = k.find(std::make_pair(channel, notekey));
					if (it == k.end()) {
						//printf("  Unmatched note off.\n");
						//throw InvalidMIDIFileException();
					} else {
						//printf("  (%u, %u, %f, %f)\n", channel, notekey, it->second, cur_time);
						// channel, notekey -> time: (it->second, cur_time)
						assert(channel < MAXCHANNEL);
						data[channel].push_back(std::make_pair(notekey, std::make_pair(it->second, cur_time)));

						k.erase(it);
					}
				} else { // on
					if (!k.insert(std::make_pair(std::make_pair(channel, notekey), cur_time)).second) {
						//printf("  Multiple note on detected.\n");
						//throw InvalidMIDIFileException();
					}
				}

				break;
			}
			case 0xA:
			case 0xB:
			case 0xE: {
				// printf("Ignored:\n"); mtrk->SubBuffer(2, 0)->Dump();
				mtrk->Read(nullptr, 2, 1);
				break;
			}
			case 0xC:
			case 0xD: {
				// printf("Ignored:\n"); mtrk->SubBuffer(1, 0)->Dump();
				mtrk->Read(nullptr, 1, 1);
				break;
			}

			default:
				printf(" Unknown Event %02X.\n", (unsigned) status_byte);
				throw InvalidMIDIFileException();
			}
		}
	}
}

void MIDIData::CalcBars()
{
	unsigned cur_tempo = 500000;
	unsigned nn = 4, dd = 4;

	std::vector<unsigned long long> time_list;
	for (auto &st: tempo_map) time_list.push_back(st.first);
	for (auto &ts: ts_map) time_list.push_back(ts.first);
	time_list.push_back(max_raw_time);
	std::sort(time_list.begin(), time_list.end());
	time_list.resize(std::unique(time_list.begin(), time_list.end()) - time_list.begin());

	unsigned long long last_bar_raw_time = 0;
	unsigned long long last_raw_time = 0;
	double cur_time = 0.0;



	for (auto raw_time: time_list) {
		//printf("raw_time = %llu\n", raw_time);
		unsigned long long raw_delta_time = raw_time - last_raw_time;
		last_raw_time = raw_time;
		double delta_time = raw_delta_time * cur_tempo / 1000000.0 / division;
		cur_time += delta_time;

		unsigned long long bar_len_raw = (unsigned long long) division * nn / (dd / 4);
		
		while (last_bar_raw_time + bar_len_raw < raw_time) {
			last_bar_raw_time += bar_len_raw;
			
			double bar_time = cur_time - (raw_time - last_bar_raw_time) * cur_tempo / 1000000.0 / division;
			bar.push_back(bar_time);
			//printf("raw_time %llu, last_bar_raw_time %llu, BAR: %f\n", raw_time, last_bar_raw_time, bar_time);
		}

		{ // check time signature
			auto it = ts_map.find(raw_time);
			if (it != ts_map.end()) {
				nn = it->second.first;
				dd = it->second.second;
			}
			last_bar_raw_time = raw_time;
			bar.push_back(cur_time);
		}
		{ // check set tempo
			auto it = tempo_map.find(raw_time);
			if (it != tempo_map.end()) {
				cur_tempo = it->second.first;
			}
			bar.push_back(cur_time);
		}
	}
}

void MIDIData::LoadTestSample()
{
	for (auto &cdata: data) cdata.clear();
	for (int ch = 0; ch < MAXCHANNEL; ch++) {
		data[ch].push_back(std::make_pair(MAXCHANNEL - ch - 1, std::make_pair(ch, ch + 3)));
	}
}

void MIDIData::LoadRandomTestSample()
{
	for (auto &cdata: data) cdata.clear();
	for (int ch = 0; ch < MAXCHANNEL; ch++) {
		data[ch].push_back(std::make_pair(rand() % 10, std::make_pair(ch, ch + 3 + rand() % 5)));
	}
}
