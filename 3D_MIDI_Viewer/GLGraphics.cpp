#include "pch.h"

GLGraphics::GLGraphics()
{
	
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
        fail("opengl version is too low to run this program. (opengl 1.5 required)");
    }


	glutDisplayFunc([](){ GLGraphics::Instance()->Render(); });
	glutIdleFunc([](){ GLGraphics::Instance()->Render(); });
	glutReshapeFunc([](int w, int h){ GLGraphics::Instance()->Reshape(w, h); });
	glutKeyboardFunc([](unsigned char k, int x, int y){ GLGraphics::Instance()->KeyboardFunc(k, x, y); });
}

void GLGraphics::Run()
{
	mplay->Play();
	ResetTimer();
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

void GLGraphics::Render()
{
	UpdateTimer();

	//if (GetTimer() > 10.0f) ResetTimer();
	glLineWidth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POLYGON_OFFSET_FILL);
	
	glPolygonOffset(1.0f, 1.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set projection matrix
	glMatrixMode(GL_PROJECTION);
	glm::mat4 proj;
	//proj = glm::lookAt(glm::vec3(5.0f, 20.0f, 20.0f), glm::vec3(5.0f, 10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	proj = glm::lookAt(glm::vec3(trect_x, trect_y * 1.2f, 20.0f), glm::vec3(trect_x / 2.0f, trect_y / 2.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	proj = glm::perspective(glm::radians(45.0f), (float) window_width / window_height, 0.1f, axis_max) * proj;
	glLoadMatrixf(&proj[0][0]);

	// set model martix
	glm::mat4 model;
	glMatrixMode(GL_MODELVIEW);
	model = glm::scale(glm::vec3(1.0f, 1.0f, -1.0f));
	glLoadMatrixf(&model[0][0]);
	

	// draw axis
	DrawAxis();


	// render MIDI object

	glMatrixMode(GL_MODELVIEW);
	model = glm::scale(glm::vec3(1.0f, 1.0f, -1.0f));
	model = glm::scale(glm::vec3(1.0f, 1.0f / 128.0f * trect_y, 1.0f)) * model;
	model = glm::translate(glm::vec3(0.0f, 0.0f, GetTimer())) * model;
	model = glm::scale(glm::vec3(1.0f, 1.0f, time_scale)) * model;
	glLoadMatrixf(&model[0][0]);
	
	if (note_mode & 2) {
		 glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		mobj->Render();
	}
	if (note_mode & 1) {
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		mobj->Render(1);
		glEnable(GL_POLYGON_OFFSET_FILL);
	}

	
	// render transparent rect
	glMatrixMode(GL_MODELVIEW);
	model = glm::scale(glm::vec3(1.0f, 1.0f, -1.0f));
	glLoadMatrixf(&model[0][0]);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	DrawTransparentRect();
	

    if (use_double_buffer) glutSwapBuffers(); else glFlush();
}

void GLGraphics::DrawAxis()
{
	glLineWidth(1.0f);
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
    glutPostRedisplay();
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

	if (key == ']') {
		Seek(GetTimer() + seek_delta);
	}
	if (key == '[') {
		Seek(GetTimer() - seek_delta);
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
}

void GLGraphics::Seek(float abs_time)
{
	if (abs_time < 0) abs_time = 0;
	mplay->Stop();
	mplay->Seek(abs_time);
	if (!pause) mplay->Play();
	ResetTimer(abs_time);
}
