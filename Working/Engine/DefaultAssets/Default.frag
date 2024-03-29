#version 460

in vec3 FragPos;

in vec3 N;
in mat3 TBN;

in vec2 vTexCoords;

uniform vec3 AmbientColour;
uniform vec3 LightColour;
uniform vec3 LightDirection;

uniform sampler2D ColourMap;
uniform vec3 Colour;
uniform sampler2D NormalMap;

out vec4 FragColour;

void main()
{
	vec3 L = TBN * normalize(LightDirection);
	
	vec3 normal = texture(NormalMap, vTexCoords).rgb;
	normal = normal * 2.0 - 1.0;
	
	float lambertTerm = max(0, min( 1, dot(normal, -L) ) );

	vec3 textureColour = texture(ColourMap, vTexCoords).rgb;

	vec3 diffuse = LightColour * lambertTerm * textureColour * Colour;
	vec3 ambient = AmbientColour * textureColour;

	FragColour = vec4((ambient + diffuse), 1);
}