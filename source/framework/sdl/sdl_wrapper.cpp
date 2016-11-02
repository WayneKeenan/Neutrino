#include "sdl_wrapper.h"
#include "../Log.h"
#include "../GLUtils.h"
#include "../imgui/imgui.h"
#include "imgui_impl_sdl_gl3.h"
#include "../Input.h"
#include "../Debug.h"
#include "../Memory.h"


namespace Neutrino
{
	SDL_Window* pSDL_WindowHandle = NULL;
	SDL_GLContext SDL_GLContext;

	static const char* s_pBasePath;
	static const char* s_pPrefsPath;
	static const char* s_pGameName;
	
	static int s_iKeyDown[512];

	static JoypadInput_t* s_pJoypad_1_Input;
	static JoypadInput_t* s_pJoypad_2_Input;
	static JoypadInput_t* s_pJoypad_3_Input;
	static JoypadInput_t* s_pJoypad_4_Input;

	static int s_iNumConnectedControllers = 0;
	static int s_iNumRealControllers = 0;
	static const int s_iMaxJoypads = 4;

	typedef struct GameController_t
	{
		int 				_INSTANCE_ID;
		SDL_GameController* _GAMEPAD;
		bool 				_ATTACHED = false;
	} GameController_t;

	// TODO: This needs to be converted to a linked list rather than an array
	//       Add and remove events need to remove the element from the list
	//       This'll give some auto remapping of the controllers when devices
	//       are removed...
	static GameController_t* s_pControllers;

	// TODO: Integrate the community SDL mappings from here:
	//       https://github.com/gabomdq/SDL_GameControllerDB


	// ------------------------------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------------------------
	 
	
	// Detect the number of game controllers attached and setup our mappings...
	// Note, this doesn't handle devices being added at runtime...
	static void AssignGameControllers()
	{
		s_iNumConnectedControllers = SDL_NumJoysticks();

		if ( s_iNumConnectedControllers > 1)
			LOG_INFO("Found %d possible game controllers", s_iNumConnectedControllers);
		else
			LOG_INFO("Found %d possible game controller", s_iNumConnectedControllers);

		// Loop over all potential controllers and check them. There may be more
		// attached than we actually want...
		for(int i=0; i < s_iNumConnectedControllers; ++i)
		{
		    if (!SDL_IsGameController(i))
		    	continue;

		    s_pControllers[s_iNumRealControllers]._GAMEPAD = SDL_GameControllerOpen(i);
		    SDL_Joystick *pJoy = SDL_GameControllerGetJoystick( s_pControllers[s_iNumRealControllers]._GAMEPAD );
		   	s_pControllers[s_iNumRealControllers]._INSTANCE_ID = SDL_JoystickInstanceID(pJoy);
		   	s_pControllers[s_iNumRealControllers]._ATTACHED = true;

		    // Output some debug log info about this pad
		    {
	            
	            SDL_JoystickGUID guid = SDL_JoystickGetGUID(pJoy);
  				char guid_str[1024];
  				SDL_JoystickGetGUIDString(guid, guid_str, sizeof(guid_str));
  				LOG_INFO("Info for game controller %d:", i);
				LOG_INFO("Joystick Name: '%s'", SDL_JoystickName(pJoy));
				LOG_INFO("Controller Name: '%s'", SDL_GameControllerName(s_pControllers[s_iNumRealControllers]._GAMEPAD));
				LOG_INFO("GUID: %s", guid_str);
				LOG_INFO("SDL Instance ID: %d", SDL_JoystickInstanceID(pJoy));
				LOG_INFO("Number of Axes: %2d", SDL_JoystickNumAxes(pJoy));
				LOG_INFO("Number of Buttons: %2d", SDL_JoystickNumButtons(pJoy));
				LOG_INFO("Mapping: '%s'", SDL_GameControllerMappingForGUID(guid));
		    }

		    ++s_iNumRealControllers;

		    if (s_iNumRealControllers >= s_iMaxJoypads)
		  		break;
		}
	}

	// Basic cleanup for the SDLKill() function... May get called on return to menu state?
	static void UnassignGameControllers()
	{
		for(int i = 0; i < s_iMaxJoypads; ++i)
		{
			if (s_pControllers[i]._ATTACHED)
			{
				SDL_GameControllerClose(s_pControllers[i]._GAMEPAD);
			}
		}
	}

