#pragma once

class MIDIObject {
	static constexpr float NOTE_WIDTH = 0.5f;
	static constexpr float NOTE_HEIGHT = 1.0f;
private:
	std::shared_ptr<MIDIData> mdata;

	std::vector<glm::vec3> vertex_data[2][MIDIData::MAXCHANNEL]; // [roll][ch]
	GLuint vbuf[2][MIDIData::MAXCHANNEL];
	glm::vec3 vertex_color[MIDIData::MAXCHANNEL][2]; // [ch][color]

	std::vector<glm::vec3> bvertex_data;
	GLuint bvbuf;
public:
	float minx, maxx;
	float miny, maxy;
	float minz, maxz;
	
public:
	void LoadData(std::shared_ptr<MIDIData> mdata);
	void Render(int roll = 0, int color = 0);
	void RenderBox();
	void RandomColor();
private:
	void MakeBox();
};