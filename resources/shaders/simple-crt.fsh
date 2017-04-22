uniform sampler2D texture;

varying vec2 fragmentTextureCoordinates;
varying vec4 colorVarying;
varying vec2 pixelDims;

const float ScanlineDark = 0.2;
const float PixelBias = 0.3;
const float Brightness = 2.5;
const float Contrast = 1.5;
const float Width = 480.0;
const float Height = 270.0;
const int ReadOffset = 2;

const vec4 vRBias = vec4(1.0,PixelBias,PixelBias, 1.0);
const vec4 vGBias = vec4(PixelBias, 1.0,PixelBias, 1.0);
const vec4 vBBias = vec4(PixelBias,PixelBias, 1.0, 1.0);
const vec4 vBright = vec4(Brightness, Brightness, Brightness, 1.0);
const vec2 vOffset = vec2(1.0/Width, 1.0/Height);

void main()
{
  vec4 col;
  
  col.r = texture2D( texture, vec2( fragmentTextureCoordinates.x, fragmentTextureCoordinates.y ) ).x; 
  col.g = texture2D( texture, vec2( fragmentTextureCoordinates.x, fragmentTextureCoordinates.y ) ).y;
  col.b = texture2D( texture, vec2( fragmentTextureCoordinates.x, fragmentTextureCoordinates.y ) ).z;
  col.a = texture2D( texture, vec2( fragmentTextureCoordinates.x, fragmentTextureCoordinates.y ) ).w;

  vec4 addCol = col;

	float yPos = mod(gl_FragCoord.y, 4.0);
	float xPos = mod(gl_FragCoord.x, 4.0);

	
	/*	
	vec4 sum = vec4(0);
	int j;
	int i;
	// Do a really shit bloom summation
	for( i= -ReadOffset;i < ReadOffset; i++)
	{
		for (j = -ReadOffset; j < ReadOffset; j++)
		{
			sum += texture2D(texture, fragmentTextureCoordinates + vec2(j, i)*0.008) * 0.25;
		}
	}
	*/

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
			col = col * ScanlineDark;
		}
	}
 
	// Add everything up...

	//col += sum*sum*0.025;
	
	col.rgb = ((col.rgb - 0.5) * max(Contrast, 0.0)) + 0.5;
	col = col * vBright;
	col += addCol * 0.4f;

  gl_FragColor = col  * colorVarying;

}
