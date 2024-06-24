#version 460

in vec4 GUIDColour;

out vec4 Colour;

void main() // Frag
{
	Colour = GUIDColour;
}
