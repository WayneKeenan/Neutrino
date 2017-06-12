#pragma once

#include <inttypes.h>

#define _SDL_MIXER_AUDIO 1																// Predicting support for FMOD/OpenAL will end up being around the corner...
#define _SILENT_RUNNING 1																	// Set music volume to zero on startup 
#define _BOX2D_DEBUG_RENDER 1															// Enable/Disable support for Box2D debug draw of the physics world

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
static const float fPI = 3.141592653589f;
static const float fHALF_PI = fPI / 2.0f;
static const float fQUARTER_PI = fPI / 4.0f;
static const float fTWO_PI = fPI * 2.0f;
static const float fTILEMAP_ZPOS = 5.0f;									// Ortho far distance is 10 so tilemaps effectively sit in the middle of our z-range

static const int _iMAX_TEXTURES = 4;											// The intention is for these textures to be 4k ;)
static const int _iMAX_LEVELS = 16;												// Arbitrary, just picked that out of my arse. 
static const int _iMAX_RENDERMODES = 128;									// How many display modes do we keep track of?
static const uint32 _iMAX_SPRITES = 1024*64;							// Cap here is really how much memory we're pushing each tick by the looks of things
static const uint32 _iMAX_BOX2D_SPRITES = 1024 * 16;			// Cap on how many items we can add to a BOX2D Debug Draw VBO. Again, pulled out of arse
static const uint32 _iMAX_TILEMAP_SPRITES = 1024 * 256;		// These are static draw, so pushed to the GPU once. No real limit needed
static const uint8 _iMAX_TILEMAPS = 32;										// We're statically allocating the GLuint VBO IDs, so can have more (static GLuint s_pTilemapVBOs[iMAX_TILEMAPS])
static const uint8 _iMAX_SAMPLES = 255;
static const uint8 _iMAX_MUSIC = 32;

static const int _iDEFAULT_VIEWPORT_WIDTH = 1920;					// TODO: When we go FULLSCREEN_WINDOW, this shouldn't matter. No need to open at a fixed resolution?
static const int _iDEFAULT_VIEWPORT_HEIGHT = 1080;
static const int _iDEFAULT_INTERNAL_WIDTH = 480;					// TODO: This will be used by the FBO, and game logic should scale to it
static const int _iDEFAULT_INTERNAL_HEIGHT = 270;					

// Debug builds will check for function keys to flip the framework into specific editor modes. 
// Bit flags are tested in CoreUpdate()
// 
static const uint8 _SPLINE_ED = 0x01;
static const uint8 _MAP_ED = 0x02;
static const uint8 _PARTICLE_ED = 0x04;

#define RADIANS_TO_DEGREES(radians) ((radians) * (180.0f / fPI))
#define DEGREES_TO_RADIANS(angle) ((angle) / 180.0f * fPI)

typedef struct Rendermode_t
{
	uint16 _iWidth;
	uint16 _iHeight;
	float _fRefreshRate;
} Remdermode_t;

// This struct defines the framework wide preferences that can be saved by the user
typedef struct NeutrinoPreferences_t {
	int _iScreenWidth = _iDEFAULT_VIEWPORT_WIDTH;						
	int _iScreenHeight = _iDEFAULT_VIEWPORT_HEIGHT;
	int _iInternalWidth = _iDEFAULT_INTERNAL_WIDTH;					// GNTODO: GameConfig.ini should over write these...
	int _iInternalHeight = _iDEFAULT_INTERNAL_HEIGHT;
	float _InternalPixelWidth;
	float _InternalPixelHeight;
	float _fMasterVolume = 1.0f;
	float _fSampleVolume = 1.0f;
	float _fMusicVolume = 1.0f;
	const char* _pResourcePath;			
	const char* _pPrefsPath;				
	Rendermode_t _aRenderModes[_iMAX_RENDERMODES];
} NeutrinoPreferences_t;

static const char* const s_pPostProcessSettingsFilename = "NeutrinoPost.tdi";

