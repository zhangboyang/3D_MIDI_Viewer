#include "pch.h"

void MIDIObject::RandomColor()
{
	for (int ch = 0; ch < MIDIData::MAXCHANNEL; ch++) {
		vertex_color[ch][0] = glm::linearRand(glm::vec3(0.5f), glm::vec3(1.0f));
		vertex_color[ch][1] = vertex_color[ch][0] * 0.6f;
	}
}
void MIDIObject::LoadData(std::shared_ptr<MIDIData> mdata)
{
	for (auto &i : vertex_data) {
		for (auto &j : i) {
			j.clear();
		}
	}
	this->mdata = mdata;
	
	RandomColor();

	for (int ch = 0; ch < MIDIData::MAXCHANNEL; ch++) {
		auto &cdata = mdata->data[ch];

		for (auto &ndata : cdata) {
			int note = ndata.first;
			double st = ndata.second.first, ed = ndata.second.second;

			vertex_data[0][ch].push_back(glm::vec3(ch, note + NOTE_HEIGHT / 2.0f, st));
			vertex_data[0][ch].push_back(glm::vec3(ch, note + NOTE_HEIGHT / 2.0f, ed));
			vertex_data[0][ch].push_back(glm::vec3(ch + NOTE_WIDTH, note + NOTE_HEIGHT / 2.0f, ed));
			vertex_data[0][ch].push_back(glm::vec3(ch + NOTE_WIDTH, note + NOTE_HEIGHT / 2.0f, st));

			vertex_data[1][ch].push_back(glm::vec3(ch + NOTE_WIDTH / 2.0f, note, st));
			vertex_data[1][ch].push_back(glm::vec3(ch + NOTE_WIDTH / 2.0f, note, ed));
			vertex_data[1][ch].push_back(glm::vec3(ch + NOTE_WIDTH / 2.0f, note + NOTE_HEIGHT, ed));
			vertex_data[1][ch].push_back(glm::vec3(ch + NOTE_WIDTH / 2.0f, note + NOTE_HEIGHT, st));
		}
	}

	minx = miny = minz = std::numeric_limits<float>::infinity();
	maxx = maxy = maxz = -std::numeric_limits<float>::infinity();
	for (auto &i: vertex_data) {
		for (auto &j: i) {
			for (auto &v: j) {
				minx = std::min(minx, v.x);
				miny = std::min(miny, v.y);
				minz = std::min(minz, v.z);
				maxx = std::max(maxx, v.x);
				maxy = std::max(maxy, v.y);
				maxz = std::max(maxz, v.z);
			}
		}
	}

    float xborder = (maxx - minx) * 0.05f;
    minx -= xborder;
    maxx += xborder;
    float yborder = (maxy - miny) * 0.05f;
    miny -= yborder;
    maxy += yborder;

	printf("MIDI 3D Object:\n");

	glGenBuffers(MIDIData::MAXCHANNEL, vbuf[0]);
	glGenBuffers(MIDIData::MAXCHANNEL, vbuf[1]);
	
	double vsize = 0.0;
	for (int r = 0; r < 2; r++) {
		for (int ch = 0; ch < MIDIData::MAXCHANNEL; ch++) {
			glBindBuffer(GL_ARRAY_BUFFER, vbuf[r][ch]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data[r][ch]) * vertex_data[r][ch].size(), vertex_data[r][ch].data(), GL_STATIC_DRAW);
			vsize += sizeof(vertex_data[r][ch][0]) * vertex_data[r][ch].size() / 1024.0;
		}
	}
	printf(" Note Vertex: %.3f KB\n", vsize);

	MakeBox();

	printf(" Box Vertex: %.3f KB\n", sizeof(bvertex_data[0]) * bvertex_data.size() / 1024.0);
}


void MIDIObject::Render(int roll, int color)
{
	glEnableClientState(GL_VERTEX_ARRAY);

	for (int ch = 0; ch < MIDIData::MAXCHANNEL; ch++) {
		glColor3fv(&vertex_color[ch][color][0]);
		glBindBuffer(GL_ARRAY_BUFFER, vbuf[roll][ch]);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glDrawArrays(GL_QUADS, 0, vertex_data[roll][ch].size());
	}
}


void MIDIObject::RenderBox()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, bvbuf);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glDrawArrays(GL_QUADS, 0, bvertex_data.size());
}

void MIDIObject::MakeBox()
{
	bvertex_data.clear();
	float boxminz = 0.0f, boxmaxz = maxz;

	auto AddBoxX = [&](double y1, double z1, double y2, double z2, double x){
		bvertex_data.push_back(glm::vec3(x, y1, z1));
		bvertex_data.push_back(glm::vec3(x, y1, z2));
		bvertex_data.push_back(glm::vec3(x, y2, z2));
		bvertex_data.push_back(glm::vec3(x, y2, z1));
	};
	AddBoxX(miny, boxminz, maxy, boxmaxz, minx);
	AddBoxX(miny, boxminz, maxy, boxmaxz, maxx);

	auto AddBoxZ = [&](double x1, double y1, double x2, double y2, double z){
		bvertex_data.push_back(glm::vec3(x1, y1, z));
		bvertex_data.push_back(glm::vec3(x1, y2, z));
		bvertex_data.push_back(glm::vec3(x2, y2, z));
		bvertex_data.push_back(glm::vec3(x2, y1, z));
	};

	AddBoxZ(minx, miny, maxx, maxy, boxminz);
	AddBoxZ(minx, miny, maxx, maxy, boxmaxz);

	// draw bars
	for (auto &t: mdata->bar) {
		AddBoxZ(minx, miny, maxx, maxy, t);
	}

	glGenBuffers(1, &bvbuf);
	glBindBuffer(GL_ARRAY_BUFFER, bvbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bvertex_data[0]) * bvertex_data.size(), bvertex_data.data(), GL_STATIC_DRAW);

}