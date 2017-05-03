
#include "Colour.h"

namespace Neutrino
{
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