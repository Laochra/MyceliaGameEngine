#pragma once

#include "GLIncludes.h"

#include "Colour.h"

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
	enum class Shape : uint { Sphere, Cone, Box, Quad, Circle };
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
		float colourRange[10] = { 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
										  1.0f, 0.0f, 1.0f, 1.0f, 1.0f };
		HDRColour& ColourA() { return *(HDRColour*)&colourRange[0]; }
		HDRColour& ColourB() { return *(HDRColour*)&colourRange[5]; }

		// Emission Settings
		bool emitOverTime;
		uint particlesPerSecond;
		Shape shape;
		union { float shapeData0, innerRadius, width;  };
		union { float shapeData1, outerRadius, height; };
		union { float shapeData2, coneAngle,	depth;  };

		// Rendering Settings
		uint spriteFilepathLength = 13;
		char spriteFilepath[256] = "DefaultColour";
		float alphaClippingThreshold = 0.5f;
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
	const uint GetSizeBuffer() const noexcept;

private:
	State state = Stopped;
	float elapsedTime = 0.0f;

	// vec4 buffer: XYZ position + W for active status
	uint positionBuffer;
	// vec4 buffer: XYZ velocity + W for lifetime
	uint velocityBuffer;
	// vec4 buffer: RGBA colour
	uint colourBuffer;
	// vec2 buffer: Size (Width, Height)
	uint sizeBuffer;
};