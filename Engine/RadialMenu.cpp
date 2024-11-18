#include "RadialMenu.h"
#include "AppInfo.h"

#include "stb_image.h"

#include "Camera.h"

#include "Debug.h"

RadialMenu::RadialMenu(const char* regularSprite, const char* hoveredSprite, const char* disabledSprite) noexcept
{
	spriteMesh.InitialiseSpriteQuad();
	program.LoadAndLinkFromJSON("Assets\\Shaders\\RadialMenu.gpu");

	if (regularSprite == string("None")) { Debug::LogError("Sprite 1 was not provided for radial menu.", locationinfo); }
	if (hoveredSprite == string("None")) { Debug::LogError("Sprite 2 was not provided for radial menu.", locationinfo); }
	if (disabledSprite == string("None")) { Debug::LogError("Sprite 3 was not provided for radial menu.", locationinfo); }

	int x1, y1, channels1;
	unsigned char* sprite1 = stbi_load(regularSprite, &x1, &y1, &channels1, STBI_default);
	int x2, y2, channels2;
	unsigned char* sprite2 = stbi_load(hoveredSprite, &x2, &y2, &channels2, STBI_default);
	int x3, y3, channels3;
	unsigned char* sprite3 = stbi_load(disabledSprite, &x3, &y3, &channels3, STBI_default);

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
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, glChannels, x1, x2, 3, 0, glChannels, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTextureSubImage3D(spriteArray, 0, 0, 0, 0, x1, y1, 1, glChannels, GL_UNSIGNED_BYTE, sprite1);
	glTextureSubImage3D(spriteArray, 0, 0, 0, 1, x2, y2, 1, glChannels, GL_UNSIGNED_BYTE, sprite2);
	glTextureSubImage3D(spriteArray, 0, 0, 0, 2, x3, y3, 1, glChannels, GL_UNSIGNED_BYTE, sprite3);
}
RadialMenu::~RadialMenu() noexcept
{
	glDeleteTextures(1, &spriteArray);
}

static uint GetSliceFromInput(vec2 input, ushort radialSlices)
{
	vec2 inputDirection = input / glm::length(input);

	float increment = 360.0f / radialSlices;
	vector<vec2> directions;
	for (ushort i = 0; i < radialSlices; i++)
	{
		float rad = glm::radians(-increment * i);
		vec2 dir(std::sin(rad), std::cos(rad));

		directions.push_back(dir);
	}
	
	ushort correctIndex = 0;
	
	for (ushort i = 1; i != (ushort)directions.size(); i++)
	{
		if (glm::dot(inputDirection, directions[i]) > glm::dot(inputDirection, directions[correctIndex]))
		{
			correctIndex = i;
		}
	}

	return correctIndex;
}
void RadialMenu::Update(vec2 input, InputBind interactKey) noexcept
{
	if (!enabled) return;

	float inputMagSqr = glm::length2(input);
	bool noInput = inputMagSqr < deadzoneMagnitudeSqr;
	
	if (noInput)
	{
		if (!initialInputGiven) lastInput = vec2(0.0f, 1.0f);
		input = lastInput;
		inputMagSqr = glm::length2(input);
	}

	ushort correctIndex = GetSliceFromInput(input, radialSlices);
	if (!sliceEnabledFlags[correctIndex])
	{
		correctIndex = GetSliceFromInput(lastInput, radialSlices);
		input = lastInput;
	}

	if (interactKey.pressed())
	{
		interactionHandler(correctIndex);
		lastInput = vec2();
		initialInputGiven = false;
	}

	if (!noInput)
	{
		initialInputGiven = true;
		lastInput = input;
	}
}

void RadialMenu::Draw() noexcept
{
	if (!enabled) return;

	float inputMagSqr = glm::length2(lastInput);
	vec2 inputDirection = lastInput / sqrt(inputMagSqr);

	program.Bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, spriteArray);
	program.BindUniform("Sprites", 0);

	float aspect = 1.0f / (AppInfo::screenHeight == 0 ? 0.0f : (AppInfo::screenWidth / (float)AppInfo::screenHeight));
	program.BindUniform("Aspect", aspect);
	
	program.BindUniform("Scale", scale);

	program.BindUniform("SliceCount", (int)radialSlices);
	
	ushort enabledFlags = 0;

	float increment = 360.0f / radialSlices;
	vector<vec2> directions;
	for (ushort i = 0; i < radialSlices; i++)
	{
		float rad = glm::radians(-increment * i);
		vec2 dir(std::sin(rad), std::cos(rad));

		directions.push_back(dir);

		enabledFlags |= ((int)sliceEnabledFlags[i] << i);
		program.BindUniform(StringBuilder("SliceDirections[", i, "]").CStr(), dir);
	}
	program.BindUniform("SliceEnabledFlags", (int)enabledFlags);

	if (inputMagSqr < deadzoneMagnitudeSqr)
	{
		program.BindUniform("HoveredSlice", -1);
		spriteMesh.Draw();
		return;
	}

	int hoveredSlice = 0;
	float hoveredAngle = glm::dot(inputDirection, directions[0]);
	for (ushort i = 1; i < radialSlices; i++)
	{
		float newAngle = glm::dot(inputDirection, directions[i]);
		if (newAngle > hoveredAngle)
		{
			hoveredSlice = i;
			hoveredAngle = newAngle;
		}
	}
	program.BindUniform("HoveredSlice", hoveredSlice);
	spriteMesh.Draw();
}
