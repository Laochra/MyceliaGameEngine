#pragma once

#include "GLIncludes.h"

#include <vector>
using std::vector;

#include <cstdlib>

const int particlesWorkGroupSize = 1;

typedef unsigned int uint;

struct Particle
{
	enum Status : bool { Inactive = false, Active = true };

	vec4 position;
	vec4 velocity;
	vec4 colour;
	Status status;

	// The below field is there so that the data is correctly spaced for the GPU (chunks of 16 bytes).
	// The 'status' bool leaves us with 49 bytes which *would* get rounded to 52 for CPU alignment needs,
	// but we add an extra 15 bytes for the GPU to be happy as well.
	// I added 15 bytes instead of just 12 to make it clear how much unused space there actually is.
	char unusedData[15];

	Particle(Status statusInit = Inactive) :
		status(statusInit),
		position(vec4()),
		velocity(vec4()),
		colour(vec4()) {}
	Particle(vec4 positionInit, vec4 velocityInit, vec4 colourInit) :
		position(positionInit),
		velocity(velocityInit),
		colour(colourInit),
		status(Active) {}
};

class ParticleSystem
{
public:
	ParticleSystem(uint maxParticlesInit = 100) : maxParticles(maxParticlesInit)
	{
		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, maxParticles * sizeof(Particle), NULL, GL_STATIC_DRAW);

		int bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;

		Particle* points = (Particle*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, maxParticles * sizeof(Particle), bufMask);
		for (uint i = 0; i < maxParticles; i++)
		{
			points[i] = Particle(Particle::Status(i % 2 == 0));
			points[i].velocity = vec4((std::rand() % 100) - 50, (std::rand() % 100) - 50, (std::rand() % 100) - 50, 1.0f);
			points[i].colour = vec4(i / (float)maxParticles, i % 5 / (float)maxParticles, i % 10 / (float)maxParticles, 1.0f);
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	// Bind ShaderProgram and Uniforms First
	void Dispatch()
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

		float workGroups = ceil(maxParticles / (float)particlesWorkGroupSize);
		glDispatchCompute(workGroups, 1, 1); // Y and Z are 1 because this is just a 1 dimensional buffer
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	const uint GetMaxParticles() const { return maxParticles; }
	const uint GetSSBO() const { return ssbo; }

	vector<Particle> GetParticles() const
	{
		vector<Particle> result(maxParticles);
		glGetNamedBufferSubData(ssbo, 0, maxParticles * sizeof(Particle), result.data());
		return result;
	}

private:
	uint maxParticles;
	uint ssbo;
};