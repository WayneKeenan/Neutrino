#pragma once
#include "libconfig.h"
#include "Types.h"
#include <glm/glm.hpp> 

// Bitfield shifters for joypad face buttons
#define _ACTIONBUTTON_A  0x00
#define _ACTIONBUTTON_B  0x01
#define _ACTIONBUTTON_X  0x02
#define _ACTIONBUTTON_Y  0x03
#define _SHOULDER_LEFT  0x04
#define _SHOULDER_RIGHT  0x05
// Bitfield shifters for joypad meta buttons (Home, Share, etc.)
#define _META_BACK  0x00
#define _META_START  0x01
#define _META_GUIDE  0x02
#define _META4  0x03
// Bitfield shifters for the DPAD
#define _DPAD_UP 0x00
#define _DPAD_DOWN 0x01
#define _DPAD_LEFT 0x02
#define _DPAD_RIGHT 0x03

#define _MAX_JOYPADS 4

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
		uint8 _DPAD;
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
	void SetControls(int* pKeys, JoypadInput_t* pPads[]);

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