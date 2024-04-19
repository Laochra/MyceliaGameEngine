#version 460

uniform sampler2D BrightTexture;

in vec2 FragTexCoords;

out vec4 FragColour;

void main() // Fragment
{
	vec2 texelSize = 1.0 / textureSize(BrightTexture, 0);
	
	vec4 colourSum;
	for (int xOffset = -4; xOffset <= 4; xOffset++)
	{
		for (int yOffset = -4; yOffset <= 4; yOffset++)
		{
			vec4 colour = texture(BrightTexture, FragTexCoords + vec2(xOffset * texelSize.x, yOffset * texelSize.y));

			colourSum += colour;
		}
	}
	
	FragColour = colourSum / 81;
}
