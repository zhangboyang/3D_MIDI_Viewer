#include "pch.h"

GLGraphics::GLGraphics()
{
	for (auto &s: mouse_button) s = 1;
}

GLGraphics *GLGraphics::Instance()
{
	static GLGraphics instance;
	return &instance;
}

void GLGraphics::InitGlut(int *argcp, char **argv)
{
	glutInit(argcp, argv);
	
	unsigned int mode = GLUT_RGBA | GLUT_DEPTH;
    mode |= use_double_buffer ? GLUT_DOUBLE : GLUT_SINGLE;
    if (multisample_level) {
        glutSetOption(GLUT_MULTISAMPLE, multisample_level);
        mode |= GLUT_MULTISAMPLE;
    }
    glutInitDisplayMode(mode);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow(window_title.c_str());

	typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALPROC)(int);
	PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress( "wglSwapIntervalEXT" );
	if (wglSwapIntervalEXT) wglSwapIntervalEXT(vsync);

	printf("OpenGL Information:\n");

	const char *glstr;
    glstr = (const char *) glGetString(GL_VENDOR); printf(" Vendor: %s\n", glstr ? glstr : "null");
    glstr = (const char *) glGetString(GL_RENDERER); printf(" Renderer: %s\n", glstr ? glstr : "null");
    glstr = (const char *) glGetString(GL_VERSION); printf(" Version: %s\n", glstr ? glstr : "null");
    glstr = (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION); printf(" Shading: %s\n", glstr ? glstr : "null");
 
	GLenum glewerr = glewInit();
    if (glewerr != GLEW_OK) {
        fail("glewInit() failed : %s", glewGetErrorString(glewerr));
    }
    if (!(GLEW_VERSION_1_5)) {
        printf("WARNING: OpenGL version is too low to run this program. (OpenGL 1.5 required)");
    }


	glutDisplayFunc([](){ GLGraphics::Instance()->Render(); });
	glutIdleFunc([](){ GLGraphics::Instance()->Render(); });
	glutReshapeFunc([](int w, int h){ GLGraphics::Instance()->Reshape(w, h); });
	glutKeyboardFunc([](unsigned char k, int x, int y){ GLGraphics::Instance()->KeyboardFunc(k, x, y); });
	glutMouseFunc([](int b, int s, int x, int y){ GLGraphics::Instance()->MouseFunc(b, s, x, y); });
	glutMotionFunc([](int x, int y){ GLGraphics::Instance()->MotionFunc(x, y); });

}

void GLGraphics::Run()
{
	PrintUsage();

	mplay->Play();
	ResetTimer();
	Reshape(window_width, window_height);
	CalcFPS();
	glutMainLoop();
}

float GLGraphics::GetTimer()
{
	return raw_timer / 1000.0f;
}

void GLGraphics::ResetTimer(float value)
{
	raw_timer = (DWORD) (value * 1000.0f);
	last_timer = timeGetTime();
}

void GLGraphics::UpdateTimer()
{
	DWORD now_timer = timeGetTime();
	if (!pause) raw_timer += now_timer - last_timer;
	last_timer = now_timer;
}

void GLGraphics::CalibrateTimer()
{
	ResetTimer(GetTimer());
}

void GLGraphics::LoadMIDIObject(std::shared_ptr<MIDIObject> mobj)
{
	this->mobj = mobj;
}
void GLGraphics::BindMIDIPlayer(std::unique_ptr<MIDIPlayer> mplay)
{
	this->mplay = std::move(mplay);
}

void GLGraphics::CalcFPS()
{
	DWORD cur_time = timeGetTime();
	
	if (cur_time - last_fps_time >= FPS_UPDATE * 1000) {
		fps = frame_cnt * 1000.0f / (cur_time - last_fps_time);
		last_fps_time = cur_time;
		frame_cnt = 0;
	}

	frame_cnt++;
}

