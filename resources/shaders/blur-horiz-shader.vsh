//
//  blur-shader.h
//  Beat Arena
//
//  Created by kor on 30/05/2011.
//  Copyright 2011 Ruffian Games. All rights reserved.
//

attribute vec4    position;
attribute vec4    color;
attribute vec2    textureCoordinates;

uniform   mat4    matrix;
uniform   float   translate;

varying   vec2    fragmentTextureCoordinates; 
varying   vec4    colorVarying;

void main()
{
  colorVarying = color;
  gl_Position = matrix * position;
  fragmentTextureCoordinates = textureCoordinates;
}