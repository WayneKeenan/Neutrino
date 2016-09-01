#include "sdl_wrapper.h"
#include "../Log.h"
#include "../GLUtils.h"


namespace Neutrino
{
	SDL_Window* pSDL_WindowHandle = NULL;
	SDL_GLContext SDL_GLContext;

	static const char* s_pBasePath;
	static const char* s_pPrefsPath;
	static const char* s_pGameName;

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
		return true;
	}


	bool SDLCreateWindowAndContext(int iScreenWidth, int iScreenHeight)
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
		//glewExperimental = GL_TRUE; 
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
		//SDL_SetWindowFullscreen(pSDL_WindowHandle,SDL_WINDOW_FULLSCREEN);
		return true;
	}


	void SDLPresent()
	{
		SDL_GL_SwapWindow( pSDL_WindowHandle );
	}


	bool SDLKill()
	{
		SDL_DestroyWindow( pSDL_WindowHandle );
		SDL_Quit();
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

    bool GLTextureFromSDLSurface(GLuint* pTextureID, SDL_Surface* pSurf, bool bFiltered)
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
}
