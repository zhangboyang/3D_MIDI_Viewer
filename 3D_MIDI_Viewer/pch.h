#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#ifndef _DEBUG
#define NDEBUG
#endif

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>
#include <memory>
#include <set>
#include <map>

#include <windows.h>
#include <dshow.h>

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
#include "MIDIPlayer.h"
#include "MIDIData.h"
#include "MIDIObject.h"
#include "GLGraphics.h"

#define be32toh _byteswap_ulong
#define be16toh _byteswap_ushort

#define MAXLINE 4096

#pragma comment(lib, "strmiids.lib")

#pragma warning(disable: 4996)

#define eps (1e-6)


template <class T> void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

