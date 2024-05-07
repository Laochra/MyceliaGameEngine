#version 460

in vec4 VertColour;

out vec4 FragColour;

void main() // Frag
{
	FragColour = VertColour;
}
