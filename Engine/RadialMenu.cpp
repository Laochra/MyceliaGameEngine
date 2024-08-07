#include "RadialMenu.h"

#include "stb_image.h"

#include "Camera.h"

RadialMenu::RadialMenu() noexcept
{
	spriteMesh.InitialiseSpriteQuad();
	program.LoadAndLinkFromJSON("Assets\\Shaders\\RadialMenu.gpu");

	int x1, y1, channels1;
	unsigned char* sprite1 = stbi_load("Assets\\Textures\\RadialMenuSprite.png", &x1, &y1, &channels1, STBI_default);
	int x2, y2, channels2;
	unsigned char* sprite2 = stbi_load("Assets\\Textures\\RadialMenuSpriteHovered.png", &x2, &y2, &channels2, STBI_default);

	int glChannels;
	switch (channels1)
	{
	case STBI_grey: glChannels = GL_RED; break;
	case STBI_grey_alpha: glChannels = GL_RG; break;
	case STBI_rgb: glChannels = GL_RGB; break;
	case STBI_rgb_alpha: glChannels = GL_RGBA; break;
	default: glChannels = GL_RGB; break;
	}
	glGenTextures(1, &spriteArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, spriteArray);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, glChannels, x1, x2, 2, 0, glChannels, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTextureSubImage3D(spriteArray, 0, 0, 0, 0, x1, y1, 1, glChannels, GL_UNSIGNED_BYTE, sprite1);
	glTextureSubImage3D(spriteArray, 0, 0, 0, 1, x2, y2, 1, glChannels, GL_UNSIGNED_BYTE, sprite2);
}
RadialMenu::~RadialMenu() noexcept
{
	glDeleteTextures(1, &spriteArray);
}

void RadialMenu::Draw(vec2 input) noexcept
{
	float aspect = 1.0f / (screenHeight == 0 ? 0.0f : (screenWidth / (float)screenHeight));
	vec2 inputDirection = vec2(input.x / aspect, input.y);
	float inputMagSqr = glm::length2(inputDirection);
	inputDirection /= sqrt(inputMagSqr);

	program.Bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, spriteArray);
	program.BindUniform("Sprites", 0);

	program.BindUniform("Aspect", aspect);
	
	program.BindUniform("Scale", scale);

	float increment = 360.0f / sliceCount;
	float acceptedHalfAngle = 1.0f - std::cos(glm::radians(increment / 2));
	program.BindUniform("AcceptedHalfAngle", acceptedHalfAngle);

	if (inputMagSqr < deadzoneMagnitudeSqr * scale)
	{
		program.BindUniform("HoveredSliceDirection", vec2(0.0f));
		spriteMesh.Draw();
		return;
	}

	vector<vec2> sortedDirections;
	for (unsigned int i = 0; i < sliceCount; i++)
	{
		float deg = glm::radians(increment * i);
		vec2 dir(std::sin(deg), std::cos(deg));
		if (sortedDirections.size() == 0)
		{
			sortedDirections.push_back(dir);
			continue;
		}

		for (vector<vec2>::iterator it = sortedDirections.begin(); it != sortedDirections.end(); it++)
		{
			if (glm::dot(inputDirection, dir) > glm::dot(inputDirection, *it))
			{
				it = sortedDirections.insert(it, dir);
				break;
			}
			else
			{
				if (it + 1 == sortedDirections.end())
				{
					it = sortedDirections.insert(sortedDirections.end(), dir);
				}
			}
		}
	}

	program.BindUniform("HoveredSliceDirection", sortedDirections[0]);
	spriteMesh.Draw();
}
