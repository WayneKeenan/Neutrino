#pragma once
#include "libconfig.h"
#include "Types.h"
#include <glm/glm.hpp> 

// Bitfield indexes for joypad face buttons
#define _ACTIONBUTTON1 = 1;
#define _ACTIONBUTTON2 = 2;
#define _ACTIONBUTTON3 = 4;
#define _ACTIONBUTTON4 = 8;
#define _SHOULDERLEFT = 16;
#define _SHOULDERRIGHT = 32;
// Bitfield indexes for joypad meta buttons (Home, Share, etc.)
#define _META1 = 1;
#define _META2 = 2;
#define _META3 = 4;
#define _META4 = 8;


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


	// Container struct for possible joypad inputs on a regular next gen joypad. 
	// 
	typedef struct JoypadInput_t
	{
		glm::vec3 _LEFT_STICK;		// Sticks
		glm::vec3 _RIGHT_STICK;
		float _ACTION_TRIGGER_1;	// Triggers
		float _ACTION_TRIGGER_2;
		uint8 _FACE_BUTTONS;		// Bitfield for facebuttons
		uint8 _META_BUTTONS;		// Bitfield for home, share, options, back etc.
	} JoypadInput_t;


	// Container for mouse input. Framework assumes there's one mouse, and SDL wrapper will populate this during the 
	// normal input processing event loop
	//
	typedef struct MouseInput_t
	{
		glm::vec2 _MOUSE_COORDS;
		bool _LEFT_BUTTON;
		bool _RIGHT_BUTTON;
		float _MOUSE_WHEEL;
	} MouseInput_t;

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

	// SetKeys()
	// 		SDL creates the array of key states that we want to use in these files. This function will set up the static pointer
	//   	in this compilation unit to point to the correct location
	void SetControls(int* pKeys, JoypadInput_t* pPad1, JoypadInput_t* pPad2, JoypadInput_t* pPad3, JoypadInput_t* pPad4);

	// BuildInputAxis()
	// 		Should be called once per tick. Will create a glm::vec3 for the current input direction. Keyboard only atm
	void BuildInputAxis(const bool bKeyPressed);

	// 	GetInputAxis()
	//  	Return the raw input axis, created above
	glm::vec3* GetInputAxis(int iPlayer);

	// GetMouseCoords()
	//		Return the current X/Y coordinates of the mouse
	//glm::vec2* GetMouseCoords();

	// GetInputAxisGameDeltaScaled()
	// 		Return the input axis, scaled to the current frame's GameDeltaMS (See: Time.h)
	glm::vec3* GetInputAxisGameDeltaScaled(int iPlayer);

	// GetRawKeyState()
	//		Shouldn't really use this, but if you want to peek into the keyboard input state directly, this will give you access.
	//  	iRawKey is: (SDLK_<key> & ~SDLK_SCANCODE_MASK) 
	bool GetRawKeyState(const int iRawKey);
}