void GLGraphics::Render()
{
	msg.clear();
	char s[MAXLINE];

	UpdateTimer();
	

	// calc FPS
	CalcFPS();
	sprintf(s, "fps %f\n", fps); msg += s;

	// show parameters
	sprintf(s, "time %f\n", GetTimer()); msg += s;
	sprintf(s, "yaw %f pitch %f\n", yaw, pitch); msg += s;
	sprintf(s, "center %f %f %f\n", eyecenter.x, eyecenter.y, eyecenter.z); msg += s;
	sprintf(s, "distance %f\n", eyedistance); msg += s;

	

	//if (GetTimer() > 10.0f) ResetTimer();
	glLineWidth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonOffset(1.0f, 1.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set projection matrix
	CalcEyeVector();

	glMatrixMode(GL_PROJECTION);
	glm::mat4 proj;
	//proj = glm::lookAt(glm::vec3(5.0f, 20.0f, 20.0f), glm::vec3(5.0f, 10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//proj = glm::lookAt(glm::vec3(trect_x * 1.2f, trect_y * 2.0f, 20.0f), glm::vec3(trect_x / 2.0f, trect_y / 2.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	proj = glm::lookAt(eye, eyecenter, eyenormal);
	//proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, axis_max) * glm::scale(glm::vec3(1.0f) / eyedistance) * proj;
	proj = glm::perspective(glm::radians(45.0f), (float) window_width / window_height, 0.1f, axis_max) * proj;
	glLoadMatrixf(&proj[0][0]);

	// set model martix
	glm::mat4 model;
	glMatrixMode(GL_MODELVIEW);
	model = glm::scale(glm::vec3(1.0f, 1.0f, -1.0f));
	glLoadMatrixf(&model[0][0]);
	

	// draw axis
	if (draw_axis) {
		DrawAxis();
	}


	// render MIDI object

	glMatrixMode(GL_MODELVIEW);
	model = glm::scale(glm::vec3(1.0f, 1.0f, -1.0f));
	model = glm::translate(glm::vec3(-mobj->minx, -mobj->miny, 0.0f)) * model;
	model = glm::scale(glm::vec3(1.0f / (mobj->maxx - mobj->minx) * trect_x, 1.0f / (mobj->maxy - mobj->miny) * trect_y, 1.0f)) * model;
	model = glm::translate(glm::vec3(0.0f, 0.0f, GetTimer())) * model;
	model = glm::scale(glm::vec3(1.0f, 1.0f, time_scale)) * model;
	glLoadMatrixf(&model[0][0]);
	
	if (draw_box) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glDepthFunc(GL_ALWAYS);
		mobj->RenderBox();
		glDepthFunc(GL_LEQUAL);
	}

	if (note_mode & 2) {
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		mobj->Render();
	}
	if (note_mode & 1) {
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		mobj->Render(1);
	}


	
	// render transparent rect
	glMatrixMode(GL_MODELVIEW);
	model = glm::scale(glm::vec3(1.0f, 1.0f, -1.0f));
	glLoadMatrixf(&model[0][0]);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	DrawTransparentRect();

	// render message
	if (draw_msg) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRasterPos2f(-1.0f, 1.0f);

		glDisable(GL_BLEND);
		glColor3f(1.0f, 1.0f, 1.0f);
		glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned  char *) "\n");
		glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned  char *) msg.c_str());
	}

	glFinish();
    if (use_double_buffer) glutSwapBuffers(); else glFlush();
}

void GLGraphics::DrawAxis()
{
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(axis_max, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, axis_max, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, axis_max);
	glEnd();
}

void GLGraphics::DrawTransparentRect()
{
	glColor4f(1.0f, 1.0f, 1.0f, trect_a);
	glBegin(GL_QUADS);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(trect_x, 0.0f, 0.0f);
	glVertex3f(trect_x, trect_y, 0.0f);
	glVertex3f(0.0f, trect_y, 0.0f);
	glEnd();
}

void GLGraphics::Reshape(int width, int height)
{
    window_width = width;
    window_height = height;
	glViewport(0, 0, width, height);
	mouse_rotate_factor = 2.0f * M_PI / std::min(width, height);
	mouse_eyecenter_factor = 20.0f / std::min(width, height);
    glutPostRedisplay();
}

void GLGraphics::CalcEyeVector()
{
	glm::vec4 v(1.0f, 0.0f, 0.0f, 1.0f), n(0.0f, 1.0f, 0.0f, 1.0f), z(0.0f, 0.0f, 1.0f, 1.0f);

	glm::mat4 m;
	m = glm::rotate(yaw, glm::vec3(0.0f, 1.0f, 0.0f));
	m = glm::rotate(pitch, glm::vec3(1.0f, 0.0f, 0.0f)) * m;
	v = m * v;
	n = m * n;
	z = m * z;


	//printf("v %f %f %f %f\n", v.x, v.y, v.z, v.w);
	//printf("%f\n", glm::vec3(m * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)).x);
	
	//eyecenter = glm::vec3(trect_x / 2.0f, trect_y / 2.0f, 0.0f);
	eye = eyecenter + glm::vec3(v) * eyedistance;
	eyenormal = n;
	eyez = z;
}

