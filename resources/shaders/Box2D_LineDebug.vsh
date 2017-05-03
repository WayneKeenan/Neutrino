attribute vec4 position;
attribute vec4 color;

uniform mat4 matrix;

varying vec4 colorVarying;

void main ()
{
	colorVarying = color;
	gl_Position = matrix * position;
}

