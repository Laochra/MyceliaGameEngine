#version 460

in vec4 Colour;

layout (location = 0) out vec4 FragColour;
layout (location = 1) out vec4 BrightColour;

void main() // Frag
{
	if (Colour.a == 0.0) discard;
	
	FragColour = pow(Colour, vec4(2.2));
	
	const vec3 brightnessConstant = vec3(0.2126, 0.7152, 0.0722);
	float brightness = dot(FragColour.rgb, brightnessConstant);
	BrightColour = vec4(brightness > 1.0 ? vec4(FragColour.xyz, min(brightness - 1.0, 1.0)) : vec4(0.0));
}
