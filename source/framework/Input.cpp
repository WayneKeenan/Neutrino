#include "Input.h"
#include "Memory.h"
#include "Log.h"
#include <SDL.h>
#include <string.h>
#include "Time.h"
#include "Assert.h"
#include <string.h>

namespace Neutrino
{
	static InputMappings_t* s_pInputMappings;
	static char* s_pKeyboardMappingsString;
	static int* s_pKeyState;
	
	static JoypadInput_t** s_aJoypads;
	static MouseInput_t* s_pMouseInput;

	static glm::vec3 s_vInputAxis_Player1;
	static glm::vec3 s_vInputAxisScaled_Player1;

	static glm::vec3 s_vInputAxis_Player2;
	static glm::vec3 s_vInputAxisScaled_Player2;

	static glm::vec3 s_vInputAxis_Player3;
	static glm::vec3 s_vInputAxisScaled_Player3;

	static glm::vec3 s_vInputAxis_Player4;
	static glm::vec3 s_vInputAxisScaled_Player4;

	static void Init()
	{
		s_pInputMappings = NEWX (InputMappings_t);
		s_pInputMappings->_aKeyboardMappings = NEWX int[_NUM_INPUTS];

		for(int i=0;i<_NUM_INPUTS;++i)
		{
			s_pInputMappings->_aKeyboardMappings[i] = 0;		// Maps to SDLK_UNKNOWN
		}

		// TODO: This will have to handle joypad mappings as well. Too small! 
		s_pKeyboardMappingsString = NEWX char[25*16];
		memset(s_pKeyboardMappingsString, '\0', sizeof(char) * (25*16));

	}


