#version 330 core

out vec3 lightDir, normal;
out vec2 uvVals;

uniform mat4 MVP;
//layout(location = 0) in vec4 Vertex;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 texCoord;


void main() {
	
		normal = Normal;		
		lightDir = normalize(vec3(gl_LightSource[0].position));
		
		uvVals = TexCoord;	
		
		gl_Position = MVP * gl_Vertex;
	}
	
	
	
	