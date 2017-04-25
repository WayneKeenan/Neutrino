uniform sampler2D texture;

varying vec2 fragmentTextureCoordinates;
varying vec4 colorVarying;

uniform float Threshold;
uniform float ThresholdBrightScaler;

const vec4 empty = vec4(0.0, 0.0, 0.0, 0.0);

void main()
{
	vec4 col =  texture2D( texture, fragmentTextureCoordinates.xy ); 
  float fV =  dot(col.rgba, vec4(0.299,0.587,0.114,0.0));
  if(fV > Threshold)
    gl_FragColor = col;
  else
   gl_FragColor = col * ThresholdBrightScaler;
}
