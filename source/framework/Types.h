#pragma once

#include <inttypes.h>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef uint64_t uint64;

typedef signed char int8;
typedef signed short int16;
typedef signed int int32;
typedef int64_t int64;

static const float fEPSILON = 0.00001f;
static const float fASPECT_RATIO = 1.6f;
static const float fPI = 3.141592653589f;
static const float fHALF_PI = fPI / 2.0f;
static const float fQUARTER_PI = fPI / 4.0f;
static const float fTWO_PI = fPI * 2.0f;
static const int iMAX_TEXTURES = 4;
static const uint32 iMAX_SPRITES = 1024*128;
static const uint8 iMAX_TILEMAPS = 32;

static const int iDEFAULT_VIEWPORT_WIDTH = 1920;
static const int iDEFAULT_VIEWPORT_HEIGHT = 1080;
static const int iDEFAULT_INTERNAL_WIDTH = 1920;		// 320(Div6) - 384(Div5)
static const int iDEFAULT_INTERNAL_HEIGHT = 1080;		// 180 - 216

// Debug builds will check for function keys to flip
// the framework into specific editor modes. 
//
// (f2-f4 to toggle)
static const uint8 _SPLINE_ED = 0x01;
static const uint8 _MAP_ED = 0x02;
static const uint8 _PARTICLE_ED = 0x04;

#define RADIANS_TO_DEGREES(radians) ((radians) * (180.0f / fPI))
#define DEGREES_TO_RADIANS(angle) ((angle) / 180.0f * fPI)
