#pragma once
#include "libconfig.h"

namespace Neutrino
{
	// Indexes into the input mapping array for Keyboard inputs within the framework. 
	// Joypads will be processed separately...
	enum eKeyboard_GameInputs
	{
		_PLAYER1_UP,
		_PLAYER1_DOWN,
		_PLAYER1_LEFT,
		_PLAYER1_RIGHT,
		_PLAYER1_ACTION1,
		_PLAYER1_ACTION2,
		_PLAYER1_ACTION3,
		_PLAYER1_ACTION4,

		_PLAYER2_UP,
		_PLAYER2_DOWN,
		_PLAYER2_LEFT,
		_PLAYER2_RIGHT,
		_PLAYER2_ACTION1,
		_PLAYER2_ACTION2,
		_PLAYER2_ACTION3,
		_PLAYER2_ACTION4,
		_NUM_INPUTS,
	};

	// Array of possible keyboard inputs we want to track that map to the lower level event processing (SDL atm)
	// 
	typedef struct InputMappings_t
	{
		int* _aKeyboardMappings;
		bool _bKeyWasPressed;
	} InputMappings_t;


	// InputInit()
	// 		Allocates memory for the input mappings array and parses the Player's Prefs file for the current input mappings
	bool InputInit(config_t* cfg);

	// InputKill()
	// 		Deallocates the input mappings array. 
	bool InputKill();

	//	InputInitWithDefaults()
	// 		If no player prefs file is found, this is called to setup the input mappings to a default set of WSAD/SPACE
	void InputInitWithDefaults();

	//	GetInputMappingsString()
	// 		Return a human readable string of current input mappings that we can save to the PlayerPrefs file...
	char* GetInputMappingsString();
}