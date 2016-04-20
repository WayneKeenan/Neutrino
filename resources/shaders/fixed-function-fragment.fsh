precision mediump float;
uniform sampler2D texture;

varying mediump vec2 fragmentTextureCoordinates;
varying mediump vec4 colorVarying;

void main()
{
  gl_FragColor = texture2D(texture, fragmentTextureCoordinates) * colorVarying;
}