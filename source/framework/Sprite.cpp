#include "Sprite.h"
#include "Log.h"
#include "Memory.h"

namespace Neutrino 
{
	static uint32* a_u32SprColours = NULL;
	static glm::vec3* a_vSprPositions = NULL;
	static uint16 iActiveSpriteCount = 0;

	void AllocateSpriteArrays(uint16_t iSpriteCount)
	{
		iActiveSpriteCount = 0;

		a_u32SprColours = NEWX uint32[iSpriteCount];
		a_vSprPositions = NEWX glm::vec3[iSpriteCount];
		
		LOG_INFO("Allocated %d bytes [%dK] for sprite colours", sizeof(uint32) * iSpriteCount, (sizeof(uint32) * iSpriteCount) / 1024 );		
		LOG_INFO("Allocated %d bytes [%dK] for sprite positions", sizeof(glm::vec3) * iSpriteCount, (sizeof(glm::vec3) * iSpriteCount) / 1024);
	}

	void DeallocateSpriteArrays()
	{
		DELETEX a_u32SprColours;
		DELETEX a_vSprPositions;
	}

}