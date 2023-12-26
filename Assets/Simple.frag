#version 460

in vec3 vNormal;
in vec2 vTexCoords;

uniform vec3 AmbientColour;
uniform vec3 LightColour;
uniform vec3 LightDirection;

uniform sampler2D diffuseTex;

out vec4 FragColour;

void main()
{
	vec3 N = normalize(vNormal);
	vec3 L = normalize(LightDirection);

	float lambertTerm = max(0, min( 1, dot(N, -L) ) );

	vec3 textureColour = texture(diffuseTex, vTexCoords).rgb;

	vec3 diffuse = LightColour * lambertTerm * textureColour;
	vec3 ambient = AmbientColour * textureColour;

	vec3 colour = vec3(0.7, 0.4, 1);

	FragColour = vec4((ambient + diffuse), 1);
}