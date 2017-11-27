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
#include <memory>

#include <windows.h>

#define GLEW_STATIC
#include <GL/glew.h>
#define FREEGLUT_STATIC
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#ifdef NDEBUG
#pragma comment(lib, "glew32s.lib")
#else
#pragma comment(lib, "glew32sd.lib")
#endif

#include "misc.h"
#include "MIDIData.h"
#include "MIDIObject.h"
#include "GLGraphics.h"
