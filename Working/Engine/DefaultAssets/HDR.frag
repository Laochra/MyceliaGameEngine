#version 460

uniform sampler2D HDRTexture;
uniform float Exposure;

in vec2 FragTexCoords;

out vec4 FragColour;

void main()
{
	vec3 hdrColour = texture(HDRTexture, FragTexCoords).rgb;
	
	vec3 remapped = vec3(1.0) - exp(-hdrColour * Exposure);
	remapped = pow(remapped, vec3(0.45)); // Colour to the power of 1/2.2 to return to non-linear space
	
	FragColour = vec4(remapped, 1);
}