#include "Input.h"
#include "Memory.h"
#include "Log.h"
#include <SDL.h>
#include <string.h>
#include "Time.h"
#include "Assert.h"


namespace Neutrino
{
	static InputMappings_t* s_pInputMappings;
	static char* s_pKeyboardMappingsString;
	static int* s_pKeyState;
	static glm::vec3 s_vInputAxis;
	static glm::vec3 s_vInputAxisScaled;

	static void Init()
	{
		s_pInputMappings = NEWX (InputMappings_t);
		s_pInputMappings->_aKeyboardMappings = NEWX int[_NUM_INPUTS];

		for(int i=0;i<_NUM_INPUTS;++i)
		{
			s_pInputMappings->_aKeyboardMappings[i] = 0;		// Maps to SDLK_UNKNOWN
		}

		s_pKeyboardMappingsString = NEWX char[25*16];
	}


	static void GenerateMappingsString()
	{
		snprintf(s_pKeyboardMappingsString, 25*16, 
				"player1_up: %d\nplayer1_down: %d\nplayer1_left: %d\nplayer1_right: %d\nplayer1_action1: %d\nplayer1_action2: %d\nplayer1_action3: %d\nplayer1_action4: %d\nplayer2_up: %d\nplayer2_down: %d\nplayer2_left: %d\nplayer2_right: %d\nplayer2_action1: %d\nplayer2_action2: %d\nplayer2_action3: %d\nplayer2_action4: %d\n",
				s_pInputMappings->_aKeyboardMappings[_PLAYER1_UP],
				s_pInputMappings->_aKeyboardMappings[_PLAYER1_DOWN],
				s_pInputMappings->_aKeyboardMappings[_PLAYER1_LEFT],
				s_pInputMappings->_aKeyboardMappings[_PLAYER1_RIGHT],
				s_pInputMappings->_aKeyboardMappings[_PLAYER1_ACTION1],
				s_pInputMappings->_aKeyboardMappings[_PLAYER1_ACTION2],
				s_pInputMappings->_aKeyboardMappings[_PLAYER1_ACTION3],
				s_pInputMappings->_aKeyboardMappings[_PLAYER1_ACTION4],	

				s_pInputMappings->_aKeyboardMappings[_PLAYER2_UP],
				s_pInputMappings->_aKeyboardMappings[_PLAYER2_DOWN],
				s_pInputMappings->_aKeyboardMappings[_PLAYER2_LEFT],
				s_pInputMappings->_aKeyboardMappings[_PLAYER2_RIGHT],
				s_pInputMappings->_aKeyboardMappings[_PLAYER2_ACTION1],
				s_pInputMappings->_aKeyboardMappings[_PLAYER2_ACTION2],
				s_pInputMappings->_aKeyboardMappings[_PLAYER2_ACTION3],
				s_pInputMappings->_aKeyboardMappings[_PLAYER2_ACTION4]
			);
	}	

	static bool SetMapping(config_t* cfg, const char* sAction, int* mapping)
	{
		if( !config_lookup_int(cfg, sAction, mapping))
		{
			config_destroy(cfg);
			LOG_ERROR("Unable to parse %s from Player Prefs file, exiting...", sAction);
			return false;
		}
		return true;
	}

	bool InputInit(config_t* cfg)
	{
		Init();

		if (!SetMapping(cfg, "player1_up", &s_pInputMappings->_aKeyboardMappings[_PLAYER1_UP]) ) return false;
		if (!SetMapping(cfg, "player1_down", &s_pInputMappings->_aKeyboardMappings[_PLAYER1_DOWN]) ) return false;
		if (!SetMapping(cfg, "player1_left", &s_pInputMappings->_aKeyboardMappings[_PLAYER1_LEFT]) ) return false;
		if (!SetMapping(cfg, "player1_right", &s_pInputMappings->_aKeyboardMappings[_PLAYER1_RIGHT]) ) return false;
		if (!SetMapping(cfg, "player1_action1", &s_pInputMappings->_aKeyboardMappings[_PLAYER1_ACTION1]) ) return false;
		if (!SetMapping(cfg, "player1_action2", &s_pInputMappings->_aKeyboardMappings[_PLAYER1_ACTION2]) ) return false;
		if (!SetMapping(cfg, "player1_action3", &s_pInputMappings->_aKeyboardMappings[_PLAYER1_ACTION3]) ) return false;
		if (!SetMapping(cfg, "player1_action4", &s_pInputMappings->_aKeyboardMappings[_PLAYER1_ACTION4]) ) return false;

		if (!SetMapping(cfg, "player2_up", &s_pInputMappings->_aKeyboardMappings[_PLAYER2_UP]) ) return false;
		if (!SetMapping(cfg, "player2_down", &s_pInputMappings->_aKeyboardMappings[_PLAYER2_DOWN]) ) return false;
		if (!SetMapping(cfg, "player2_left", &s_pInputMappings->_aKeyboardMappings[_PLAYER2_LEFT]) ) return false;
		if (!SetMapping(cfg, "player2_right", &s_pInputMappings->_aKeyboardMappings[_PLAYER2_RIGHT]) ) return false;
		if (!SetMapping(cfg, "player2_action1", &s_pInputMappings->_aKeyboardMappings[_PLAYER2_ACTION1]) ) return false;
		if (!SetMapping(cfg, "player2_action2", &s_pInputMappings->_aKeyboardMappings[_PLAYER2_ACTION2]) ) return false;
		if (!SetMapping(cfg, "player2_action3", &s_pInputMappings->_aKeyboardMappings[_PLAYER2_ACTION3]) ) return false;
		if (!SetMapping(cfg, "player2_action4", &s_pInputMappings->_aKeyboardMappings[_PLAYER2_ACTION4]) ) return false;

		GenerateMappingsString();
		return true;
	}


