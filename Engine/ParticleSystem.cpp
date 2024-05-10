#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(Properties propertiesInit) noexcept
{
	properties = propertiesInit;

	if (properties.autoPlay)
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
		float active = i < properties.startingCount;
		positions[i] = vec4(vec3(0.0f), active);
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


	glGenBuffers(1, &velocityBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, properties.maxCount * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);

	vec4* velocities = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, properties.maxCount * sizeof(vec4), bufMask);
	for (uint i = 0; i < properties.maxCount; i++)
	{
		// There is room to pack something into velocity's 4th component (compute only)
		velocities[i] = vec4(((std::rand() % 100) - 50) * 0.01f, ((std::rand() % 100) - 50) * 0.01f, ((std::rand() % 100) - 50) * 0.01f, 0.0f);
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


	glGenBuffers(1, &colourBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, colourBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, properties.maxCount * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);

	vec4* colours = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, properties.maxCount * sizeof(vec4), bufMask);
	for (uint i = 0; i < properties.maxCount; i++)
	{
		vec3 colour;
		if (vec3(velocities[i]) == vec3(0, 0, 0)) colour = vec3(0, 0, 0);
		else colour = glm::normalize(vec3(velocities[i]));
		colours[i] = vec4(vec3(abs(colour.x), abs(colour.y), abs(colour.z)), 1.0f);
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
