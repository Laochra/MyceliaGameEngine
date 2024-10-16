#pragma once

#include "GameObject3D.h"
#include "ParticleSystem.h"
#include "ComputeShader.h"

class ParticleEmitter : public GameObject3D
{
public:
	SerialiseAs(ParticleEmitter);

	using GameObject3D::GameObject3D;
	
	virtual void Draw(intptr_t lastUsedMaterial) override;
	virtual void DrawDebug() override;
	virtual void Update() override;

	virtual void Initialise() override;
	virtual void OnDestroy() override;
	virtual void OnRestore() override;
	
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
