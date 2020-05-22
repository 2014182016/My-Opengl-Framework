#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <algorithm>

#include "Dependencies\glew.h"
#include "Dependencies\glm/gtx/euler_angles.hpp"

#include "LoadPng.h"

GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__) 