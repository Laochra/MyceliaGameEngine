#include "HexFog.h"

#include "HexCoords.h"
#include "HexGrid.h"

#include "GLIncludes.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "stb_image.h"
#include <algorithm>

typedef unsigned char ubyte;
typedef unsigned short ushort;

float HexFog::currentRadius = 3.0f;
float HexFog::gradientRange = 0.25f;

uint HexFog::fogDistanceField;
Texture::Filter HexFog::filter;

static mat2 RadToMat2(float radians) noexcept
{
	return mat2(
		cos(radians), -sin(radians),
		sin(radians), cos(radians)
	);
}

void HexFog::Load(const char* filepath, Texture::Filter filterInit) noexcept
{
	if (fogDistanceField != 0) glDeleteTextures(1, &fogDistanceField);

	int w, h, components;
	float* loadedPixels = stbi_loadf(filepath, &w, &h, &components, 3);
	
	if (loadedPixels != nullptr)
	{
		filter = filterInit;
	
		glGenTextures(1, &fogDistanceField);
		glBindTexture(GL_TEXTURE_2D, fogDistanceField);
	
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, loadedPixels);

		switch (filterInit)
		{
		case Texture::Filter::None:
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;
		}
		case Texture::Filter::Bilinear:
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		}
		case Texture::Filter::Trilinear:
		{
			glGenerateMipmap(GL_TEXTURE_2D);
			float anisotropy = 0.0f;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &anisotropy);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		}
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(loadedPixels);
	}
}

void HexFog::Bind(int slot) noexcept
{
	if (fogDistanceField == 0) return;
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, fogDistanceField);
}

const Texture::Filter HexFog::GetFilter() noexcept
{
	return filter;
}

const float rad[8]{
	glm::radians(-300.0f), // This just exists to avoid a branch protecting rad[i-1]
	glm::radians(-0.0f),
	glm::radians(-60.0f),
	glm::radians(-120.0f),
	glm::radians(-180.0f),
	glm::radians(-240.0f),
	glm::radians(-300.0f),
	glm::radians(-0.0f), // This just exists to avoid a branch protecting rad[i+1]
};
const mat2 rot[8]{
	RadToMat2(rad[0]), // This just exists to avoid a branch protecting rot[i-1]
	RadToMat2(rad[1]),
	RadToMat2(rad[2]),
	RadToMat2(rad[3]),
	RadToMat2(rad[4]),
	RadToMat2(rad[5]),
	RadToMat2(rad[6]),
	RadToMat2(rad[7]), // This just exists to avoid a branch protecting rot[i+1]
};
constexpr const ushort sideLength = 4096;
constexpr const float invHexSize = 1.0f / sideLength / HEX_GRID_RADIUS;
constexpr const ubyte components = 3;
void HexFog::MakeHexagonalDistanceField() noexcept
{
	float* image = new float[sideLength * sideLength * components];
	for (ushort x = 0; x < sideLength; x++)
	{
		for (ushort y = 0; y < sideLength; y++)
		{
			vec2 position = (vec2(x, y) - vec2(sideLength * 0.5f)) * invHexSize;
			HexCubeCoord cubeCoord = HexCubeCoord::GetFromPos(position);
			vec2 hexPos = HexCubeCoord::ToPos(cubeCoord);
			vec2 relativePos = hexPos - position;

			vec3 colour = vec3();

			int index;
			bool onAxis;

			short& q = cubeCoord.q;
			short& r = cubeCoord.r;
			short s = -q-r;

			if (q == 0)
			{
				onAxis = true;
				index = (r < 0) ? 1 : 4;
			}
			else if (s == 0)
			{
				onAxis = true;
				index = (q > 0) ? 2 : 5;
			}
			else if (r == 0)
			{
				onAxis = true;
				index = (s < 0) ? 3 : 6;
			}
			else
			{
				onAxis = false;
				if (abs(r) > abs(q) && abs(r) > abs(s)) index = (r < 0) ? 1 : 4;
				else if (abs(q) > abs(s)) index = (q > 0) ? 2 : 5;
				else index = (s < 0) ? 3 : 6;
			}

			if (onAxis)
			{
				float sDist1 = (relativePos * rot[index - 1]).y;
				float sDist2 = (relativePos * rot[index]).y;
				float sDist3 = (relativePos * rot[index + 1]).y;
				float maxY = (sDist1 > sDist2 ? (sDist1 > sDist3 ? sDist1 : sDist3) : (sDist2 > sDist3 ? sDist2 : sDist3));
				
				colour.r = maxY + 0.5f + cubeCoord.GetMagnitude();
			}
			else
			{
				float sDist1 = (relativePos * rot[index]).y;
				float sDist2 = (relativePos * rot[index + 1]).y;
				float maxY = (sDist1 > sDist2 ? sDist1 : sDist2);

				colour.r = maxY + 0.5f + cubeCoord.GetMagnitude();
			}

			if (cubeCoord == HexCubeCoord(0, 0))
			{
				colour.r = 0.5f + HexCubeCoord::GetMagnitudePartial(HexCubeCoord::GetFromPosPartial(vec2(position.y, position.x)));
			}

			image[y * sideLength * components + x * components] = colour.r;
			//image[y * sideLength * components + x * components + 1] = colour.g;
			//image[y * sideLength * components + x * components + 2] = colour.b;
		}
	}
	stbi_write_hdr("Assets\\HexDF.hdr", sideLength, sideLength, components, image);
	delete image;
}
void HexFog::MakeHexagonImage() noexcept
{
	constexpr const ushort sideLength = 512;
	constexpr const ubyte components = 2;
	ubyte* image = new ubyte[sideLength * sideLength * components];
	for (ushort x = 0; x < sideLength; x++)
	{
		for (ushort y = 0; y < sideLength; y++)
		{
			vec2 position = vec2(x, y) / float(sideLength * 0.5f) - 1.0f;
			vec2 partialCubeCoord = HexCubeCoord::GetFromPosPartial(position);
			float signedDistance = HexCubeCoord::GetMagnitudePartial(partialCubeCoord);
			ubyte alpha = signedDistance > 1.0f ? 0 : 255;
			ubyte colour = signedDistance > 1.0f ? 0 : 255;

			image[x * sideLength * components + y * components] = colour;
			image[x * sideLength * components + y * components + 1] = alpha;
		}
	}
	stbi_write_png("Assets\\HexImage.png", sideLength, sideLength, components, image, sideLength * components * (ushort)sizeof(ubyte));
	delete image;
}
