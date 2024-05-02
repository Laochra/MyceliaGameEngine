#pragma once

#include "GLIncludes.h"

#define NUM_PARTICLES 1024 * 1024
#define WORK_GROUP_SIZE 128

class ParticleSystem
{
	void Setup()
	{
		// Position
		glGenBuffers(1, &positionSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_PARTICLES * sizeof(vec4), NULL, GL_STATIC_DRAW);

		int bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;

		vec4* points = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, NUM_PARTICLES * sizeof(vec4), bufMask);
		for (int i = 0; i < NUM_PARTICLES; i++)
		{
			points[i] = vec4(0, 0, 0, 1);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		// Velocity
		glGenBuffers(1, &velocitySSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocitySSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_PARTICLES * sizeof(vec4), NULL, GL_STATIC_DRAW);

		vec4* velocities = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, NUM_PARTICLES * sizeof(vec4), bufMask);
		for (int i = 0; i < NUM_PARTICLES; i++)
		{
			velocities[i] = vec4(0, 0, 0, 0);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		// Colour
		glGenBuffers(1, &colourSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, colourSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_PARTICLES * sizeof(vec4), NULL, GL_STATIC_DRAW);

		vec4* colours = (vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, NUM_PARTICLES * sizeof(vec4), bufMask);
		for (int i = 0; i < NUM_PARTICLES; i++)
		{
			colours[i] = vec4(1, 1, 1, 1);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void Invoke(int program)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positionSSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocitySSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, colourSSBO);

		glUseProgram(program);
		glDispatchCompute(NUM_PARTICLES / WORK_GROUP_SIZE, 1, 1); // Y and Z are 1 because this is just a 1 dimensional buffer
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	unsigned int positionSSBO;
	unsigned int velocitySSBO;
	unsigned int colourSSBO;
};