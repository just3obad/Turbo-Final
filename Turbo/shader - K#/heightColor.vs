#version 330

uniform mat4 MVP;
//layout(location = 0) in vec4 Vertex;
out float h;

void main() 
{
	h = gl_Vertex.y/256+0.2f;
	gl_Position = MVP * gl_Vertex;
}