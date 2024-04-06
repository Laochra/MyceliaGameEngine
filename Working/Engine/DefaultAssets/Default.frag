#version 460

in vec3 FragPos;

in vec3 N;
in mat3 TBN;

in vec2 FragTexCoords;

uniform vec3 CameraPosition;

uniform vec3 AmbientColour;

uniform vec3 DirectionalLightColour;
uniform vec3 DirectionalLightDirection;

uniform vec3 PointLightColour;
uniform vec3 PointLightPosition;
uniform float PointLightRange;

uniform sampler2D ColourMap;
uniform vec3 ColourTint;

uniform sampler2D NormalMap;

uniform sampler2D SpecularMap;
uniform float SpecularGlossiness;

out vec4 FragColour;

void main() // Fragment
{	
	vec3 normal = texture(NormalMap, FragTexCoords).rgb;
	normal = normalize(TBN * (normal * 2.0 - 1.0));
	
	vec3 viewDirection = normalize(CameraPosition - FragPos);

	
	vec3 specularColour = texture(SpecularMap, FragTexCoords).rgb;
	
	
	float lightDotD = dot(normal, -DirectionalLightDirection);
	vec3 reflectionDirectionD = -reflect(-DirectionalLightDirection, normal);
	float specularComponentD = pow(max(dot(viewDirection, reflectionDirectionD), 0.0), SpecularGlossiness);
	
	vec3 pointLightDirection = normalize(FragPos - PointLightPosition);
	float attenuation = PointLightRange / length(PointLightPosition - FragPos);
	attenuation = min(attenuation, 1.0);
	float lightDotP = dot(normal, -pointLightDirection) * attenuation;
	vec3 reflectionDirectionP = -reflect(-pointLightDirection, normal);
	float specularComponentP = pow(max(dot(viewDirection, reflectionDirectionP), 0.0) * attenuation, SpecularGlossiness);
	
	float diffuseComponent = clamp(lightDotD + lightDotP, 0, 1);
	
	if (lightDotD < 0)
	{
		specularComponentD = 0;
		
		if (lightDotP < 0)
		{
			diffuseComponent = 0;
			specularComponentP = 0;
		}
		else
		{
			diffuseComponent = clamp(lightDotP, 0, 1);
		}
	}
	else if (lightDotP < 0)
	{
		diffuseComponent = clamp(lightDotD, 0, 1);
		specularComponentP = 0;
	}
	
	vec3 specularResultD = specularColour * specularComponentD * DirectionalLightColour;
	vec3 specularResultP = specularColour * specularComponentP * PointLightColour;
	vec3 specularResult = specularResultD + specularResultP;
	
	vec3 textureColour = texture(ColourMap, FragTexCoords).rgb;

	vec3 diffuseResult = (DirectionalLightColour + (PointLightColour * attenuation)) * diffuseComponent * textureColour * ColourTint;
	vec3 ambientResult = AmbientColour * textureColour;
	
	vec3 result = vec3(diffuseResult + specularResult + ambientResult);
	
	
	FragColour = vec4(result, 1);
	
	// Display Surface Normals
	//FragColour = vec4(N, 1);
	
	// Display Texture Coords
	//FragColour = vec4(FragTexCoords, 0, 1);
	
	// Display Colour Map
	//FragColour = vec4(textureColour, 1);
	
	// Display Normal Map
	//FragColour = vec4(normal, 1);
	
	// Display Specular Map
	//FragColour = vec4(specularColour, 1);
	
	// Display Diffuse Result
	//FragColour = vec4(diffuseResult, 1);
	
	// Display Specular Result
	//FragColour = vec4(specularResult, 1);
	
	// Display Depth
	//float near = 0.01;
	//float far = 10.0;
	//float z = gl_FragCoord.z * 2.0 - 1.0;
	//float depth = (2.0 * near * far) / (far + near - z * (far - near));
	//FragColour = vec4(vec3(depth / far), 1);
}