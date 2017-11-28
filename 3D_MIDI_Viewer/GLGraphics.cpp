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
	ResetTimer();
	glutMainLoop();
}

float GLGraphics::GetTimer()
{
	return raw_timer / 1000.0f;
}

void GLGraphics::ResetTimer(float value)
{
	raw_timer = value * 1000.0f;
}

void GLGraphics::UpdateTimer()
{
	DWORD now_timer = timeGetTime();
	if (!pause) raw_timer += now_timer - last_timer;
	last_timer = now_timer;
}

void GLGraphics::LoadMIDIObject(std::shared_ptr<MIDIObject> mobj)
{
	this->mobj = mobj;
}

void GLGraphics::Render()
{
	UpdateTimer();

	if (GetTimer() > 10.0f) ResetTimer();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set projection matrix
	glMatrixMode(GL_PROJECTION);
	glm::mat4 proj;
	//proj = glm::lookAt(glm::vec3(5.0f, 20.0f, 20.0f), glm::vec3(5.0f, 10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	proj = glm::lookAt(glm::vec3(20.0f, 20.0f, 10.0f), glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	proj = glm::perspective(glm::radians(45.0f), (float) window_width / window_height, 0.1f, 100.0f) * proj;
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
	model = glm::translate(glm::vec3(0.0f, 0.0f, 3.0f * GetTimer())) * model;
	glLoadMatrixf(&model[0][0]);
	
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	//glPolygonMode(GL_BACK, GL_LINE);

	mobj->Render();

	
	/*glColor3f(1,1,0);
	glBegin(GL_QUADS);
	glVertex3f(0,0,0);
	glVertex3f(1,0,0);
	glVertex3f(1,1,0);
	glVertex3f(0,1,0);
	glEnd();*/

	

    if (use_double_buffer) glutSwapBuffers(); else glFlush();
}

void GLGraphics::DrawAxis()
{
	glLineWidth(1.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(AXIS_MAX, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, AXIS_MAX, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, AXIS_MAX);
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
	printf("keyboard func: %c, %d, %d\n", key, x, y);

	if (key == ' ') pause = !pause;
}