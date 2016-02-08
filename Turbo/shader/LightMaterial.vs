#version 330 core

uniform mat4 MVP;
in vec4 Position;
in vec3 Normal;

out vec3 normal, lightDir, halfVector;
out vec4 diffuse, ambient, outColor;

void main() {
	
		normal = normalize(gl_NormalMatrix * Normal);
		lightDir = normalize(vec3(gl_LightSource[0].position));
		halfVector = normalize(gl_LightSource[0].halfVector.xyz);
		
		//diffuse
		diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
		
		//ambient
		ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
		ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;
		
		
		gl_Position = MVP * Position;
	}
	
	
	
	