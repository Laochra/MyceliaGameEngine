#include "ParticleSystem.h"

#include "GeneralMacros.h"
#include "RandomGen.h"

ParticleSystem::ParticleSystem(Properties propertiesInit) noexcept
{
	properties = propertiesInit;

	if (properties.autoplay)
	{
		if (properties.delay > 0.0f) state = WaitingToStart;
		else Start();
	}
}
ParticleSystem::~ParticleSystem() noexcept
{
	glDeleteBuffers(1, &positionBuffer);
	glDeleteBuffers(1, &velocityBuffer);
	glDeleteBuffers(1, &colourBuffer);
	glDeleteBuffers(1, &sizeBuffer);
}

void ParticleSystem::Start() noexcept
{
	state = Playing;

	int bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;

	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, properties.maxCount * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);

	vec4* positions = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, properties.maxCount * sizeof(vec4), bufMask);
	for (uint i = 0; i < properties.maxCount; i++)
	{
		// Active status is packed into the 4th component of position since it will otherwise always be 1.0
		float active = true;
		vec3 pos(1.0f);
		switch (properties.shape)
		{
		case Shape::Sphere:
		{
			float theta = std::acos(Random::Float(-1, 1));
			float phi = Random::Float(0, glm::pi<float>() * 2.0f);
			pos.x = std::sin(theta) * std::cos(phi);
			pos.y = std::sin(theta) * std::sin(phi);
			pos.z = std::cos(theta);

			pos *= Random::Float(properties.InnerRadius(), properties.OuterRadius());
			break;
		}
		case Shape::Cone:
		{
			float theta = Random::Float(-properties.ConeAngle(), properties.ConeAngle());
			float phi = Random::Float(0, glm::pi<float>() * 2.0f);
			pos.x = std::sin(theta) * std::cos(phi);
			pos.y = std::sin(theta) * std::sin(phi);
			pos.z = std::cos(theta);

			float innerBaseRadius = properties.InnerRadius() * tan(theta);
			float outerBaseRadius = properties.OuterRadius() * tan(theta);

			float distanceMin = sqrt(sqr(properties.InnerRadius()) + sqr(innerBaseRadius));
			float distanceMax = sqrt(sqr(properties.OuterRadius()) + sqr(outerBaseRadius));

			pos *= Random::Float(distanceMin, distanceMax);
			break;
		}
		case Shape::Box:
		{
			pos.x = Random::Float(-properties.Width()  * 0.5f, properties.Width()  * 0.5f);
			pos.y = Random::Float(-properties.Height() * 0.5f, properties.Height() * 0.5f);
			pos.z = Random::Float(-properties.Depth()  * 0.5f, properties.Depth()  * 0.5f);
			break;
		}
		case Shape::Quad:
		{
			// set to random point on an oriented quad
			break;
		}
		case Shape::Circle:
		{
			// set to random point on an oriented circle
			break;
		}
		}
		positions[i] = vec4(pos, active);
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


	glGenBuffers(1, &velocityBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, properties.maxCount * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);

	vec4* velocities = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, properties.maxCount * sizeof(vec4), bufMask);
	for (uint i = 0; i < properties.maxCount; i++)
	{
		// Lifetime is packed into the 4th component of velocity since it will otherwise always be 0.0
		float lifetime = Random::Float(properties.lifetimeRange[0], properties.lifetimeRange[1]);
		velocities[i] = vec4(glm::normalize(vec3(positions[i])) * Random::Float(properties.speedRange[0], properties.speedRange[1]), lifetime);
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


	glGenBuffers(1, &colourBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, colourBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, properties.maxCount * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);

	Colour* colours = (Colour*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, properties.maxCount * sizeof(Colour), bufMask);
	for (uint c = 0; c < properties.maxCount; c++)
	{
		float h = properties.colourRange[0] < properties.colourRange[5] ? Random::Float(properties.colourRange[0], properties.colourRange[5]) : Random::Float(properties.colourRange[5], properties.colourRange[0]);
		float s = properties.colourRange[1] < properties.colourRange[6] ? Random::Float(properties.colourRange[1], properties.colourRange[6]) : Random::Float(properties.colourRange[6], properties.colourRange[1]);
		float v = properties.colourRange[2] < properties.colourRange[7] ? Random::Float(properties.colourRange[2], properties.colourRange[7]) : Random::Float(properties.colourRange[7], properties.colourRange[2]);
		float a = properties.colourRange[3] < properties.colourRange[8] ? Random::Float(properties.colourRange[3], properties.colourRange[8]) : Random::Float(properties.colourRange[8], properties.colourRange[3]);
		float i = properties.colourRange[4] < properties.colourRange[9] ? Random::Float(properties.colourRange[4], properties.colourRange[9]) : Random::Float(properties.colourRange[9], properties.colourRange[4]);
		HDRColour hsv = HDRColour(h, s, v, a, i);
		HDRColour colour = HDRColour::HSVToRGB(hsv);
		colours[c] = Colour(colour.r * colour.i, colour.g * colour.i, colour.b * colour.i, colour.a);

		HDRColour backToHSV = HDRColour::RGBToHSV(colour);
		int sdfsf = 0;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


	glGenBuffers(1, &sizeBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, sizeBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, properties.maxCount * sizeof(vec2), NULL, GL_DYNAMIC_DRAW);

	vec2* sizes = (vec2*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, properties.maxCount * sizeof(vec2), bufMask);
	for (uint i = 0; i < properties.maxCount; i++)
	{
		vec2 spriteSizeRatio(1, 1);
		sizes[i] = spriteSizeRatio * Random::Float(properties.sizeRange[0], properties.sizeRange[1]);
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ParticleSystem::Stop() noexcept
{
	state = Stopped;
	elapsedTime = 0.0f;

	glDeleteBuffers(1, &positionBuffer);
	glDeleteBuffers(1, &velocityBuffer);
	glDeleteBuffers(1, &colourBuffer);
	glDeleteBuffers(1, &sizeBuffer);
}

void ParticleSystem::Dispatch() const noexcept
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positionBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocityBuffer);

	float workGroups = ceil(properties.maxCount / (float)particlesWorkGroupSize);
	glDispatchCompute(workGroups, 1, 1); // Y and Z are 1 because this is just a 1 dimensional buffer
}

const float ParticleSystem::GetElapsedTime() const noexcept
{
	return elapsedTime;
}

const uint ParticleSystem::GetPositionBuffer() const noexcept
{
	return positionBuffer;
}
const uint ParticleSystem::GetVelocityBuffer() const noexcept
{
	return velocityBuffer;
}
const uint ParticleSystem::GetColourBuffer() const noexcept
{
	return colourBuffer;
}
const uint ParticleSystem::GetSizeBuffer() const noexcept
{
	return sizeBuffer;
}
