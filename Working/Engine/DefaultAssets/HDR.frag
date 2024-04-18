#version 460

uniform sampler2D HDRTexture;
uniform sampler2D CurrentFramebuffer;
uniform float Exposure;

in vec2 FragTexCoords;

out vec4 FragColour;

void main()
{
	vec4 hdrColour = texture(HDRTexture, FragTexCoords);
	vec4 currentColour = vec4(texture(CurrentFramebuffer, FragTexCoords).xyz, 1.0 - hdrColour.w);
	
	vec4 remapped = vec4(vec3(1.0) - exp(-hdrColour.xyz * Exposure), hdrColour.w);
	remapped.xyz = pow(remapped.xyz, vec3(0.45)); // Colour to the power of 1/2.2 to return to non-linear space
	
	FragColour = vec4(currentColour.xyz * currentColour.w, currentColour.w) + vec4(remapped.xyz * remapped.w, remapped.w);
}