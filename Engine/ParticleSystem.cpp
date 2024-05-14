#include "ParticleSystem.h"

#include "RandomGen.h"

ParticleSystem::ParticleSystem(Properties propertiesInit) noexcept
{
	properties = propertiesInit;

	if (properties.autoplay)
	{
		state = Playing;
		Start();
	}
}
ParticleSystem::~ParticleSystem() noexcept
{
	glDeleteBuffers(1, &positionBuffer);
	glDeleteBuffers(1, &velocityBuffer);
	glDeleteBuffers(1, &colourBuffer);
}

void ParticleSystem::Start() noexcept
{
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
			// set to random point in sphere
			break;
		}
		case Shape::Cone:
		{
			// set to random point in cone
			break;
		}
		case Shape::Box:
		{
			pos.x = Random::Float(-properties.shapeData[0] * 0.5f, properties.shapeData[0] * 0.5f);
			pos.y = Random::Float(-properties.shapeData[1] * 0.5f, properties.shapeData[1] * 0.5f);
			pos.z = Random::Float(-properties.shapeData[2] * 0.5f, properties.shapeData[2] * 0.5f);
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
		// There is room to pack something into velocity's 4th component (compute only)
		velocities[i] = vec4(glm::normalize(vec3(positions[i])) * Random::Float(properties.speedRange[0], properties.speedRange[1]), 0.0f);
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


	glGenBuffers(1, &colourBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, colourBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, properties.maxCount * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);

	vec4* colours = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, properties.maxCount * sizeof(vec4), bufMask);
	for (uint i = 0; i < properties.maxCount; i++)
	{
		colours[i] = *(vec4*)&properties.colour;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ParticleSystem::Stop() noexcept
{
	glDeleteBuffers(1, &positionBuffer);
	glDeleteBuffers(1, &velocityBuffer);
	glDeleteBuffers(1, &colourBuffer);
}

void ParticleSystem::Dispatch() const noexcept
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positionBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocityBuffer);

	float workGroups = ceil(properties.maxCount / (float)particlesWorkGroupSize);
	glDispatchCompute(workGroups, 1, 1); // Y and Z are 1 because this is just a 1 dimensional buffer
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
