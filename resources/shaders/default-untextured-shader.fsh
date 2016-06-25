//
//  Shader.fsh
//  BulletPoint
//
//  Created by Korruptor on 15/08/2010.
//  Copyright Ruffian Games 2010. All rights reserved.
//

uniform sampler2D texture;
varying vec2 fragmentTextureCoordinates;
varying vec4 colorVarying;

void main()
{
  gl_FragColor = colorVarying;
}
