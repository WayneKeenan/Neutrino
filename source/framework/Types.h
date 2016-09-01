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
static const int iMAX_SPRITES = 2048;

#define RADIANS_TO_DEGREES(radians) ((radians) * (180.0f / fPI))
#define DEGREES_TO_RADIANS(angle) ((angle) / 180.0f * fPI)
