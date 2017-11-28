#include "pch.h"

void MIDIObject::LoadData(std::shared_ptr<MIDIData> mdata)
{
	this->mdata = mdata;
	
	glGenBuffers(1, &vbuf);
	glGenBuffers(1, &ibuf);

	for (int ch = 0; ch < MIDIData::MAXCHANNEL; ch++) {
		auto &cdata = mdata->data[ch];
		note_vertex v;
		v.color = glm::linearRand(glm::vec3(0.5f), glm::vec3(1.0f));
		v.color2 = v.color * 0.6f;

		for (auto &ndata : cdata) {
			int note = ndata.first;
			double st = ndata.second.first, ed = ndata.second.second;

			size_t index = vertex_data.size();
			v.pos = glm::vec3(ch, note, st);
			vertex_data.push_back(v);
			v.pos = glm::vec3(ch, note, ed);
			vertex_data.push_back(v);
			v.pos = glm::vec3(ch + NOTE_WIDTH, note, st);
			vertex_data.push_back(v);
			v.pos = glm::vec3(ch + NOTE_WIDTH, note, ed);
			vertex_data.push_back(v);

			indice_data.push_back(index);
			indice_data.push_back(index + 1);
			indice_data.push_back(index + 3);
			indice_data.push_back(index + 2);
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data[0]) * vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indice_data[0]) * indice_data.size(), indice_data.data(), GL_STATIC_DRAW);
	
	printf("MIDI 3D Object:\n");
	printf(" Vertex: %.3f KB\n", sizeof(vertex_data[0]) * vertex_data.size() / 1024.0);
	printf(" Indice: %.3f KB\n", sizeof(indice_data[0]) * indice_data.size() / 1024.0);
}


void MIDIObject::Render(bool color2)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf);

	glVertexPointer(3, GL_FLOAT, sizeof(note_vertex), (const void *) offsetof(note_vertex, pos));
	if (color2) {
		glColorPointer(3, GL_FLOAT, sizeof(note_vertex), (const void *) offsetof(note_vertex, color2));
	} else {
		glColorPointer(3, GL_FLOAT, sizeof(note_vertex), (const void *) offsetof(note_vertex, color));
	}

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
	glDrawElements(GL_QUADS, indice_data.size(), GL_UNSIGNED_INT, (const void *) 0);
}
