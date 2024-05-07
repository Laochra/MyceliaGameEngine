#include "ParticleEmitter.h"

#include "Debug.h";

void ParticleEmitter::Draw()
{

	debug->Log({ "Attempting to draw particles"});
	

}
void ParticleEmitter::Update()
{
	computeShader->Bind();
	computeShader->BindUniform("Delta", Time::delta);
	particleSystem->Dispatch();
}

void ParticleEmitter::Initialise()
{
	GameObject::Initialise();
	Updater::DrawAdd(this);
}
void ParticleEmitter::OnDestroy()
{
	del(computeShader);
	del(particleSystem);

	GameObject3D::OnDestroy();
}

ParticleEmitter& ParticleEmitter::SetShaderProgram(ShaderProgram* shaderProgramInit)
{
	del(shaderProgram);
	shaderProgram = shaderProgramInit;
	return *this;
}

ParticleEmitter& ParticleEmitter::SetParticleSystem(ParticleSystem* particleSystemInit)
{
	del(particleSystem);
	particleSystem = particleSystemInit;
	return *this;
}
ParticleEmitter& ParticleEmitter::SetComputeShader(ComputeShader* computeShaderInit)
{
	del(computeShader);
	computeShader = computeShaderInit;
	return *this;
}
