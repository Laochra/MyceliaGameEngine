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
	virtual void Update() override;

	virtual void Initialise() override;
	virtual void OnDestroy() override;
	
	ParticleEmitter& SetShaderProgram(ShaderProgram* shaderProgramInit);
	ParticleEmitter& SetParticleSystem(ParticleSystem* particleSystemInit);
	ParticleEmitter& SetComputeShader(ComputeShader* computeShaderInit);

private:
	ShaderProgram* shaderProgram;
	ParticleSystem* particleSystem;
	ComputeShader* computeShader;
};
