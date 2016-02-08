#version 330 core

uniform mat4 MVP;
//layout(location = 0) in vec4 Vertex;
layout(location = 1) in vec3 Normal;

out vec3 normal, lightDir, halfVector;
out vec4 diffuse, ambient;

void main() {
	
		normal = normalize(gl_NormalMatrix * Normal);
		lightDir = normalize(vec3(gl_LightSource[0].position));
		halfVector = normalize(gl_LightSource[0].halfVector.xyz);
		
		//diffuse
		diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
		
		//ambient
		ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
		ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;
		
		
		/*
		NdotL = max(dot(normal, lightDir), 0.0);
		
		//specular
		if (NdotL > 0.0) {	
			// normalize the half-vector, and then compute the 
			// cosine (dot product) with the normal
			NdotHV = max(dot(normal, halfVector),0.0);
			specular = gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(NdotHV,gl_FrontMaterial.shininess);
		}
		
		gl_FrontColor =  NdotL * diffuse + ambient + specular;	
		*/
		
		
		gl_Position = MVP * gl_Vertex;
	}
	
	
	
	