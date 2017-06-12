uniform sampler2D texture;
uniform float ScanlineDark;
uniform float VScanlineDark;
uniform float PixelBias;
uniform float Brightness;
uniform float Contrast;		// Should never be negative. 
uniform float AdditiveStrength;
uniform float TextureWidth;
uniform float TextureHeight;
uniform float ViewportWidth;
uniform float ViewportHeight;

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
 
  	vec2 screenCoords = fragmentTextureCoordinates * vec2(ViewportWidth, ViewportHeight);

	int xPos = int(mod(screenCoords.x, ViewportWidth / TextureWidth*0.5));
	int yPos = int(mod(screenCoords.y, ViewportHeight / TextureHeight*0.5));	

	// Scanline
	if(yPos == 0)
	{
		col = col * ScanlineDark;
	}


	// Pixel bias
	if(xPos == 0)
	{
		col = col * vRBias;
	}
 	else if(xPos == 1)
	{
		col = col * vGBias;
	}
	else if(xPos == 2)
	{
		col = col * vBBias;
	}
 	else if(xPos == 3)
	{
		col = col * VScanlineDark;
	}	

 
	// Add everything up...
	col = col * vBright;
	col.rgb = (((col.rgb - 0.5) * Contrast) + 0.5);
	col += addCol * AdditiveStrength;
	gl_FragColor = col;
}
