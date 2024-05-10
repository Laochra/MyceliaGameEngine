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
	enum State { Stopped, Playing, Paused, };

	struct Properties
	{
		uint maxCount = 10000;
		uint startingCount = 10000;

		bool autoPlay;
	};
	Properties properties;

	ParticleSystem(Properties propertiesInit = Properties()) noexcept;

	~ParticleSystem() noexcept;

	void Start() noexcept;
	void Stop() noexcept;

	// Bind ShaderProgram and Uniforms First
	void Dispatch() const noexcept;

	const uint GetPositionBuffer() const noexcept;
	const uint GetVelocityBuffer() const noexcept;
	const uint GetColourBuffer() const noexcept;

private:
	State state = Stopped;

	// vec4 buffer: XYZ position + W for active status
	uint positionBuffer;
	// vec4 buffer: XYZ velocity + W unused
	uint velocityBuffer;
	// vec4 buffer: RGBA colour
	uint colourBuffer;
};