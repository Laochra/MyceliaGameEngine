#pragma once

#include "GLIncludes.h"

#include <vector>
using std::vector;

#include <cstdlib>

const int particlesWorkGroupSize = 1;

typedef unsigned int uint;

class ParticleSystem
{
public:
	ParticleSystem(uint maxParticlesInit = 100) : maxParticles(maxParticlesInit)
	{
		int bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;

		glGenBuffers(1, &positionBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);

		vec4* positions = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, maxParticles * sizeof(vec4), bufMask);
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


		glGenBuffers(1, &velocityBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);

		vec4* velocities = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, maxParticles * sizeof(vec4), bufMask);
		for (uint i = 0; i < maxParticles; i++)
		{
			velocities[i] = vec4(((std::rand() % 100) - 50) * 0.01f, ((std::rand() % 100) - 50) * 0.01f, ((std::rand() % 100) - 50) * 0.01f, 1.0f);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


		glGenBuffers(1, &colourBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, colourBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);

		vec4* colours = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, maxParticles * sizeof(vec4), bufMask);
		for (uint i = 0; i < maxParticles; i++)
		{
			colours[i] = vec4(1.0f);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


		glGenBuffers(1, &activeStatusBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, activeStatusBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);

		bool* activeStatuses = (bool*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, maxParticles * sizeof(bool), bufMask);
		for (uint i = 0; i < maxParticles; i++)
		{
			activeStatuses[i] = true;
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	~ParticleSystem()
	{
		glDeleteBuffers(1, &positionBuffer);
		glDeleteBuffers(1, &velocityBuffer);
		glDeleteBuffers(1, &colourBuffer);
		glDeleteBuffers(1, &activeStatusBuffer);
	}

	// Bind ShaderProgram and Uniforms First
	void Dispatch()
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positionBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocityBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, colourBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, activeStatusBuffer);

		float workGroups = ceil(maxParticles / (float)particlesWorkGroupSize);
		glDispatchCompute(workGroups, 1, 1); // Y and Z are 1 because this is just a 1 dimensional buffer
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	const uint GetMaxParticles() const { return maxParticles; }

	const uint GetPositionBuffer() const { return positionBuffer; }
	const uint GetVelocityBuffer() const { return velocityBuffer; }
	const uint GetColourBuffer() const { return colourBuffer; }
	const uint GetActiveStatusBuffer() const { return activeStatusBuffer; }

	//vector<Particle> GetParticles() const
	//{
	//	vector<Particle> result(maxParticles);
	//	glGetNamedBufferSubData(ssbo, 0, maxParticles * sizeof(Particle), result.data());
	//	return result;
	//}

private:
	uint maxParticles;

	// vec4 buffer
	uint positionBuffer;
	// vec4 buffer
	uint velocityBuffer;
	// vec4 buffer
	uint colourBuffer;
	// bool buffer
	uint activeStatusBuffer;
};