#pragma once
#include "GLUtils.h"
#include "Types.h"


namespace Neutrino
{

	// The SpriteArraySet is a struct of arrays for the data required to build quads in the rendered VBOs. These arrays are 
	// traversed each tick.	
	// 
	// There's no caching of sprite data in any VBO, or within these arrays. It's assumed that most sprites will move and
	// animate so it'll be quicker to populate each VBO from scratch, each frame. 
	// 
	// Active game objects will need to call GetActiveSprite with the correct TextureID and populate the returned struct in 
	// order to be drawn...
	// 
	// Every loaded Texture will have it's own SpriteArraySet, calling the AllocateSpriteArrays() function when it's been 
	// successfully bound on the OGL side. This is to ensure that all sprites for a given texture will populate the same VBO, 
	// and consequently output in one draw call. 
	// 
	// The framework currently makes no distinction for alpha blended sprites, so will potentially be slower on mobile until 
	// these aren't split into their own VBO and rendered last.
	// 
	typedef struct SpriteArraySet_t
	{
		float* 		_afHalfWidth;
    	float* 		_afHalfHeight;
    	float* 		_afSpriteRotDegrees;
    	float* 		_afSpriteScale;
    	glm::vec4* 	_avSprColours;
    	glm::vec3* 	_avSprPositions;
	} SpriteArraySet_t;



	// Sprite_t is a struct of pointers into the SpriteArraysSet_t arrays, defined above.
	// This is just a utility BasePtr collection that can be grabbed / passed around for simpler indexing into the SpriteArraySet.  
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



	// SpriteRenderInfo_t contains the information GLUtils needs to know about each texture (and it's SpriteArraySet) in order 
	// to decide whether to populate and render VBOs for this frame. 
	//  
	typedef struct SpriteRenderInfo_t
	{
		uint16 				_iActiveSpriteCount;
		GLuint 				_iTextureID;
		Sprite_t* 			_SpriteBasePointers;
		SpriteArraySet_t 	_SprArrayBase;
	} TextureApriteArrayInfo_t;


	// AllocateSpriteArrays
	// 		Called by a successfully loaded texture to allocate the SpriteArraySet_t, and SpriteRenderInfo_t structures. 
	void AllocateSpriteArrays(GLuint iTextureID);

	// DeallocateSpriteArrays
	// 		Called by the framework to release the sprite settings arrays
	//   	TO DO: Actually delete all of them...
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
	// 		Simple functoin to draw a sprite to the centre of the screen and verify Sprite_t
	//   	data is working correctly in the VBO
	void TestSprite();
}
