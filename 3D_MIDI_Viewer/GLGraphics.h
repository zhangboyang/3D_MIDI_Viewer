#pragma once

class GLGraphics {
	static const int MAX_MOUSE_BUTTON = 5;
	static constexpr float FPS_UPDATE = 0.5f;
private:
	int vsync = 1;
	int use_double_buffer = 1;
	int multisample_level = 0;
	int window_width = 800;
	int window_height = 600;
	std::string window_title;

	std::shared_ptr<MIDIObject> mobj;
	std::unique_ptr<MIDIPlayer> mplay;

	glm::vec3 eye;
	glm::vec3 eyecenter = glm::vec3(12.0f, 12.0f, 0.0f);
	glm::vec3 eyenormal, eyez;
	float yaw = 5.15f, pitch = 6.00f;

	float eyedistance = 35.0f;
	float eyedistance_max = 200.0f;
	float eyedistance_min = 5.0f;
	float eyedistance_delta = 5.0f;


	float axis_max = 1000.0f;

	float trect_x = 16.5f;
	float trect_y = 20.0f;
	float trect_a = 7.0f / 8.0f; float trect_a_delta = 1.0f / 8.0f;

	float seek_delta = 1.0f;

	float time_scale = 10.0f;
	float time_scale_factor = 1.1f;
	float time_scale_min = 1.0f;
	float time_scale_max = 100.0f;
	
	int pause = 0;

	int note_mode = 3;

	int draw_box = 1;
	int draw_axis = 1;
	int draw_msg = 1;

	int mouse_button[MAX_MOUSE_BUTTON];
	int mouse_buttonX[MAX_MOUSE_BUTTON];
	int mouse_buttonY[MAX_MOUSE_BUTTON];

	float mouse_yaw, mouse_pitch;
	float mouse_rotate_factor = 0.0f;

	glm::vec3 mouse_eyecenter;
	float mouse_eyecenter_factor = 0.0f;
	
	std::string msg;

	DWORD raw_timer = 0;
	DWORD last_timer = 0;

	DWORD last_fps_time = 0;
	unsigned frame_cnt = 0;
	float fps = 0.0f;
	
public:
	static GLGraphics *Instance();
	void SetMIDIName(const std::string &midname);
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
	void MouseFunc(int button, int state, int x, int y);
	void MotionFunc(int x, int y);

	float GetTimer();
	void ResetTimer(float value = 0.0f);
	void UpdateTimer();
	void CalibrateTimer();

	void Seek(float abs_time);

	void DrawAxis();
	void DrawTransparentRect();

	void CalcEyeVector();

	void CalcFPS();

	void PrintUsage();
	
};