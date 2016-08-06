
#include "Colour.h"

namespace Neutrino
{
	uint32 GetPackedColour(const float fR, const float fG, const float fB, const float fA)
	{
		return (uint32)((uint8)(fA * 255.0f) << 24) | ((uint8)(fB * 255.0f) << 16) | ((uint8)(fG * 255.0f) << 8) | (uint8)(fR * 255.0f);
	}

	uint32 GetPackedColourV4(const glm::vec4 vColour)
	{
		return (uint32)((uint8)(vColour.w * 255.0f) << 24) | ((uint8)(vColour.z * 255.0f) << 16) | ((uint8)(vColour.y * 255.0f) << 8) | (uint8)(vColour.x * 255.0f);
	}

	uint32 GetPackedColourHSV(const float fH, const float fS, const float fV, const float fA)
	{
		int i;
		float f, p, q, t, fR, fG, fB, fh;

	  	// Are we grey?
		if( fS == 0 ) 
		{
			fR = fG = fB = fV;
			return 0;
		}

		fh = fH / 60.0f;           // sector 0 to 5
		i = (int)floor( fH );
		f = fh - (float)i;    
		p = fV * ( 1 - fS );
		q = fV * ( 1 - fS * f );
		t = fV * ( 1 - fS * ( 1 - f ) );

		switch( i ) 
		{
			case 0:
				fR = fV;
				fG = t;
				fB = p;
				break;

			case 1:
				fR = q;
				fG = fV;
				fB = p;
				break;

			case 2:
				fR = p;
				fG = fV;
				fB = t;
				break;

			case 3:
				fR = p;
				fG = q;
				fB = fV;
				break;

			case 4:
				fR = t;
				fG = p;
				fB = fV;
				break;

			default:		// case 5:
				fR = fV;
				fG = p;
				fB = q;
				break;
		}

		return GetPackedColour(fR, fG, fB, fA);
	}
}