#version 330 core

in vec3 lightDir,normal;
in vec2 uvVals;
uniform sampler2D tex;

void main()
{
	vec3 ct,cf;
	vec4 texel;
	float intensity,at,af;
		
	intensity = max(dot(lightDir,normalize(normal)),0.0);
		
	cf = intensity * (gl_FrontMaterial.diffuse).rgb + gl_FrontMaterial.ambient.rgb;
	af = gl_FrontMaterial.diffuse.a;
		
	texel = texture2D(tex, uvVals);
	ct = texel.rgb;
	at = texel.a;
		
	gl_FragColor = vec4(ct * cf, at * af);	
	//gl_FragColor = texel;
	//gl_FragColor = vec4(uvVals, 0.0f, 0.0f);
}