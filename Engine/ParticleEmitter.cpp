#include "ParticleEmitter.h"

#include "Camera.h"

#include "Debug.h";

void ParticleEmitter::Draw()
{	
	// Bind Vertex Array Object
	glBindVertexArray(vao);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	// Upload Positions and Colours
	glBindBuffer(GL_ARRAY_BUFFER, particleSystem->positionBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), 0);

	glBindBuffer(GL_ARRAY_BUFFER, particleSystem->colourBuffer);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), 0);

	// Bind Program and View Projection Matrix
	shaderProgram->Bind();
	shaderProgram->BindUniform("PVMatrix", mainCamera->GetPVMatrix());

	// Draw the Particles!
	glDrawArrays(GL_POINTS, 0, particleSystem->maxParticles);

	// Unbind Buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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
	glGenVertexArrays(1, &vao);
	glPointSize(5.0f);
	Updater::DrawAdd(this);
}
void ParticleEmitter::OnDestroy()
{
	glDeleteVertexArrays(1, &vao);
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
