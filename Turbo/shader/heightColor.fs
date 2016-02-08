#version 150

in float h;

void main() 
{
	if (h < 0)
		gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	else
		gl_FragColor = vec4(h, h, h, 1.0);
}