void GLGraphics::KeyboardFunc(unsigned char key, int x, int y)
{
	//printf("keyboard func: %c, %d, %d\n", key, x, y);

	if (key == ' ') {
		pause = !pause;
		if (pause) {
			mplay->Stop();
		} else {
			float pos = GetTimer();
			mplay->Seek(pos);
			mplay->Play();
			ResetTimer(pos);
		}
	}

	if (key == '[') {
		time_scale = std::max(time_scale_min, time_scale / time_scale_factor);
	}
	if (key == ']') {
		time_scale = std::min(time_scale_max, time_scale * time_scale_factor);
	}

	if (key == ',') {
		Seek(GetTimer() - seek_delta);
	}
	if (key == '.') {
		Seek(GetTimer() + seek_delta);
	}
	
	if (key == '-') {
		trect_a = std::max(0.0f, trect_a - trect_a_delta);
	}
	if (key == '=') {
		trect_a = std::min(1.0f, trect_a + trect_a_delta);
	}

	if (key == 'r') {
		Seek(0.0f);
	}

	if (key == 'm') {
		note_mode = 1 + note_mode % 3;
	}

	if (key == 'b') {
		draw_box = !draw_box;
	}

	if (key == 'a') {
		draw_axis = !draw_axis;
	}

	if (key == 's') {
		draw_msg = !draw_msg;
	}

	if (key == 'c') {
		mobj->RandomColor();
	}
}

void GLGraphics::MouseFunc(int button, int state, int x, int y)
{
	//printf("mouse func: %d, %d, %d, %d\n", button, state, x, y);

	if (button >= 0 && button < MAX_MOUSE_BUTTON) {
		mouse_button[button] = state;
		mouse_buttonX[button] = x;
		mouse_buttonY[button] = y;
	}

	if (button == 2 && state == 0) {
		mouse_yaw = yaw;
		mouse_pitch = pitch;
	}

	if (button == 0 && state == 0) {
		mouse_eyecenter = eyecenter;
	}

	if ((button == 4 || button == 3) && state == 0) {
		if (button == 4 && state == 0) {
			eyedistance = std::min(eyedistance_max, eyedistance + eyedistance_delta);
		}
		if (button == 3 && state == 0) {
			eyedistance = std::max(eyedistance_min, eyedistance - eyedistance_delta);
		}
		//printf("dist %f\n", eyedistance);
	}
}

void GLGraphics::MotionFunc(int x, int y)
{
	if (mouse_button[2] == 0) {
		yaw = mouse_yaw + (x - mouse_buttonX[2]) * mouse_rotate_factor;
		pitch = mouse_pitch + (y - mouse_buttonY[2]) * mouse_rotate_factor;
		while (yaw > 2.0f * M_PI) yaw -= 2.0f * M_PI;
		while (yaw < 0.0f) yaw += 2.0f * M_PI;
		while (pitch > 2.0f * M_PI) pitch -= 2.0f * M_PI;
		while (pitch < 0.0f) pitch += 2.0f * M_PI;

		//printf("yaw %f pitch %f\n", yaw, pitch);
	}

	if (mouse_button[0] == 0) {
		glm::mat3 f(0.0f);
		f[0][0] = eyez.x > 0.0f ? -1.0f : 1.0f;
		f[1][1] = eyenormal.y > 0.0f ? 1.0f : -1.0f;
		f[2][2] = 1.0f;

		eyecenter = mouse_eyecenter + f * glm::vec3(-1.0f * (x - mouse_buttonX[0]) * mouse_eyecenter_factor, (y - mouse_buttonY[0]) * mouse_eyecenter_factor, 0.0f);
		//printf("center %f %f %f\n", eyecenter.x, eyecenter.y, eyecenter.z);
	}
}

void GLGraphics::Seek(float abs_time)
{
	if (abs_time < 0) abs_time = 0;
	mplay->Stop();
	mplay->Seek(abs_time);
	if (!pause) mplay->Play();
	ResetTimer(abs_time);
}

void GLGraphics::PrintUsage()
{
	printf("Usage:\n");
	printf(" Keyboard:\n");
	printf("  - +    Change alpha of Z=0\n");
	printf("  [ ]    Change time scale\n");
	printf("  < >    Seek\n");
	printf("  Space  Pause/Resume\n");
	printf("  r      Reset time\n");
	printf("  m      Switch note render mode\n");
	printf("  b      Toggle box\n");
	printf("  a      Toggle axis\n");
	printf("  s      Toggle status\n");
	printf("  c      Random color\n");
	printf(" Mouse:\n");
	printf("  Left   Move up/down\n");
	printf("  Right  Rotate\n");
	printf("  Mid    Change Distance\n");
}

void GLGraphics::SetMIDIName(const std::string &midname)
{
	const char *s = midname.c_str();
	const char *p;
	p = strrchr(s, '\\');
	if (p) s = p + 1;
	p = strrchr(s, '/');
	if (p) s = p + 1;

	window_title = std::string("3D MIDI Viewer: ") + s;
}
