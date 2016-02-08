#version 330 core

in vec3 normal, lightDir, halfVector;
in vec4 diffuse, ambient;

void main()
{
	vec3 n;
	float NdotL;
		
	//ambient
	vec4 color = ambient;

	n = normalize(normal);
		
	NdotL = max(dot(n,lightDir),0.0);

	if (NdotL > 0.0) 
	{
		//difuse
		color += diffuse * NdotL;
		//specular
		color += gl_FrontMaterial.specular *  gl_LightSource[0].specular * pow(max(dot(n,normalize(halfVector)),0.0), gl_FrontMaterial.shininess);
	}
	
	gl_FragColor = color;
}