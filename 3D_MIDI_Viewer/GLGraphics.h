#pragma once

class GLGraphics {
private:
	int use_double_buffer = 10;
	int multisample_level = 0;
	int window_width = 500;
	int window_height = 500;
	std::string window_title = "helloworld";

public:
	static GLGraphics *Instance();
	void InitGlut(int *argcp, char **argv);
	void Run();
private:
	GLGraphics();
	GLGraphics(const GLGraphics &) = delete;
    void operator = (const GLGraphics &) = delete;

	void Render();
	void Reshape(int width, int height);
};