	// Handler for the SDL event...
	static void RemoveGameController(int iID)
	{
		LOG_INFO("HERE");
		// Find the device that's been removed and 'clear' it from the controllers array...
		for( int i = 0; i < s_iMaxJoypads; ++i)
		{
			if (s_pControllers[i]._ATTACHED && iID == s_pControllers[i]._INSTANCE_ID)
			{
				LOG_WARNING("Game controller %d has been disconnected!", i);
				LOG_WARNING("SDL Instance for this controller was: %d", iID);

				SDL_GameControllerClose(s_pControllers[i]._GAMEPAD);
				s_pControllers[i]._GAMEPAD = NULL;
				s_pControllers[i]._ATTACHED = false;

				break;
			}
		}
	}


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

		s_pJoypad_1_Input = NEWX(JoypadInput_t);
		s_pJoypad_2_Input = NEWX(JoypadInput_t);
		s_pJoypad_3_Input = NEWX(JoypadInput_t);
		s_pJoypad_4_Input = NEWX(JoypadInput_t);
		s_pControllers = NEWX GameController_t[s_iMaxJoypads];

		// Tell the Input functions where to find our key state array and joypad states
		SetControls(&s_iKeyDown[0], s_pJoypad_1_Input, s_pJoypad_2_Input, s_pJoypad_3_Input, s_pJoypad_4_Input);
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

		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );
		ASSERT_GL_ERROR;

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		// Create SDL window
		pSDL_WindowHandle = SDL_CreateWindow( 
									s_pGameName, 
									SDL_WINDOWPOS_CENTERED, 
									SDL_WINDOWPOS_CENTERED, 
									iScreenWidth, 
									iScreenHeight, 
									SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );


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
			return false;
		}

		SDL_StartTextInput();

		// Setup ImGUI
    	ImGui_ImplSdlGL3_Init(pSDL_WindowHandle);
    	ImGui_ImplSdlGL3_CreateDeviceObjects();

		//SDL_SetWindowFullscreen(pSDL_WindowHandle,SDL_WINDOW_FULLSCREEN);
		
		// If we've got this far, check the attached controllers and assign them.
		AssignGameControllers();
		return true;
	}




	void SDLPresent()
	{
		ImGui::Render();
		SDL_GL_SwapWindow( pSDL_WindowHandle );
	}




	bool SDLKill()
	{
		ImGui_ImplSdlGL3_Shutdown();
		SDL_GL_DeleteContext( SDL_GLContext );
		SDL_DestroyWindow( pSDL_WindowHandle );
		UnassignGameControllers();
		SDL_Quit();

		DELETEX(s_pJoypad_1_Input);
		DELETEX(s_pJoypad_2_Input);
		DELETEX(s_pJoypad_3_Input);
		DELETEX(s_pJoypad_4_Input);
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
            return false;
        }

        glGenTextures( 1, pTextureID );
		ASSERT_GL_ERROR;        
        glBindTexture( GL_TEXTURE_2D, *pTextureID );
		ASSERT_GL_ERROR;                
        glTexImage2D( GL_TEXTURE_2D, 0, pSurf->format->BytesPerPixel, pSurf->w, pSurf->h, 0, iMode, GL_UNSIGNED_BYTE, pSurf->pixels );
   		ASSERT_GL_ERROR;        

        if (bFiltered)
        {
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        }
        else
        {
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );        
        }

        LOG_INFO("Texture bound: %d/%dpx @ %d bytes per pixel", pSurf->w, pSurf->h, pSurf->format->BytesPerPixel );
        return true;
    }





    bool SDLProcessInput()
    {
    	ImGui_ImplSdlGL3_NewFrame(pSDL_WindowHandle);

    	SDL_Event event;
    	bool bRet = true;
    	bool bKeyPressed = false;

		while( SDL_PollEvent( &event ) != 0 )
		{
			ImGui_ImplSdlGL3_ProcessEvent(&event);
		
			switch(event.type)
			{
				case SDL_QUIT:
					bRet = false;
				break;

				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_F1)
						ToggleDebugOverlay();
				case SDL_KEYUP:
		        {
		            int key = event.key.keysym.sym & ~SDLK_SCANCODE_MASK;
		            s_iKeyDown[key] = (event.type == SDL_KEYDOWN);
		            bKeyPressed = true;
		        }
		        break;

		        case SDL_CONTROLLERDEVICEREMOVED:
		        		LOG_INFO("HERE");
            		RemoveGameController( (int)event.cdevice.which );
            	break;

		        // TODO: Add in joypad Polling
			}   
		}

		// Let the Input functions build the input axis for this frame
		BuildInputAxis(bKeyPressed);
		return bRet;
    }
}
