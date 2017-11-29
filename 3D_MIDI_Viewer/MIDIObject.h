#pragma once

class MIDIObject {
	static constexpr float NOTE_WIDTH = 0.5f;
private:
	std::shared_ptr<MIDIData> mdata;

	std::vector<glm::vec3> vertex_data[MIDIData::MAXCHANNEL];
	GLuint vbuf[MIDIData::MAXCHANNEL];
	glm::vec3 vertex_color[MIDIData::MAXCHANNEL][2];

	std::vector<glm::vec3> bvertex_data;
	GLuint bvbuf;
public:
	float minx, maxx;
	float miny, maxy;
	float minz, maxz;
	
public:
	void LoadData(std::shared_ptr<MIDIData> mdata);
	void Render(int color = 0);
	void RenderBox();
	void RandomColor();
private:
	void MakeBox();
};