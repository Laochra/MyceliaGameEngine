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
	enum State { Stopped, Playing, Paused, WaitingToStart };
	enum class Shape { Sphere, Cone, Box, Quad, Circle };
	struct Properties
	{
		// System Settings
		bool autoplay;
		bool loop;
		float duration = 10.0f;
		float delay;
		uint maxCount = 100000;
		float gravity;

		// Particle Settings
		float sizeRange[2] = { 1.0f, 1.0f };
		float lifetimeRange[2] = { 0.1f, 1.0f };
		float speedRange[2] = { 1.0f, 1.0f };
		float colour[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		// Emission Settings
		bool emitOverTime;
		uint particlesPerSecond;
		Shape shape;
		float shapeData[3];

		// Sphere / Cone / Circle Shape Data
		float& InnerRadius() { return shapeData[0]; }
		float& OuterRadius() { return shapeData[1]; }
		float& ConeAngle() { return shapeData[2]; }
		// Box / Quad Shape Data
		float& Width() { return shapeData[0]; }
		float& Height() { return shapeData[1]; }
		float& Depth() { return shapeData[2]; }
	};
	Properties properties;

	ParticleSystem(Properties propertiesInit = Properties()) noexcept;

	~ParticleSystem() noexcept;

	void Start() noexcept;
	void Stop() noexcept;

	// Bind ShaderProgram and Uniforms First
	void Dispatch() const noexcept;

	const float GetElapsedTime() const noexcept;

	const uint GetPositionBuffer() const noexcept;
	const uint GetVelocityBuffer() const noexcept;
	const uint GetColourBuffer() const noexcept;

private:
	State state = Stopped;
	float elapsedTime = 0.0f;

	// vec4 buffer: XYZ position + W for active status
	uint positionBuffer;
	// vec4 buffer: XYZ velocity + W for lifetime
	uint velocityBuffer;
	// vec4 buffer: RGBA colour
	uint colourBuffer;
};