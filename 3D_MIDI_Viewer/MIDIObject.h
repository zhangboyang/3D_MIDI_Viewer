#pragma once

class MIDIObject {
	static constexpr float NOTE_WIDTH = 0.5f;
	struct note_vertex {
		glm::vec3 pos;
		glm::vec3 color;
	};
private:
	std::shared_ptr<MIDIData> mdata;
	std::vector<note_vertex> vertex_data;
	std::vector<unsigned> indice_data;
	GLuint vbuf;
	GLuint ibuf;
public:
	void LoadData(std::shared_ptr<MIDIData> mdata);
	void Render();
private:
};