	static void GenerateMappingsString()
	{
		// TODO: This is only tracking 2 players on the keyboard atm...
		//       Needs to go up to 4...
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

		s_pInputMappings->_aKeyboardMappings[_PLAYER2_UP] = SDLK_UP & ~SDLK_SCANCODE_MASK;
		s_pInputMappings->_aKeyboardMappings[_PLAYER2_DOWN] = SDLK_DOWN & ~SDLK_SCANCODE_MASK;
		s_pInputMappings->_aKeyboardMappings[_PLAYER2_LEFT] = SDLK_LEFT & ~SDLK_SCANCODE_MASK;
		s_pInputMappings->_aKeyboardMappings[_PLAYER2_RIGHT] = SDLK_RIGHT & ~SDLK_SCANCODE_MASK;
		s_pInputMappings->_aKeyboardMappings[_PLAYER2_ACTION1] = SDLK_RCTRL & ~SDLK_SCANCODE_MASK;
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

	void SetControls(int* pKeys, JoypadInput_t* pPads[], MouseInput_t* pMouse)
	{
		s_pKeyState = pKeys;
		s_aJoypads = pPads;
		s_pMouseInput = pMouse;
	}


	void ProcessFrameInput()
	{
		float fVert;
		float fHoriz;
		fVert = fHoriz = 0.0f;


		// Build the keyboard Input Axis
		{
			// Build Player 1 input axis from keyboard
			{
				if( s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER1_UP]] != s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER1_DOWN]])
				{
					if ( s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER1_UP]] ) fVert = 1.0f;
					if ( s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER1_DOWN]] ) fVert = -1.0f;
				}

				if( s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER1_LEFT]] != s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER1_RIGHT]])
				{
					if ( s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER1_LEFT]] ) fHoriz = -1.0f;
					if ( s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER1_RIGHT]] ) fHoriz = 1.0f;
				}

				s_vInputAxis_Player1.x = fHoriz;
				s_vInputAxis_Player1.y = fVert;
				s_vInputAxis_Player1.z = 0.0f;

				s_vInputAxisScaled_Player1 = s_vInputAxis_Player1 * GetGameMSDelta();
			}

			fVert = fHoriz = 0.0f;

			// Build Player 2 input axis from keyboard
			{
				if( s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER2_UP]] != s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER2_DOWN]])
				{
					if ( s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER2_UP]] ) fVert = 1.0f;
					if ( s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER2_DOWN]] ) fVert = -1.0f;
				}

				if( s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER2_LEFT]] != s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER2_RIGHT]])
				{
					if ( s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER2_LEFT]] ) fHoriz = -1.0f;
					if ( s_pKeyState[s_pInputMappings->_aKeyboardMappings[_PLAYER2_RIGHT]] ) fHoriz = 1.0f;
				}

				s_vInputAxis_Player2.x = fHoriz;
				s_vInputAxis_Player2.y = fVert;
				s_vInputAxis_Player2.z = 0.0f;

				s_vInputAxisScaled_Player2 = s_vInputAxis_Player2 * GetGameMSDelta();
			}
		}

		// Build the DPAD Axis for each pad
		{
			fVert = fHoriz = 0.0f;
			for (uint8 i = 0; i < _MAX_JOYPADS; ++i)
			{
				if( GetDPadU(i) != GetDPadD(i))
				{
					if (GetDPadU(i)) fVert = 1.0f;
					if (GetDPadD(i)) fVert = -1.0f;
				}

				if( GetDPadL(i) != GetDPadR(i))
				{
					if (GetDPadL(i)) fHoriz = -1.0f;
					if (GetDPadR(i)) fHoriz = 1.0f;
				}

				s_aJoypads[i]->_DPAD_AXIS.x = fHoriz;
				s_aJoypads[i]->_DPAD_AXIS.y = fVert;
				s_aJoypads[i]->_DPAD_AXIS.z = 0.0f;
				s_aJoypads[i]->_DPAD_AXIS_SCALED = s_aJoypads[i]->_DPAD_AXIS * GetGameMSDelta();
			}
		}

		// Scale Joypad axis
		for (int i = 0; i < _MAX_JOYPADS; ++i)
		{
			s_aJoypads[i]->_LEFT_STICK_SCALED = s_aJoypads[i]->_LEFT_STICK * GetGameMSDelta();
			s_aJoypads[i]->_RIGHT_STICK_SCALED = s_aJoypads[i]->_RIGHT_STICK * GetGameMSDelta();
		}
	}


	glm::vec3* GetInputAxis(const int iPlayer, bool bKeyOverride)
	{
		switch(iPlayer)
		{
			default:
			case 0:
				if (bKeyOverride) return &s_vInputAxis_Player1; else return &s_aJoypads[0]->_LEFT_STICK;
				break;
			case 1:
				if (bKeyOverride) return &s_vInputAxis_Player2; else return &s_aJoypads[1]->_LEFT_STICK;
				break;
			case 2:
				if (bKeyOverride) return &s_vInputAxis_Player3; else return &s_aJoypads[2]->_LEFT_STICK;
				break;
			case 3:
				if (bKeyOverride) return &s_vInputAxis_Player4; else return &s_aJoypads[3]->_LEFT_STICK;
				break;
		}
	}

	glm::vec3* GetInputAxisGameDeltaScaled(int iPlayer, bool bKeyOverride)
	{
		ASSERT(iPlayer < _MAX_JOYPADS, "GetInputAxisGameDeltaScaled, index exceeds MAX_JOYPADS");
		switch(iPlayer)
		{
			default:
			case 0:
				if (bKeyOverride) return &s_vInputAxisScaled_Player1; else return &s_aJoypads[0]->_LEFT_STICK_SCALED;
				break;
			case 1:
				if (bKeyOverride) return &s_vInputAxisScaled_Player2; else return &s_aJoypads[1]->_LEFT_STICK_SCALED;
				break;
			case 2:
				if (bKeyOverride) return &s_vInputAxisScaled_Player3; else return &s_aJoypads[2]->_LEFT_STICK_SCALED;
				break;
			case 3:
				if (bKeyOverride) return &s_vInputAxisScaled_Player4; else return &s_aJoypads[3]->_LEFT_STICK_SCALED;
				break;
		}
	}

	bool GetRawKeyState(const int iRawKey)
	{
		ASSERT(iRawKey<512, "Raw Key called with index out of bounds of keymap");
		return s_pKeyState[iRawKey] != 0;
	}

	bool GetKeyState(const eKeyboard_EditorInputs iKey)
	{
		switch (iKey)
		{
			case eKeyboard_EditorInputs::_ESC: return s_pKeyState[SDLK_ESCAPE & ~SDLK_SCANCODE_MASK] == 1; break;
			case eKeyboard_EditorInputs::_UNDO: return s_pKeyState[SDLK_u & ~SDLK_SCANCODE_MASK] == 1; break;
			case eKeyboard_EditorInputs::_REDO: return s_pKeyState[SDLK_r & ~SDLK_SCANCODE_MASK] == 1; break;
			default: break;
		}
		return false;
	}

	bool GetButton(const uint8 iInput, const uint8 iPlayerIndex)
	{
		ASSERT(iPlayerIndex < _MAX_JOYPADS, "Get button called for a player index larger than _MAX_JOYPADS");
		return (s_aJoypads[iPlayerIndex]->_FACE_BUTTONS >> iInput) & 1;
	}

	bool GetDPadDir(const uint8 iInput, const uint8 iPlayerIndex)
	{
		ASSERT(iPlayerIndex < _MAX_JOYPADS, "Get dpad dir  called for a player index larger than _MAX_JOYPADS");
		return (s_aJoypads[iPlayerIndex]->_DPAD >> iInput) & 1;
	}

	glm::vec2* GetMouseCoords()
	{
		ASSERT(s_pMouseInput);
		return &(s_pMouseInput->_MOUSE_COORDS);
	}

	bool GetMouseLB()
	{
		return s_pMouseInput->_LEFT_BUTTON;
	}

	bool GetMouseRB()
	{
		return s_pMouseInput->_RIGHT_BUTTON;
	}
}
