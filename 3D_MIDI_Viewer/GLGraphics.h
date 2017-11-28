#pragma once

class GLGraphics {
	static constexpr float AXIS_MAX = 100.0f;
private:
	int use_double_buffer = 1;
	int multisample_level = 0;
	int window_width = 500;
	int window_height = 500;
	std::string window_title = "helloworld";

	std::shared_ptr<MIDIObject> mobj;

	int pause = 0;

	DWORD raw_timer;
	DWORD last_timer;
	
public:
	static GLGraphics *Instance();
	void InitGlut(int *argcp, char **argv);
	void LoadMIDIObject(std::shared_ptr<MIDIObject> mobj);
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

	void DrawAxis();
};