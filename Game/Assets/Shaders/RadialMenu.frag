#version 460

uniform sampler2DArray Sprites;

uniform float AcceptedHalfAngle;
uniform vec2 HoveredSliceDirection;

in vec2 FragDirection;
in vec2 FragTexCoords;

out vec4 Colour;

void main() // Frag
{
	if (dot(normalize(FragDirection), HoveredSliceDirection) > 1.0 - AcceptedHalfAngle)
	{
		Colour = texture(Sprites, vec3(FragTexCoords, 1));
		//Colour = vec4(0, 1, 0, 1);
	}
	else
	{
		Colour = texture(Sprites, vec3(FragTexCoords, 0));
		//Colour = vec4(1, 0, 0, 1);
	}
}
