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
	// Active game objects will need to call NewSprite with the correct TextureID/SprIndex and populate the returned struct in 
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
		float* 		_afX_S;
		float* 		_afY_T;
		float* 		_afX_SnS;
		float* 		_afY_TnT;
		float* 		_afHalfWidth;
		float* 		_afHalfHeight;
		float* 		_afSpriteRotDegrees;
		float* 		_afSpriteScale;
		glm::vec4* 	_avSprColours;
		glm::vec3* 	_avSprPositions;
	} SpriteArraySet_t;

	// Sprite_t is a struct of pointers into the SpriteArraysSet_t arrays, defined above.
	// This is just a utility collection that can be grabbed / passed around for 
	// simpler indexing into the SpriteArraySet. Dereferncing an instance of this gives you
	// one sprite's data. 
	typedef struct Sprite_t			
	{ 
		float* 		_fX_S;
		float* 		_fY_T;
		float* 		_fX_SnS;
		float* 		_fY_TnT;
		float*		_fHalfWidth;		// The pixel dimensions, as mapped to the internal coord system, not the GL pixel output
		float* 		_fHalfHeight;
		float*		_fRotDegrees;		// Sprite rotation
		float*		_fScale;			// Sprite's scale
		glm::vec4*	_vColour;			// Sprite's RGBA colour values
		glm::vec3* 	_vPosition;			// Sprites position, as mapped to the internal coord system, not the GL pixel output
	} Sprite_t;

	// SpriteRenderInfo_t contains the information Sprite functions pass to GLUtils so it can 
	// populate and render VBOs for a given texture's sprites during this frame.  
	typedef struct SpriteRenderInfo_t
	{
		uint32 _iActiveSpriteCount;
		GLuint _iTextureID;
		Sprite_t* _SpriteBasePointers;
		SpriteArraySet_t 	_SprArrayBase;
	} TextureSpriteArrayInfo_t;


	// AllocateSpriteArrays
	// 		Called by a successfully loaded texture to allocate the SpriteArraySet_t, and
	//		SpriteRenderInfo_t structures. 
	void AllocateSpriteArrays(const GLuint iTextureID);

	// DeallocateSpriteArrays
	// 		Called by the framework to release the sprite settings arrays
	void DeallocateSpriteArrays();

	// NewSprite()
	// 		Returns a populated Sprite_t for the sprite at the given index on the given texture 
	// 		page. Will return NULL if the sprite or tpage parameters are incorrect.
	//
	// 		Note: Sprites returned by this function WILL be rendered at the end of the tick. If 
	//		you don't want, don't ask ;)
	Sprite_t* NewSprite(const GLuint iTextureID, const uint16 iSprIndex);

	// ResetSpriteCount()
	// 		Called by the framework at the start of each tick, moves the "active sprite" to the
	//   	bottom of the "sprite settings" arrays, in effect clearing (without zeroing memory)
	//		all sprites. (VBOS are only built and sent to the GPU if there's an active sprite)
	void ResetSpriteCount();

	// DrawSprites()
	// 		Passes all active sprites to the OGL renderer to populate the VBOs that will be output 
	//		this frame. 
	void DrawSprites();


	// TestSprite()
	// 		Simple functoin to draw a sprite to the centre of the screen and verify Sprite_t
	//   	data is working correctly in the VBO
	void TestSprite();


	uint32 GetSpriteCount();


#if defined DEBUG
	uint32 GetDebugSpriteCount();

	// In debug modes, where editor modes are compiled in, untextured sprites are used by several of the editors to show
	// bounds, grids and other information. 
	typedef struct UntexturedSpriteArraySet_t
	{
		float* 		_afHalfWidth;
		float* 		_afHalfHeight;
		float* 		_afSpriteRotDegrees;
		float* 		_afSpriteScale;
		glm::vec4* 	_avSprColours;
		glm::vec3* 	_avSprPositions;
	} UntexturedSpriteArraySet_t;

	typedef struct UntexturedSprite_t
	{
		float*		_fHalfWidth;
		float* 		_fHalfHeight;
		float*		_fRotDegrees;
		float*		_fScale;
		glm::vec4*	_vColour;
		glm::vec3* 	_vPosition;
	} UntexturedSprite_t;

	typedef struct UntexturedSpriteRenderInfo_t
	{
		uint32 _iActiveSpriteCount;
		UntexturedSprite_t* _SpriteBasePointers;
		UntexturedSpriteArraySet_t 	_SprArrayBase;
	} UntexturedTextureSpriteArrayInfo_t;


	// AllocateUntexturedSpriteArrays
	// 		Called by the framework in DEBUG builds to create storage for untextured sprites. 
	void AllocateUntexturedSpriteArrays();

	// DeallocateUntexturedSpriteArrays
	//		Deallocate storage assigned for the debug sprites. 
	void DeallocateUntexturedSpriteArrays();

	// NewDebugSprite
	// 		Grab a new untextured debug sprite. This sprite WILL be rendered on the next tick. 
	UntexturedSprite_t* NewDebugSprite();

	// DrawDebugSprites()
	// 		Passes all the untextured sprites to OGL for rendering. 
	void DrawDebugSprites();
#endif 

}
