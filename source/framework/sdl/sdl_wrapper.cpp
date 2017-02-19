#include "sdl_wrapper.h"
#include "../Log.h"
#include "../GLUtils.h"
#include "../Input.h"
#include "../Debug.h"
#include "../Memory.h"
#include "../Time.h"
#include "../NeutrinoMath.h"
#include "../imgui/imgui.h"
#include "imgui_impl_sdl_gl3.h"
#include <vector>
#include <math.h>

namespace Neutrino
{
	SDL_Window* pSDL_WindowHandle = NULL;
	SDL_GLContext SDL_GLContext;

	static const char* s_pBasePath;
	static const char* s_pPrefsPath;
	static const char* s_pGameName;

	static int s_iKeyDown[512];
	static bool s_bIsFullscreen = false;


	static const uint16 s_iAxisDeadZone = 8000;		// TODO: This will probably need to be configurable from the front end. 

	static JoypadInput_t* s_aJoypads[_MAX_JOYPADS];
	static MouseInput_t* s_pMouseInput;

	typedef struct GameController_t
	{
		int 				_INSTANCE;
		SDL_GameController* _GAMEPAD;
	} GameController_t;

	typedef std::vector<GameController_t*> ControllerList;


	// The Controller list is a vector of MaxJoypads that we will poll for input each frame. 
	// 
	// The default mapping between controller and player is based on the position in this vector, 
	// so 0 == Player 1, etc. 
	// 
	// Note: If device 0 were to be removed, this would mean the shuffling down of any other 
	// attached devices! This might not be what you want mid-game! 
	// 
	// (The framework biases always having an active controller for player one over 
	// explicitly handling this. You may want to provide an alternate mapping above this vector
	// for game logic...)
	// 
	static ControllerList s_aControllers;


	// TODO: Integrate the community SDL mappings from here:
	//       https://github.com/gabomdq/SDL_GameControllerDB


	// ------------------------------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------------------------


	bool SDLInit(const char* const pOrgName, const char * const pGameName)
	{
		if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			LOG_ERROR("Unable to initialise SDL systems, exiting...");
			return false;
		}

		s_pGameName = pGameName;
		s_pPrefsPath = SDL_GetPrefPath(pOrgName, pGameName);
		s_pBasePath = SDL_GetBasePath();
		s_pMouseInput = NEWX(MouseInput_t);

		for (int i = 0; i < _MAX_JOYPADS; ++i)
		{
			s_aJoypads[i] = NEWX(JoypadInput_t);
			s_aJoypads[i]->_LEFT_STICK = glm::vec3(0.0f, 0.0f, 0.0f);
			s_aJoypads[i]->_RIGHT_STICK = glm::vec3(0.0f, 0.0f, 0.0f);
			s_aJoypads[i]->_ACTION_TRIGGER_1 = 0.0f;
			s_aJoypads[i]->_ACTION_TRIGGER_2 = 0.0f;
			s_aJoypads[i]->_FACE_BUTTONS = 0x00;
			s_aJoypads[i]->_META_BUTTONS = 0x00;
			s_aJoypads[i]->_DPAD = 0x00;
		}
		// Tell the Input functions where to find our key state array and joypad states
		SetControls(&s_iKeyDown[0], s_aJoypads, s_pMouseInput);
		return true;
	}



	bool SDLCreateWindowAndContext(const int iScreenWidth, const int iScreenHeight)
	{
		// Define OpenGL 3.1 core

		/*
			 SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
			 SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
			 SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
			 */

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);


