#version 460

in flat vec3 HexPosition;

out vec4 Colour;

void main() // Frag
{
	Colour = vec4(HexPosition, 1.0);
}
