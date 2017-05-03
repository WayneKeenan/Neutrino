attribute vec4 position;
attribute vec4 color;
attribute float size;

uniform mat4 matrix;

varying vec4 colorVarying;

void main ()
{
	colorVarying = color;
	gl_Position = matrix * position;
	gl_PointSize = size;
}

 