// 		if (SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1"))
// 		{
// 			LOG_INFO("SDL_HINT_VIDEO_HIGHDPI_DISABLED set OK.");
// 		}


		// Create SDL window
		pSDL_WindowHandle = SDL_CreateWindow( 
				s_pGameName, 
				SDL_WINDOWPOS_CENTERED, 
				SDL_WINDOWPOS_CENTERED, 
				iScreenWidth, 
				iScreenHeight, 
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI); // | SDL_WINDOW_FULLSCREEN);


		if( NULL == pSDL_WindowHandle )
		{
			LOG_ERROR( "Game window could not be created. SDL Error: %s\n", SDL_GetError() );
			return false;
		}


		// Create GL context
		SDL_GLContext = SDL_GL_CreateContext( pSDL_WindowHandle );

		if( NULL == SDL_GLContext )
		{
			LOG_ERROR( "OpenGL context could not be created. SDL Error: %s\n", SDL_GetError() );
			return false;
		}
		SDL_GL_MakeCurrent( pSDL_WindowHandle, SDL_GLContext);


		// Initialize GLEW
		glewExperimental = GL_TRUE; 
		GLenum glewError = glewInit();
		if( glewError != GLEW_OK )
		{
			LOG_ERROR( "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) );
			return false;
		}
		GL_ERROR; // see: https://www.opengl.org/wiki/OpenGL_Loading_Library

		// Use Vsync
		if( SDL_GL_SetSwapInterval( 1 ) < 0 )
		{
			LOG_ERROR( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
		}

		SDL_StartTextInput();

		// Setup ImGUI
		if (!ImGui_ImplSdlGL3_Init(pSDL_WindowHandle))
		{
			LOG_ERROR("ImGUI Init failed, exiting");
			return false;
		}
		
		if (!ImGui_ImplSdlGL3_CreateDeviceObjects())
		{
			LOG_ERROR("ImGUI Create Device failed, exiting...");
			return false;
		}

		//SDL_SetWindowFullscreen(pSDL_WindowHandle,SDL_WINDOW_FULLSCREEN);

		return true;
	}





	void SDLPresent()
	{
		ImGui::Render();
		SDL_GL_SwapWindow( pSDL_WindowHandle );
	}





	// ------------------------------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------------------------

	// Setup responses to standard controls, like Alt+Enter, Esc to bring up exit overly, etc. 
	static void ProcessStandardInputs()
	{

		if (s_iKeyDown[SDLK_LALT & ~SDLK_SCANCODE_MASK] && s_iKeyDown[SDLK_RETURN & ~SDLK_SCANCODE_MASK])
		{
			// Debounce the keys so we don't flip back and forth
			s_iKeyDown[SDLK_LALT & ~SDLK_SCANCODE_MASK] = 0;
			s_iKeyDown[SDLK_RETURN & ~SDLK_SCANCODE_MASK] = 0;

			if (!s_bIsFullscreen)
				SDL_SetWindowFullscreen(pSDL_WindowHandle, SDL_WINDOW_FULLSCREEN);
			else
				SDL_SetWindowFullscreen(pSDL_WindowHandle, 0);

			s_bIsFullscreen = !s_bIsFullscreen;
		}

	}


	// Takes a controller ID and checks it for being a valid controller to add to our list
	static void AddGameController(const int iID, const bool bLog = true)
	{
		// If we're already tracking Max Controllers ignore whatever this is...
		if (s_aControllers.size() >= _MAX_JOYPADS)
		{
			LOG_WARNING("A new controller has been attached, but Neutrino is already tracking %d. Ignoring thie device.", _MAX_JOYPADS);
			return;
		}

		// If this isn't even a valid controller, ignore it
		if (!SDL_IsGameController(iID))
			return;

		// Get the pad associated with this ID
		SDL_GameController *pPad = SDL_GameControllerOpen(iID);

		// Have we been able to open the controller for reading?
		if (!pPad)
		{
			LOG_WARNING("Unable to open GameController [InstanceID %d], ignoring...");
			return;
		}

		SDL_Joystick *pJoy = SDL_GameControllerGetJoystick(pPad);
		int iInstance = SDL_JoystickInstanceID(pJoy);

		GameController_t* pElem = NEWX(GameController_t);
		pElem->_GAMEPAD = pPad;
		pElem->_INSTANCE = iInstance;

		// Add this to our controller list. 
		s_aControllers.push_back(pElem);

		if (bLog)
		{
			// Output some debug log info about this pad
			{

				SDL_JoystickGUID guid = SDL_JoystickGetGUID(pJoy);
				char guid_str[1024];
				SDL_JoystickGetGUIDString(guid, guid_str, sizeof(guid_str));
				LOG_INFO("Info for game controller %d:", s_aControllers.size() - 1);
				LOG_INFO("Joystick Name: '%s'", SDL_JoystickName(pJoy));
				LOG_INFO("Controller Name: '%s'", SDL_GameControllerName(pElem->_GAMEPAD));
				LOG_INFO("GUID: %s", guid_str);
				LOG_INFO("SDL Instance ID: %d", SDL_JoystickInstanceID(pJoy));
				LOG_INFO("Number of Axes: %2d", SDL_JoystickNumAxes(pJoy));
				LOG_INFO("Number of Buttons: %2d", SDL_JoystickNumButtons(pJoy));
				LOG_INFO("Mapping: '%s'", SDL_GameControllerMappingForGUID(guid));
			}
		}
	}



	// Handler for the SDL event...
	static void RemoveGameController(const int iID)
	{
		LOG_INFO("Looking to remove controller matching istance ID: %d.", iID);

		ControllerList::iterator it;
		GameController_t* pElem;
		bool bFound = false;

		for (it = s_aControllers.begin(); it != s_aControllers.end(); ++it)
		{
			pElem = *it;
			if (pElem->_INSTANCE == iID)
			{
				LOG_INFO("GameController %d [Instance ID: %d] has been removed from the list of attached controllers.", it - s_aControllers.begin(), iID);
				SDL_GameControllerClose(pElem->_GAMEPAD);
				bFound = true;
				break;
			}
		}

		if (bFound)
		{
			DELETEX pElem;
			s_aControllers.erase(it);
			LOG_INFO("There are now %d game controllers attached.", s_aControllers.size());
		}
		else
		{
			LOG_ERROR("Unable to find a game controller matching ID %d in the list of attached controllers! You may have a player assigned to a disconnected controller!", iID);
		}
	}



	// Basic cleanup for the SDLKill() function... May get called on return to menu state?
	static void UnassignGameControllers()
	{
		int iIndex = 0;
		for (ControllerList::iterator it = s_aControllers.begin(); it != s_aControllers.end(); ++it, ++iIndex)
		{
			GameController_t* pElem = *it;
			SDL_GameControllerClose(pElem->_GAMEPAD);
			DELETEX pElem;

			// Clear what ever we have stored as an input for this pad
			s_aJoypads[iIndex]->_LEFT_STICK = glm::vec3(0.0f, 0.0f, 0.0f);
			s_aJoypads[iIndex]->_RIGHT_STICK = glm::vec3(0.0f, 0.0f, 0.0f);
			s_aJoypads[iIndex]->_ACTION_TRIGGER_1 = 0.0f;
			s_aJoypads[iIndex]->_ACTION_TRIGGER_2 = 0.0f;
			s_aJoypads[iIndex]->_FACE_BUTTONS = 0x00;
			s_aJoypads[iIndex]->_META_BUTTONS = 0x00;
			s_aJoypads[iIndex]->_DPAD = 0x00;
		}
		s_aControllers.clear();
	}



	static void OnGameControllerAxis(const SDL_ControllerAxisEvent& event)
	{
		ControllerList::iterator it;
		GameController_t* pElem;
		bool bFound = false;
		int iIndex = 0;
		for (it = s_aControllers.begin(); it != s_aControllers.end(); ++it, ++iIndex)
		{
			pElem = *it;
			if (pElem->_INSTANCE == event.which)
			{
				bFound = true;
				break;
			}
		}

		if (bFound)
		{
			JoypadInput_t* pJoypad = s_aJoypads[iIndex];
			switch (event.axis)
			{
			case SDL_CONTROLLER_AXIS_LEFTX:	pJoypad->_LEFT_STICK.x = (float)event.value / 32768.0f; break;
			case SDL_CONTROLLER_AXIS_LEFTY:	pJoypad->_LEFT_STICK.y = (float)event.value / 32768.0f; break;
			case SDL_CONTROLLER_AXIS_RIGHTX: pJoypad->_RIGHT_STICK.x = (float)event.value / 32768.0f; break;
			case SDL_CONTROLLER_AXIS_RIGHTY: pJoypad->_RIGHT_STICK.y = (float)event.value / 32768.0f; break;
			case SDL_CONTROLLER_AXIS_TRIGGERLEFT:	pJoypad->_ACTION_TRIGGER_1 = (float)event.value / 32768.0f; break;
			case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: pJoypad->_ACTION_TRIGGER_2 = (float)event.value / 32768.0f; break;
			default:	break;
			}
		}
		else
		{
			LOG_WARNING("SDL_ControllerAxisEvent passed for gamepad mapping that's not in the connected pad list.");
		}
	}


	static void UpdateMouse()
	{
		int x = 0; int y = 0;
		uint32 iButtonMask = SDL_GetMouseState(&x, &y);
		s_pMouseInput->_MOUSE_COORDS.x = (float)x;
		s_pMouseInput->_MOUSE_COORDS.y = (float)y;
		(iButtonMask & SDL_BUTTON_LMASK) != 0 ? s_pMouseInput->_LEFT_BUTTON = true : s_pMouseInput->_LEFT_BUTTON = false;
		(iButtonMask & SDL_BUTTON_RMASK) != 0 ? s_pMouseInput->_RIGHT_BUTTON = true : s_pMouseInput->_RIGHT_BUTTON = false;
	}


	static void OnGameControllerButton(const SDL_ControllerButtonEvent& event)
	{
		ControllerList::iterator it;
		GameController_t* pElem;
		bool bFound = false;
		int iIndex = 0;
		for (it = s_aControllers.begin(); it != s_aControllers.end(); ++it, ++iIndex)
		{
			pElem = *it;
			if (pElem->_INSTANCE == event.which)
			{
				bFound = true;
				break;
			}
		}

		if (bFound)
		{
			JoypadInput_t* pJoypad = s_aJoypads[iIndex];
			switch (event.button)
			{
			case SDL_CONTROLLER_BUTTON_A:	pJoypad->_FACE_BUTTONS ^= 1 << _ACTIONBUTTON_A; break;
			case SDL_CONTROLLER_BUTTON_B:	pJoypad->_FACE_BUTTONS ^= 1 << _ACTIONBUTTON_B; break;
			case SDL_CONTROLLER_BUTTON_X:	pJoypad->_FACE_BUTTONS ^= 1 << _ACTIONBUTTON_X; break;
			case SDL_CONTROLLER_BUTTON_Y:	pJoypad->_FACE_BUTTONS ^= 1 << _ACTIONBUTTON_Y; break;
			case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:	pJoypad->_FACE_BUTTONS ^= 1 << _SHOULDER_LEFT; break;
			case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:	pJoypad->_FACE_BUTTONS ^= 1 << _SHOULDER_RIGHT; break;
			case SDL_CONTROLLER_BUTTON_START: pJoypad->_META_BUTTONS ^= 1 << _META_START; break;
			case SDL_CONTROLLER_BUTTON_BACK: pJoypad->_META_BUTTONS ^= 1 << _META_BACK; break;
			case SDL_CONTROLLER_BUTTON_GUIDE: pJoypad->_META_BUTTONS ^= 1 << _META_GUIDE; break;
			case SDL_CONTROLLER_BUTTON_DPAD_UP:	pJoypad->_DPAD ^= 1 << _DPAD_UP; break;
			case SDL_CONTROLLER_BUTTON_DPAD_DOWN:	pJoypad->_DPAD ^= 1 << _DPAD_DOWN; break;
			case SDL_CONTROLLER_BUTTON_DPAD_LEFT:	pJoypad->_DPAD ^= 1 << _DPAD_LEFT; break;
			case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: pJoypad->_DPAD ^= 1 << _DPAD_RIGHT; break;
			}
		}
		else
		{
			LOG_WARNING("SDL_ControllerButtonEvent passed for gamepad mapping that's not in the connected pad list.");
		}
	}

	


	bool SDLProcessInput(uint8* iEditorFlags)
	{
		ImGui_ImplSdlGL3_NewFrame(pSDL_WindowHandle);

		// NOTE: Don't clear any of the pad inputs as we'll only be getting changes through the event queue!
		
		// Clear the mouse wheel - TODO: This should roll back to zero...
		s_pMouseInput->_MOUSE_WHEEL = 0.0f;

		// clear the editor flags bitfield
		*iEditorFlags = 0x00;

		// Poll for events
		SDL_Event event;
		bool bRet = true;


		while (SDL_PollEvent(&event) != 0)
		{
			ImGui_ImplSdlGL3_ProcessEvent(&event);

			switch (event.type)
			{
			case SDL_QUIT:
				bRet = false;
				break;

			case SDL_KEYDOWN:
			{
				// F1 toggles the default debug overlays (perf tracker, fly cam, etc.)
				if (event.key.keysym.sym == SDLK_F1)
					ToggleDebugOverlay();

				// Check F Keys for toggling the internal editor modes
				{
					if (event.key.keysym.sym == SDLK_F2)
						*iEditorFlags |= _SPLINE_ED;
					if (event.key.keysym.sym == SDLK_F3)
						*iEditorFlags |= _MAP_ED;
					if (event.key.keysym.sym == SDLK_F4)
						*iEditorFlags |= _PARTICLE_ED;
				}
			}	// Note there's no break here!
			case SDL_KEYUP:
			{
				int key = event.key.keysym.sym & ~SDLK_SCANCODE_MASK;
				s_iKeyDown[key] = (event.type == SDL_KEYDOWN);
			}
			break;

			case SDL_CONTROLLERDEVICEADDED:
			{
				LOG_WARNING("Frame %d - A controller device has been added!", GetFrameCount());
				AddGameController((int)event.cdevice.which);
			}
			break;

			case SDL_CONTROLLERDEVICEREMOVED:
			{
				LOG_WARNING("Frame %d - A controller device has been removed!", GetFrameCount());
				RemoveGameController((int)event.cdevice.which);
			}
			break;

			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_CONTROLLERBUTTONUP:
				OnGameControllerButton(event.cbutton);
				break;

			case SDL_CONTROLLERAXISMOTION:
				if(fabs(event.caxis.value) > s_iAxisDeadZone)
					OnGameControllerAxis(event.caxis);
				break;

			case SDL_MOUSEWHEEL:
				s_pMouseInput->_MOUSE_WHEEL = (float)event.wheel.y;
				break;
			}
		}

		// Set mouse pointer coords and button state
		UpdateMouse();

		// Handle framework standard inputs
		ProcessStandardInputs();

		// Let Input process current frame's events for the rest of the framework. 
		ProcessFrameInput();

		return bRet;
	}


	// ------------------------------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------------------------


	// Note: The bytes per pixel of PNG files changes depending on the PNG optimisation setting
	// 			 PNG may revert to indexed colour for simple images unless you expressly turn compression off
	// 			 In texture packer, set the optimisation level to 0 to avoid this. 
	bool GLTextureFromSDLSurface(GLuint* pTextureID, const SDL_Surface* pSurf, const bool bFiltered)
	{
		int iMode = 0;
		if (pSurf->format->BytesPerPixel == 3)
		{
			iMode = GL_RGB;
		}
		else if (pSurf->format->BytesPerPixel == 4)
		{
			iMode = GL_RGBA;
		}
		else
		{
			LOG_ERROR("UploadTextureFromSDLSurface: Only 24 and 32bit SDL surfaces are supported");
			LOG_ERROR("Bytes per pixel: %d ", pSurf->format->BytesPerPixel);
			return false;
		}

		glGenTextures(1, pTextureID);
		ASSERT_GL_ERROR;
		glBindTexture(GL_TEXTURE_2D, *pTextureID);
		ASSERT_GL_ERROR;
		glTexImage2D(GL_TEXTURE_2D, 0, pSurf->format->BytesPerPixel, pSurf->w, pSurf->h, 0, iMode, GL_UNSIGNED_BYTE, pSurf->pixels);
		ASSERT_GL_ERROR;

		if (bFiltered)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		LOG_INFO("Texture bound: %d/%dpx @ %d bytes per pixel to id %d", pSurf->w, pSurf->h, pSurf->format->BytesPerPixel, *pTextureID);
		return true;
	}




	const char* SDLGetPrefPath()
	{
		return s_pPrefsPath;
	}




	const char* SDLGetBasePath()
	{
		return s_pBasePath;
	}



	// ------------------------------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------------------------


	bool SDLKill()
	{
		ImGui_ImplSdlGL3_Shutdown();
		SDL_GL_DeleteContext(SDL_GLContext);
		SDL_DestroyWindow(pSDL_WindowHandle);
		UnassignGameControllers();
		SDL_Quit();

		for (int i = 0; i < _MAX_JOYPADS; ++i)
			DELETEX(s_aJoypads[i]);

		return true;
	}



}
