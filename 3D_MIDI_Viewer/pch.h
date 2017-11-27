#pragma once

#ifndef _DEBUG
#define NDEBUG
#endif

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>

#define FREEGLUT_STATIC
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>

#ifdef NDEBUG
#pragma comment(lib, "glew32s.lib")
#else
#pragma comment(lib, "glew32sd.lib")
#endif


#include "GLGraphics.h"

