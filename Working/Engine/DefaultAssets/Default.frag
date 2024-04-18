#version 460

#define PI 3.14159265359

in vec3 FragPos;

in vec3 N;
in mat3 TBN;

in vec2 FragTexCoords;

uniform vec3 CameraPosition;

struct DirectionalLight
{
	vec3 colour;
	vec3 direction;
};
uniform int DirectionalLightCount;
uniform DirectionalLight DirectionalLights[4];

struct PointLight
{
	vec3 colour;
	vec3 position;
	float range;
};
uniform int PointLightCount;
uniform PointLight PointLights[4];

uniform sampler2D ColourMap;
uniform vec3 ColourTint;

uniform sampler2D NormalMap;

uniform sampler2D RMAOMap; // Roughness, Metallic, Ambient Occlusion

out vec4 FragColour;

float NormalDistribution(vec3 N, vec3 H, float a) // Trowbridge-Reitz GGX Normal Distribution Approximation
{
	float aSqr = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotHSqr = NdotH * NdotH;
	
	float denominator = (NdotHSqr * (aSqr - 1.0) + 1.0);
	denominator = PI * denominator * denominator;
	
	return aSqr / denominator;
}

float GeometryGGX(float NdotV, float k) // Schick-GGX Geometry Approximation
{
	float denominator = NdotV * (1.0 - k) + k;
	
	return NdotV / denominator;
}
float Geometry(vec3 N, vec3 V, vec3 L, float k) // Smith Geometry Approximation
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	
	return GeometryGGX(NdotV, k) * GeometryGGX(NdotL, k);
}

vec3 Fresnel(float cosTheta, vec3 F0) // Fresnel-Schlick Fresnel Approximation
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 ReflectanceEquation(vec3 colour, vec3 normal, float roughness, float metallic, float ao, vec3 viewDirection, vec3 lightDirection, float attenuation, vec3 lightColour, vec3 F0)
{
	// Calculate Per-Light Radiance
	
	vec3 H = normalize(viewDirection + lightDirection);
	vec3 radiance = lightColour * attenuation;
	
	// Cook-Torrance BRDF
	float NDF = NormalDistribution(normal, H, roughness);
	float G = Geometry(normal, viewDirection, lightDirection, roughness);
	vec3 F = Fresnel(max(dot(H, viewDirection), 0.0), F0);
	
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;
	
	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(normal, viewDirection), 0.0) * max(dot(normal, lightDirection), 0.0) + 0.0001;
	vec3 specular = numerator / denominator;
	
	// Return amount to add
	float NdotL = max(dot(normal, lightDirection), 0.0);
	return (kD * colour / PI + specular) * radiance * NdotL;
}

void main() // Fragment
{	
	// Material Inputs
	vec3 colour = texture(ColourMap, FragTexCoords).rgb * ColourTint;
	vec3 normal = texture(NormalMap, FragTexCoords).rgb; normal = normalize(TBN * (normal * 2.0 - 1.0));
	float roughness = texture(RMAOMap, FragTexCoords).r;
	float metallic = texture(RMAOMap, FragTexCoords).g;
	float ao = texture(RMAOMap, FragTexCoords).b;
	
	vec3 viewDirection = normalize(CameraPosition - FragPos);
	
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, colour, metallic);
	
	// Reflectance Equation
	vec3 Lo = vec3(0.0);
	for (int i = 0; i < min(DirectionalLightCount, 4); i++) // DirectionalLights
	{
		float attenuation = (DirectionalLights[i].colour != vec3(0.0)) ? 1.0 : 0.0;
		Lo += ReflectanceEquation(colour, normal, roughness, metallic, ao, viewDirection, -DirectionalLights[i].direction, attenuation, DirectionalLights[i].colour, F0);
	}
	for (int i = 0; i < min(PointLightCount, 4); i++) // PointLights
	{
		vec3 lightDirection = normalize(PointLights[i].position - FragPos);
		float d = length(PointLights[i].position - FragPos);
		float dSqr = d * d;
		float attenuation = clamp(PointLights[i].range / dSqr, 0.0, 1.0);
		
		if (d > PointLights[i].range) attenuation = 0.0;
		
		Lo += ReflectanceEquation(colour, normal, roughness, metallic, ao, viewDirection, lightDirection, attenuation, PointLights[i].colour, F0);
	}
	
	vec3 ambientResult = vec3(0.03) * colour * ao;
	vec3 colourResult = ambientResult + Lo;
	
	//colourResult = pow(colourResult, vec3(0.45)); // Colour to the power of 1/2.2 to return to non-linear space
	
	FragColour = vec4(colourResult, 1);
	
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
	
	// Display Point Light Attenuation
	//FragColour = vec4(vec3(attenuation), 1);
	
	// Display Depth
	//float near = 0.01;
	//float far = 10.0;
	//float z = gl_FragCoord.z * 2.0 - 1.0;
	//float depth = (2.0 * near * far) / (far + near - z * (far - near));
	//FragColour = vec4(vec3(depth / far), 1);
}