uniform sampler2D texture;

varying vec2 fragmentTextureCoordinates;
varying vec4 colorVarying;

uniform float BloomAlpha;
uniform float BloomContrast;	
uniform float BloomBrightness;

void main()
{
	vec4 col =  texture2D( texture, fragmentTextureCoordinates.xy );  
	col.rgb = col.rgb * BloomBrightness;
  col.rgb = ((col.rgb - 0.5) * BloomContrast) + 0.5;
  gl_FragColor = vec4(col.r * BloomAlpha, col.g *BloomAlpha, col.b * BloomAlpha, BloomAlpha);
}
