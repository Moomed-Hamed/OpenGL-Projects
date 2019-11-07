#pragma once				/*--- Isometric Tower Defense Game ---*/

#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glew32s.lib"  )
#pragma comment( lib, "glfw3.lib"    )
#pragma comment( lib, "winmm.lib"    )
#pragma comment( lib, "Leap.lib"     )

#define GLEW_STATIC
#define GLM_FORCE_RADIAN
#define STB_IMAGE_IMPLEMENTATION

#include <Windows.h> //For Timing & Memory

#include "GLEW\glew.h"
#include <GLFW\glfw3.h>

#include <GLM\glm\glm.hpp> //For Math
#include <GLM\glm\gtc\matrix_transform.hpp>
#include <GLM\glm\gtc\type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/glm/gtc/quaternion.hpp> //for quaternions
#include <GLM/glm/gtx/quaternion.hpp> //for quaternions

#include "stb_image.h" //For Image Loading

#define LEAP_EXPORT
#include "Leap.h" //For Leap Motion

#define DEBUG_MODE 1 //for debug options
#define SLOW_MODE  1 //for things that slow down the code

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::quat;
using glm::ivec2;

typedef signed   char  int8;
typedef signed   short int16;
typedef signed   int   int32;
typedef signed   long  int64;

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;
typedef unsigned long  uint64;

typedef signed   int sint;
typedef unsigned int uint;
typedef unsigned int bool32;

typedef uint8 byte;

#define NULL 0

#define PI	  3.14159265359f
#define TWOPI 6.28318530718f

#define ToRadians(value) ( ((value) * PI) / 180.0f )
#define ToDegrees(value) ( ((value) * 180.0f) / PI )

#define vecmagnitude(vec) sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z))

#define global static

#if SLOW_MODE
#define Assert(expression) if( !(expression) ) *(int*)0 = 1;
#define out(val) std::cout << ' ' << val << '\n'
#define vecout(vec) std::cout << "x: " << vec.x << " y: " << vec.y << " z: " << vec.z << '\n'
#define stop std::cin.get();
#else
#define Assert(expression)
#define vecout(vec)
#define out(val)
#endif

#if DEBUG_MODE
#define DebugShader(shader) GLint logSize = 0; glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize); char *errorLog = new char[logSize]; glGetShaderInfoLog(shader, logSize, NULL, errorLog); std::cout << errorLog;
#else
#define DebugShader(vert,frag)
#endif