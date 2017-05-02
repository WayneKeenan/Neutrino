attribute vec2 position;
attribute vec4 color;
attribute float size;

uniform mat4 matrix;

varying vec4 colorVarying;

void main ()
{
	colorVarying = color;
	gl_Position = matrix * vec4(position, 0.0,1.0);
	gl_PointSize = size;
}

