//
//  Shader.vsh
//  BulletPoint
//
//  Created by Korruptor on 15/08/2010.
//  Copyright Ruffian Games 2010. All rights reserved.
//

attribute vec4    position;
attribute vec4    color;
attribute vec2    textureCoordinates;

uniform   mat4    matrix;
uniform   float   translate;

varying  vec2    fragmentTextureCoordinates; 
varying  vec4    colorVarying;

void main()
{
    colorVarying = color;
    gl_Position = matrix * position;
    fragmentTextureCoordinates = textureCoordinates;
}