#pragma once

#include "GameObject3D.h"
#include "ParticleSystem.h"
#include "ComputeShader.h"

class ParticleEmitter : public GameObject3D
{
	SerialiseAs(ParticleEmitter);

	using GameObject3D::GameObject3D;

public:
	
	virtual void Draw() override;
	virtual void DrawDebug() override;
	virtual void Update() override;

	virtual void Initialise() override;
	virtual void OnDestroy() override;
	
	const ShaderProgram& GetShaderProgram() const noexcept;
	ParticleSystem* GetParticleSystem() noexcept;
	const ComputeShader& GetComputeShader() const noexcept;

	ParticleEmitter& SetShaderProgram(ShaderProgram* shaderProgramInit) noexcept;
	ParticleEmitter& SetParticleSystem(ParticleSystem* particleSystemInit) noexcept;
	ParticleEmitter& SetComputeShader(ComputeShader* computeShaderInit) noexcept;

	const ParticleSystem::State GetEmissionState() const noexcept;
	ParticleEmitter& SetEmissionState(ParticleSystem::State newState) noexcept;

private:
	// Simulation
	ParticleSystem* particleSystem;
	ComputeShader* computeShader;
	// Rendering
	ShaderProgram* shaderProgram;
	uint vao;
};
