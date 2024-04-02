#version 460

in vec3 FragPos;

in vec3 N;
in mat3 TBN;

in vec2 FragTexCoords;

uniform vec3 CameraPosition;

uniform vec3 AmbientColour;
uniform vec3 LightColour;
uniform vec3 LightDirection;


uniform sampler2D ColourMap;
uniform vec3 ColourTint;

uniform sampler2D NormalMap;

uniform sampler2D SpecularMap;
uniform float SpecularGlossiness;

out vec4 FragColour;

void main()
{
	vec3 lightDir = normalize(LightDirection);
	
	vec3 normal = texture(NormalMap, FragTexCoords).rgb;
	normal = normalize(TBN * (normal * 2.0 - 1.0));
	
	vec3 viewDirection = normalize(CameraPosition - FragPos);
	vec3 reflectionDirection = -reflect(-LightDirection, normal);
	
	vec3 specularColour = texture(SpecularMap, FragTexCoords).rgb;
	
	
	float lightDot = dot(normal, -lightDir);
	float diffuseComponent = lightDot;
	float specularComponent = pow(max(dot(viewDirection, reflectionDirection), 0.0), SpecularGlossiness);
	
	if (lightDot < 0)
	{
		diffuseComponent = 0;
		specularComponent = 0;
	}
	
	vec3 specularResult = specularColour * specularComponent * LightColour;
	
	
	vec3 textureColour = texture(ColourMap, FragTexCoords).rgb;

	vec3 diffuseResult = LightColour * diffuseComponent * textureColour * ColourTint;
	vec3 ambientResult = AmbientColour * textureColour;
	
	vec3 result = vec3(diffuseResult + specularResult + ambientResult);
	
	FragColour = vec4(result, 1);
}