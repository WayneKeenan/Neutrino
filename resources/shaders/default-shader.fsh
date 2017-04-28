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
	vec4 col =  texture2D( texture, fragmentTextureCoordinates.xy );  
  gl_FragColor = col * colorVarying;
}
