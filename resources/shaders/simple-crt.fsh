uniform sampler2D texture;
uniform float ScanlineDark;
uniform float VScanlineDark;
uniform float PixelBias;
uniform float Brightness;
uniform float Contrast;		// Should never be negative. 
uniform float AdditiveStrength;

varying vec2 fragmentTextureCoordinates;
varying vec4 colorVarying;
varying vec2 pixelDims;

void main()
{
	vec4 vRBias = vec4(1.0,PixelBias,PixelBias, 1.0);
 	vec4 vGBias = vec4(PixelBias, 1.0,PixelBias, 1.0);
 	vec4 vBBias = vec4(PixelBias,PixelBias, 1.0, 1.0);
 	vec4 vBright = vec4(Brightness, Brightness, Brightness, 1.0);
	vec4 col =  texture2D( texture, fragmentTextureCoordinates.xy );
	vec4 addCol = col;
 
	float yPos = mod(gl_FragCoord.y, 4.0);	
	float xPos = mod(gl_FragCoord.x, 4.0);

	// Scanline
	if(int(yPos) == 0 || int(yPos)==3)
	{
		col = col * ScanlineDark;
	}
	else
	{
		// Pixel bias
		if(int(xPos) == 0)
		{
			col = col * vRBias;
		}
	 	else if(int(xPos) == 1)
		{
			col = col * vGBias;
		}
		else if(int(xPos) == 2)
		{
			col = col * vBBias;
		}
	 	else if(int(xPos) == 3)
		{
			col = col * VScanlineDark;
		}
	}
 
	// Add everything up...
	col = col * vBright;
	col.rgb = (((col.rgb - 0.5) * Contrast) + 0.5);
	col += addCol * AdditiveStrength;
	gl_FragColor = col;
}
