#pragma once

#include "GLIncludes.h"

#include <vector>
using std::vector;

#include <cstdlib>

const int particlesWorkGroupSize = 64;

typedef unsigned int uint;

class ParticleSystem
{
	friend class ParticleEmitter;

public:
	ParticleSystem(uint maxParticlesInit = 100) : maxParticles(maxParticlesInit)
	{
		int bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;

		glGenBuffers(1, &positionBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);

		vec4* positions = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, maxParticles * sizeof(vec4), bufMask);
		for (uint i = 0; i < maxParticles; i++)
		{
			// Active status is packed into the 4th component of position since it will otherwise always be 1.0
			float active = 1.0f;
			positions[i] = vec4(vec3(0.0f), active);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


		glGenBuffers(1, &velocityBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);

		vec4* velocities = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, maxParticles * sizeof(vec4), bufMask);
		for (uint i = 0; i < maxParticles; i++)
		{
			// There is room to pack something into velocity's 4th component (compute only)
			velocities[i] = vec4(((std::rand() % 100) - 50) * 0.01f, ((std::rand() % 100) - 50) * 0.01f, ((std::rand() % 100) - 50) * 0.01f, 0.0f);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


		glGenBuffers(1, &colourBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, colourBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);

		vec4* colours = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, maxParticles * sizeof(vec4), bufMask);
		for (uint i = 0; i < maxParticles; i++)
		{
			vec3 colour;
			if (vec3(velocities[i]) == vec3(0, 0, 0)) colour = vec3(0, 0, 0);
			else colour = glm::normalize(vec3(velocities[i]));
			colours[i] = vec4(vec3(abs(colour.x), abs(colour.y), abs(colour.z)), 1.0f);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	~ParticleSystem()
	{
		glDeleteBuffers(1, &positionBuffer);
		glDeleteBuffers(1, &velocityBuffer);
		glDeleteBuffers(1, &colourBuffer);
	}

	// Bind ShaderProgram and Uniforms First
	void Dispatch()
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positionBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocityBuffer);

		float workGroups = ceil(maxParticles / (float)particlesWorkGroupSize);
		glDispatchCompute(workGroups, 1, 1); // Y and Z are 1 because this is just a 1 dimensional buffer
		
	}

	const uint GetMaxParticles() const { return maxParticles; }

	const uint GetPositionBuffer() const { return positionBuffer; }
	const uint GetVelocityBuffer() const { return velocityBuffer; }
	const uint GetColourBuffer() const { return colourBuffer; }

private:
	uint maxParticles;

	// vec4 buffer
	uint positionBuffer;
	// vec4 buffer
	uint velocityBuffer;
	// vec4 buffer
	uint colourBuffer;
};