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

static const uint32 iU32_BAD = 0xDEADBEEF;
static const uint16 iU16_BAD = 0xDEAD;

static const float fEPSILON = 0.00001f;
static const float fASPECT_RATIO = 1.6f;
static const float fPI = 3.141592653589f;
static const float fHALF_PI = fPI / 2.0f;
static const float fQUARTER_PI = fPI / 4.0f;
static const float fTWO_PI = fPI * 2.0f;
static const float fTILEMAP_ZPOS = 5.0f;								// Ortho far distance is 10 so tilemaps effectively sit in the middle of our z-range
static const int iMAX_TEXTURES = 4;											// The intention is for these textures to be 4k ;)
static const int iMAX_LEVELS = 16;											// Arbitrary, just picked that out of my arse. 
static const uint32 iMAX_SPRITES = 1024*64;							// Cap here is really how much memory we're pushing each tick by the looks of things
static const uint32 iMAX_TILEMAP_SPRITES = 1024 * 256;	// These are static draw, so pushed to the GPU once. No real limit needed
static const uint8 iMAX_TILEMAPS = 32;									// We're statically allocating the GLuint VBO IDs, so can have more (static GLuint s_pTilemapVBOs[iMAX_TILEMAPS])

static const int iDEFAULT_VIEWPORT_WIDTH = 1920;				// TODO: When we go FULLSCREEN_WINDOW, this shouldn't matter. No need to open at a fixed resolution?
static const int iDEFAULT_VIEWPORT_HEIGHT = 1080;
static const int iDEFAULT_INTERNAL_WIDTH = 480;					// TODO: This will be used by the FBO, and game logic should scale to it [320(Div6) - 384(Div5)]
static const int iDEFAULT_INTERNAL_HEIGHT = 270;				// 180 - 216

// Debug builds will check for function keys to flip the framework into specific editor modes. 
// Bit flags are tested in CoreUpdate()
// 
static const uint8 _SPLINE_ED = 0x01;
static const uint8 _MAP_ED = 0x02;
static const uint8 _PARTICLE_ED = 0x04;

#define RADIANS_TO_DEGREES(radians) ((radians) * (180.0f / fPI))
#define DEGREES_TO_RADIANS(angle) ((angle) / 180.0f * fPI)

typedef struct NeutrinoPreferences_t {
	int _iScreenWidth;							// Viewport Dimensions
	int _iScreenHeight;
	int _iInternalWidth;						// Internal coord dimensions
	int _iInternalHeight;
	float _InternalPixelWidth;
	float _InternalPixelHeight;
	const char* _pResourcePath;		// Packfile locations
	const char* _pPrefsPath;				// Player prefs file location
} NeutrinoPreferences_t;

static const char* const s_pPostProcessSettingsFilename = "NeutrinoPost.tdi";