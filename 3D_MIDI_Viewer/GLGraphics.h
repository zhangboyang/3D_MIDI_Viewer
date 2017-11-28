#pragma once

class GLGraphics {
private:
	int vsync = 1;
	int use_double_buffer = 1;
	int multisample_level = 0;
	int window_width = 800;
	int window_height = 600;
	std::string window_title = "3D MIDI Viewer";

	std::shared_ptr<MIDIObject> mobj;
	std::unique_ptr<MIDIPlayer> mplay;

	float axis_max = 1000.0f;

	float trect_x = 17.0f;
	float trect_y = 20.0f;
	float trect_a = 7.0f / 8.0f; float trect_a_delta = 1.0f / 8.0f;

	float seek_delta = 5.0f;

	float time_scale = 10.0f;

	int pause = 0;

	int note_mode = 3;

	DWORD raw_timer;
	DWORD last_timer;
	
public:
	static GLGraphics *Instance();
	void InitGlut(int *argcp, char **argv);
	void LoadMIDIObject(std::shared_ptr<MIDIObject> mobj);
	void BindMIDIPlayer(std::unique_ptr<MIDIPlayer> mplay);
	void Run();
private:
	GLGraphics();
	GLGraphics(const GLGraphics &) = delete;
    void operator = (const GLGraphics &) = delete;

	void Render();
	void Reshape(int width, int height);
	void KeyboardFunc(unsigned char key, int x, int y);

	float GetTimer();
	void ResetTimer(float value = 0.0f);
	void UpdateTimer();
	void CalibrateTimer();

	void Seek(float abs_time);

	void DrawAxis();
	void DrawTransparentRect();
};