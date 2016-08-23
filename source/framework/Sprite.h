#pragma once
#include "GLUtils.h"
#include "Types.h"

namespace Neutrino
{
	// Sprite_t is a struct of pointers into the internal "sprite settings" arrays (see the .cpp file). 
	// 
	// The settings arrays are traversed each tick and their contents are
	// used to populate the VBO that's drawn that frame. 
	// 
	// There's no caching of sprite data in the VBO, or in the internal settings arrays.  
	// Active game objects will need to call GetActiveSprite and populate the struct in order to be drawn that tick
	// 

	typedef struct Sprite_t			
	{ 
		// Texture ID
		// Sprite Index
		float*		_fHalfWidth;	// The pixel dimensions, as mapped to the internal coord system, not the GL pixel output
		float* 		_fHalfHeight;
		float*		_fRotDegrees;	// Sprite rotation
		float*		_fScale;		// Sprite's scale
		glm::vec4*	_vColour;		// Sprite's RGBA colour values
		glm::vec3* 	_vPosition;		// Sprites position, as mapped to the internal coord system, not the GL pixel output
	} Sprite_t;


	// AllocateSpriteArrays
	// 		Called by the framework to statically allocate the sprite settings arrays
	void AllocateSpriteArrays(uint16_t iSpriteCount);

	// DeallocateSpriteArrays
	// 		Called by the framework to release the sprite settings arrays
	void DeallocateSpriteArrays();

	// GetActiveSprite
	//		Get the next available Sprite_t struct in order to draw a new sprite this frame 		
	Sprite_t* GetActiveSprite();

	// ResetSpriteCount()
	// 		Called by the framework at the start of each tick, moves the "active sprite" to the
	//   	bottom of the "sprite settings" arrays, in effect clearing (without zeroing memory)
	//    	all sprites from the next VBO that will be sent to the GPU
	void ResetSpriteCount();


	// GetSpriteCount
	// 		Return the number of active sprites this tick
	uint16 GetSpriteCount();

	// GetBasePointers
	// 		Returns a Sprite_t structure that points to the start of all the sprite arrays
	Sprite_t* GetBasePointers();

	// TestSprite
	// 		Simple functoin to draw a sprite to the centre of the screen, testing this part
	//   	of the framework. 
	void TestSprite();
}
