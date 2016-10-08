#include "Input.h"
#include "Memory.h"
#include "Log.h"
#include <SDL.h>
#include <string.h>

namespace Neutrino
{
	static InputMappings_t* s_pKeyInput;
	static char* s_pKeyboardMappingsString;

	static void Init()
	{
		s_pKeyInput = NEWX (InputMappings_t);
		s_pKeyInput->_aKeyboardMappings = NEWX int[_NUM_INPUTS];

		for(int i=0;i<_NUM_INPUTS;++i)
		{
			s_pKeyInput->_aKeyboardMappings[i] = 0;		// Maps to SDLK_UNKNOWN
		}

		s_pKeyboardMappingsString = NEWX char[25*16];
	}


	static void GenerateMappingsString()
	{
		snprintf(s_pKeyboardMappingsString, 25*16, 
				"player1_up: %d\nplayer1_down: %d\nplayer1_left: %d\nplayer1_right: %d\nplayer1_action1: %d\nplayer1_action2: %d\nplayer1_action3: %d\nplayer1_action4: %d\nplayer2_up: %d\nplayer2_down: %d\nplayer2_left: %d\nplayer2_right: %d\nplayer2_action1: %d\nplayer2_action2: %d\nplayer2_action3: %d\nplayer2_action4: %d\n",
				s_pKeyInput->_aKeyboardMappings[_PLAYER1_UP],
				s_pKeyInput->_aKeyboardMappings[_PLAYER1_DOWN],
				s_pKeyInput->_aKeyboardMappings[_PLAYER1_LEFT],
				s_pKeyInput->_aKeyboardMappings[_PLAYER1_RIGHT],
				s_pKeyInput->_aKeyboardMappings[_PLAYER1_ACTION1],
				s_pKeyInput->_aKeyboardMappings[_PLAYER1_ACTION2],
				s_pKeyInput->_aKeyboardMappings[_PLAYER1_ACTION3],
				s_pKeyInput->_aKeyboardMappings[_PLAYER1_ACTION4],	

				s_pKeyInput->_aKeyboardMappings[_PLAYER2_UP],
				s_pKeyInput->_aKeyboardMappings[_PLAYER2_DOWN],
				s_pKeyInput->_aKeyboardMappings[_PLAYER2_LEFT],
				s_pKeyInput->_aKeyboardMappings[_PLAYER2_RIGHT],
				s_pKeyInput->_aKeyboardMappings[_PLAYER2_ACTION1],
				s_pKeyInput->_aKeyboardMappings[_PLAYER2_ACTION2],
				s_pKeyInput->_aKeyboardMappings[_PLAYER2_ACTION3],
				s_pKeyInput->_aKeyboardMappings[_PLAYER2_ACTION4]
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

		if (!SetMapping(cfg, "player1_up", &s_pKeyInput->_aKeyboardMappings[_PLAYER1_UP]) ) return false;
		if (!SetMapping(cfg, "player1_down", &s_pKeyInput->_aKeyboardMappings[_PLAYER1_DOWN]) ) return false;
		if (!SetMapping(cfg, "player1_left", &s_pKeyInput->_aKeyboardMappings[_PLAYER1_LEFT]) ) return false;
		if (!SetMapping(cfg, "player1_right", &s_pKeyInput->_aKeyboardMappings[_PLAYER1_RIGHT]) ) return false;
		if (!SetMapping(cfg, "player1_action1", &s_pKeyInput->_aKeyboardMappings[_PLAYER1_ACTION1]) ) return false;
		if (!SetMapping(cfg, "player1_action2", &s_pKeyInput->_aKeyboardMappings[_PLAYER1_ACTION2]) ) return false;
		if (!SetMapping(cfg, "player1_action3", &s_pKeyInput->_aKeyboardMappings[_PLAYER1_ACTION3]) ) return false;
		if (!SetMapping(cfg, "player1_action4", &s_pKeyInput->_aKeyboardMappings[_PLAYER1_ACTION4]) ) return false;

		if (!SetMapping(cfg, "player2_up", &s_pKeyInput->_aKeyboardMappings[_PLAYER2_UP]) ) return false;
		if (!SetMapping(cfg, "player2_down", &s_pKeyInput->_aKeyboardMappings[_PLAYER2_DOWN]) ) return false;
		if (!SetMapping(cfg, "player2_left", &s_pKeyInput->_aKeyboardMappings[_PLAYER2_LEFT]) ) return false;
		if (!SetMapping(cfg, "player2_right", &s_pKeyInput->_aKeyboardMappings[_PLAYER2_RIGHT]) ) return false;
		if (!SetMapping(cfg, "player2_action1", &s_pKeyInput->_aKeyboardMappings[_PLAYER2_ACTION1]) ) return false;
		if (!SetMapping(cfg, "player2_action2", &s_pKeyInput->_aKeyboardMappings[_PLAYER2_ACTION2]) ) return false;
		if (!SetMapping(cfg, "player2_action3", &s_pKeyInput->_aKeyboardMappings[_PLAYER2_ACTION3]) ) return false;
		if (!SetMapping(cfg, "player2_action4", &s_pKeyInput->_aKeyboardMappings[_PLAYER2_ACTION4]) ) return false;

		GenerateMappingsString();
		return true;
	}


	void InputInitWithDefaults()
	{
		Init();
		s_pKeyInput->_aKeyboardMappings[_PLAYER1_UP] = SDLK_w;
		s_pKeyInput->_aKeyboardMappings[_PLAYER1_DOWN] = SDLK_s;
		s_pKeyInput->_aKeyboardMappings[_PLAYER1_LEFT] = SDLK_a;
		s_pKeyInput->_aKeyboardMappings[_PLAYER1_RIGHT] = SDLK_d;
		s_pKeyInput->_aKeyboardMappings[_PLAYER1_ACTION1] = SDLK_SPACE;
		s_pKeyInput->_aKeyboardMappings[_PLAYER1_ACTION2] = SDLK_UNKNOWN;
		s_pKeyInput->_aKeyboardMappings[_PLAYER1_ACTION3] = SDLK_UNKNOWN;
		s_pKeyInput->_aKeyboardMappings[_PLAYER1_ACTION4] = SDLK_UNKNOWN;

		s_pKeyInput->_aKeyboardMappings[_PLAYER2_UP] = SDLK_UNKNOWN;
		s_pKeyInput->_aKeyboardMappings[_PLAYER2_DOWN] = SDLK_UNKNOWN;
		s_pKeyInput->_aKeyboardMappings[_PLAYER2_LEFT] = SDLK_UNKNOWN;
		s_pKeyInput->_aKeyboardMappings[_PLAYER2_RIGHT] = SDLK_UNKNOWN;
		s_pKeyInput->_aKeyboardMappings[_PLAYER2_ACTION1] = SDLK_UNKNOWN;
		s_pKeyInput->_aKeyboardMappings[_PLAYER2_ACTION2] = SDLK_UNKNOWN;
		s_pKeyInput->_aKeyboardMappings[_PLAYER2_ACTION3] = SDLK_UNKNOWN;
		s_pKeyInput->_aKeyboardMappings[_PLAYER2_ACTION4] = SDLK_UNKNOWN;

		GenerateMappingsString();
	}


	bool InputKill()
	{
		DELETEX [] s_pKeyInput->_aKeyboardMappings;
		DELETEX s_pKeyInput;
		DELETEX s_pKeyboardMappingsString;
		return true;
	}

	char* GetInputMappingsString()
	{
		return s_pKeyboardMappingsString;
	}
}