#version 330 core

uniform mat4 MVP;
uniform vec3 lightDirection, data;

in vec4 Position;
in vec3 Normal;
in vec2 TexCoord;

out vec3 lightDir, normal;
out vec2 uvVals;

void main() {
	
		normal = Normal;		
		lightDir = normalize(vec3(gl_LightSource[0].position));		
		uvVals = TexCoord;	
		
		gl_Position = MVP * Position;
	}
	
	
	
	