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
	
	unsigned int mode = GLUT_RGBA;
    mode |= use_double_buffer ? GLUT_DOUBLE : GLUT_SINGLE;
    if (multisample_level) {
        glutSetOption(GLUT_MULTISAMPLE, multisample_level);
        mode |= GLUT_MULTISAMPLE;
    }
    glutInitDisplayMode(mode);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow(window_title.c_str());
	glutDisplayFunc([](){ GLGraphics::Instance()->Render(); });
	glutReshapeFunc([](int w, int h){ GLGraphics::Instance()->Reshape(w, h); });
}

void GLGraphics::Run()
{
	glutMainLoop();
}


void GLGraphics::Render()
{
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	

    if (use_double_buffer) glutSwapBuffers(); else glFlush();
}

void GLGraphics::Reshape(int width, int height)
{
    window_width = width;
    window_height = height;
	glViewport(0, 0, width, height);
    glutPostRedisplay();
}