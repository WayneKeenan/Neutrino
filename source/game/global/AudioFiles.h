#pragma once

// Handles to loaded samples
enum eSFX
{
	_STARTUP_CHIME,
	_NUM_SFX
};

enum eMusic
{
	_GAMEOVER,
	_TITLE,
	_LEVELA,
	_LEVELB,
	_LEVELC,
	_LOADING,
	_COMPLETION,
	_NUM_MUSIC
};

// Filenames of the samples we're using
static const char* const s_sStartUpChime = "gameboy_startup.wav";

// Filenames of the music to load
static const char* const s_sGAMEOVER = "GAMEOVER.MOD";
static const char* const s_sTITLEMUSIC = "KILLER.XM";
static const char* const s_sLEVELA = "LEVEL_A.MOD";
static const char* const s_sLEVELB = "LEVEL_B.MOD";
static const char* const s_sLEVELC = "LEVEL_C.MOD";
static const char* const s_sLOADING = "LOADING.MOD";
static const char* const s_sEND = "THE_END.MOD";