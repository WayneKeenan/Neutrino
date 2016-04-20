uniform sampler2D texture;

varying vec2 fragmentTextureCoordinates;
varying vec4 colorVarying;

void main()
{
	vec4 sum = vec4(0);
	vec2 texcoord = vec2(fragmentTextureCoordinates);
	
	int j;
	int i;
	
	for( i= -4 ;i < 4; i++)
	{
		for (j = -3; j < 3; j++)
		{
			sum += texture2D(texture, texcoord + vec2(j, i)*0.004) * 0.25;
		}
	}
	if (texture2D(texture, texcoord).r < 0.3)
	{
		gl_FragColor = sum*sum*0.012 + texture2D(texture, texcoord)* colorVarying;
	}
	else
	{
		if (texture2D(texture, texcoord).r < 0.5)
		{
			gl_FragColor = sum*sum*0.009 + texture2D(texture, texcoord)* colorVarying;
		}
		else
		{
			gl_FragColor = sum*sum*0.0075 + texture2D(texture, texcoord)* colorVarying;
		}
	}
}