	void InputInitWithDefaults()
	{
		Init();
		s_pInputMappings->_aKeyboardMappings[_PLAYER1_UP] = SDLK_w & ~SDLK_SCANCODE_MASK;
		s_pInputMappings->_aKeyboardMappings[_PLAYER1_DOWN] = SDLK_s & ~SDLK_SCANCODE_MASK;
		s_pInputMappings->_aKeyboardMappings[_PLAYER1_LEFT] = SDLK_a & ~SDLK_SCANCODE_MASK;
		s_pInputMappings->_aKeyboardMappings[_PLAYER1_RIGHT] = SDLK_d & ~SDLK_SCANCODE_MASK;
		s_pInputMappings->_aKeyboardMappings[_PLAYER1_ACTION1] = SDLK_SPACE & ~SDLK_SCANCODE_MASK;
		s_pInputMappings->_aKeyboardMappings[_PLAYER1_ACTION2] = SDLK_UNKNOWN & ~SDLK_SCANCODE_MASK;
		s_pInputMappings->_aKeyboardMappings[_PLAYER1_ACTION3] = SDLK_UNKNOWN & ~SDLK_SCANCODE_MASK;
		s_pInputMappings->_aKeyboardMappings[_PLAYER1_ACTION4] = SDLK_UNKNOWN & ~SDLK_SCANCODE_MASK;

		s_pInputMappings->_aKeyboardMappings[_PLAYER2_UP] = SDLK_UNKNOWN & ~SDLK_SCANCODE_MASK;
		s_pInputMappings->_aKeyboardMappings[_PLAYER2_DOWN] = SDLK_UNKNOWN & ~SDLK_SCANCODE_MASK;
		s_pInputMappings->_aKeyboardMappings[_PLAYER2_LEFT] = SDLK_UNKNOWN & ~SDLK_SCANCODE_MASK;
		s_pInputMappings->_aKeyboardMappings[_PLAYER2_RIGHT] = SDLK_UNKNOWN & ~SDLK_SCANCODE_MASK;
		s_pInputMappings->_aKeyboardMappings[_PLAYER2_ACTION1] = SDLK_UNKNOWN & ~SDLK_SCANCODE_MASK;
		s_pInputMappings->_aKeyboardMappings[_PLAYER2_ACTION2] = SDLK_UNKNOWN & ~SDLK_SCANCODE_MASK;
		s_pInputMappings->_aKeyboardMappings[_PLAYER2_ACTION3] = SDLK_UNKNOWN & ~SDLK_SCANCODE_MASK;
		s_pInputMappings->_aKeyboardMappings[_PLAYER2_ACTION4] = SDLK_UNKNOWN & ~SDLK_SCANCODE_MASK;

		GenerateMappingsString();
	}


	bool InputKill()
	{
		DELETEX [] s_pInputMappings->_aKeyboardMappings;
		DELETEX s_pInputMappings;
		DELETEX s_pKeyboardMappingsString;
		return true;
	}

	char* GetInputMappingsString()
	{
		return s_pKeyboardMappingsString;
	}

	void SetKeys(int* pKeys)
	{
		s_pKeyState = pKeys;
	}

	void BuildInputAxis(const bool bState)
	{
		float fVert;
		float fHoriz;
		fVert = fHoriz = 0.0f;

		// Had a couple of instance on Lumo where it was useful to know if the player was using keyboard
		// vs Joypad, so we'll keep track of it per frame just for belt and braces. 
		s_pInputMappings->_bKeyWasPressed = bState;


		// Build the input axis for this frame based around keyboard input
		if( s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER1_UP]] != s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER1_DOWN ]])
		{
			if (  s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER1_UP]] )
				fVert = 1.0f;

			if (  s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER1_DOWN]] )
				fVert = -1.0f;
		}


		if( s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER1_LEFT]] != s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER1_RIGHT]])
		{
			if (  s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER1_LEFT]] )
				fHoriz = -1.0f;

			if (  s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER1_RIGHT]] )
				fHoriz = 1.0f;
		}

		s_vInputAxis.x = fHoriz;
		s_vInputAxis.y = fVert;
		s_vInputAxis.z = 0.0f;

		s_vInputAxisScaled = s_vInputAxis * GetGameMSDelta();
	}

	glm::vec3* GetInputAxis()
	{
		return &s_vInputAxis;
	}

	glm::vec3* GetInputAxisGameDeltaScaled()
	{
		return &s_vInputAxisScaled;
	}

	bool GetRawKeyState(const int iRawKey)
	{
		ASSERT(iRawKey<512, "Raw Key called with index out of bounds of keymap");
		return (bool)s_pKeyState[iRawKey];
	}
}