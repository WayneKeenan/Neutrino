//
//  blur-shader.fsh
//  Beat Arena
//
//  Created by kor on 30/05/2011.
//  Copyright 2011 Ruffian Games. All rights reserved.
//




uniform sampler2D texture;

varying vec2 fragmentTextureCoordinates;
varying vec4 colorVarying;

const   float blurSize = 1.0 / 1024.0; 

void main(void)
{
	vec4 sum = vec4(0.0);
	sum += texture2D(texture, vec2(fragmentTextureCoordinates.x - 5.0*blurSize, fragmentTextureCoordinates.y)) * 0.03;	
	sum += texture2D(texture, vec2(fragmentTextureCoordinates.x - 4.0*blurSize, fragmentTextureCoordinates.y)) * 0.04;
	sum += texture2D(texture, vec2(fragmentTextureCoordinates.x - 3.0*blurSize, fragmentTextureCoordinates.y)) * 0.08;
	sum += texture2D(texture, vec2(fragmentTextureCoordinates.x - 2.0*blurSize, fragmentTextureCoordinates.y)) * 0.12;
	sum += texture2D(texture, vec2(fragmentTextureCoordinates.x - blurSize,     fragmentTextureCoordinates.y)) * 0.14;
	sum += texture2D(texture, vec2(fragmentTextureCoordinates.x,                fragmentTextureCoordinates.y)) * 0.16;
	sum += texture2D(texture, vec2(fragmentTextureCoordinates.x + blurSize,     fragmentTextureCoordinates.y)) * 0.14;
	sum += texture2D(texture, vec2(fragmentTextureCoordinates.x + 2.0*blurSize, fragmentTextureCoordinates.y)) * 0.12;
	sum += texture2D(texture, vec2(fragmentTextureCoordinates.x + 3.0*blurSize, fragmentTextureCoordinates.y)) * 0.08;
	sum += texture2D(texture, vec2(fragmentTextureCoordinates.x + 4.0*blurSize, fragmentTextureCoordinates.y)) * 0.04;
	sum += texture2D(texture, vec2(fragmentTextureCoordinates.x + 5.0*blurSize, fragmentTextureCoordinates.y)) * 0.03;
	
	gl_FragColor = sum * colorVarying;
}