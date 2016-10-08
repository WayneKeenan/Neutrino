uniform sampler2D Texture;
varying vec2 Frag_UV;
varying vec4 Frag_Color;

void main()
{
  vec4 col;
   col.r = texture2D( Texture, vec2( Frag_UV.s, Frag_UV.t ) ).x;
   col.g = texture2D( Texture, vec2( Frag_UV.s, Frag_UV.t ) ).y;
   col.b = texture2D( Texture, vec2( Frag_UV.s, Frag_UV.t ) ).z;
   col.a = texture2D( Texture, vec2( Frag_UV.s, Frag_UV.t ) ).w;
   gl_FragColor = Frag_Color *col;
}