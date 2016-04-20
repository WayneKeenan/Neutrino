//
//  Shader.fsh
//  BulletPoint
//
//  Created by Korruptor on 15/08/2010.
//  Copyright Ruffian Games 2010. All rights reserved.
//


//Normal Shader!

//precision lowp float;

uniform sampler2D texture;

varying vec2 fragmentTextureCoordinates;
varying vec4 colorVarying;

void main()
{
  vec4 col;
  
  col.r = texture2D( texture, vec2( fragmentTextureCoordinates.x, fragmentTextureCoordinates.y ) ).x; 
  col.g = texture2D( texture, vec2( fragmentTextureCoordinates.x, fragmentTextureCoordinates.y ) ).y;
  col.b = texture2D( texture, vec2( fragmentTextureCoordinates.x, fragmentTextureCoordinates.y ) ).z;
  col.a = texture2D( texture, vec2( fragmentTextureCoordinates.x, fragmentTextureCoordinates.y ) ).w;
                       
  //  gl_FragColor = vec2(col, 1.0f) * colorVaring;
  gl_FragColor = col * colorVarying;
}