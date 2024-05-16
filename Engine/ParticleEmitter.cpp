#include "ParticleEmitter.h"

#include "Camera.h"

#include "Debug.h";

void ParticleEmitter::Draw()
{	
	if (*this != Active) return;
	if (GetEmissionState() == ParticleSystem::WaitingToStart) return;
	if (GetEmissionState() == ParticleSystem::Stopped) return;

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
	shaderProgram->BindUniform("ProjectionViewModel", mainCamera->GetPVMatrix() * GetMatrix());

	// Draw the Particles!
	glDrawArrays(GL_POINTS, 0, particleSystem->properties.maxCount);

	// Unbind Buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void ParticleEmitter::DrawDebug()
{
	typedef ParticleSystem::Shape Shape;
	ParticleSystem::Properties* properties = &particleSystem->properties;

	switch (particleSystem->properties.shape)
	{
	case Shape::Sphere:
	{
		debug->lines.AddSphere(GetGlobalPosition(), properties->shapeData[0], 32);
		debug->lines.AddSphere(GetGlobalPosition(), properties->shapeData[1], 32);
		break;
	}
	case Shape::Cone:
	{
		vec3 direction = (vec3)glm::normalize(glm::inverse(GetMatrix())[2]);

		debug->lines.AddConicalFrustum(
			GetGlobalPosition(),
			direction,
			properties->shapeData[0],
			properties->shapeData[1],
			properties->shapeData[2],
			32
		);
		break;
	}
	case Shape::Box:
	{
		debug->lines.AddCuboid(GetGlobalPosition(), as(vec3)properties->shapeData);
		break;
	}
	case Shape::Quad:
	{
		GameObject3D::DrawDebug();
		break;
	}
	case Shape::Circle:
	{
		GameObject3D::DrawDebug();
		break;
	}
	}
}
void ParticleEmitter::Update()
{
	if (*this != Active) return;
	switch (GetEmissionState())
	{
	case ParticleSystem::Paused:  return;
	case ParticleSystem::Stopped: return;
	case ParticleSystem::WaitingToStart:
	{
		if (particleSystem->elapsedTime > particleSystem->properties.delay)
		{
			SetEmissionState(ParticleSystem::Playing);
		}
		else
		{
			particleSystem->elapsedTime += Time::delta;
		}
		return;
	}
	case ParticleSystem::Playing:
	{
		if (particleSystem->elapsedTime > particleSystem->properties.duration + particleSystem->properties.delay)
		{
			SetEmissionState(ParticleSystem::Stopped);
			if (particleSystem->properties.loop)
			{
				if (particleSystem->properties.delay > 0.0f)
				{
					SetEmissionState(ParticleSystem::WaitingToStart);
				}
				else
				{
					SetEmissionState(ParticleSystem::Playing);
				}
			}
			return;
		}
		particleSystem->elapsedTime += Time::delta;
		break;
	}
	}

	computeShader->Bind();
	computeShader->BindUniform("Delta", Time::delta);
	computeShader->BindUniform("Gravity", particleSystem->properties.gravity);
	computeShader->BindUniform("UpVector", (vec3)glm::normalize(glm::inverse(GetMatrix())[1]));
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

const ShaderProgram& ParticleEmitter::GetShaderProgram() const noexcept
{
	return *shaderProgram;
}
ParticleSystem* ParticleEmitter::GetParticleSystem() noexcept
{
	return particleSystem;
}
const ComputeShader& ParticleEmitter::GetComputeShader() const noexcept
{
	return *computeShader;
}

ParticleEmitter& ParticleEmitter::SetShaderProgram(ShaderProgram* shaderProgramInit) noexcept
{
	del(shaderProgram);
	shaderProgram = shaderProgramInit;
	return *this;
}
ParticleEmitter& ParticleEmitter::SetParticleSystem(ParticleSystem* particleSystemInit) noexcept
{
	del(particleSystem);
	particleSystem = particleSystemInit;
	return *this;
}
ParticleEmitter& ParticleEmitter::SetComputeShader(ComputeShader* computeShaderInit) noexcept
{
	del(computeShader);
	computeShader = computeShaderInit;
	return *this;
}

const ParticleSystem::State ParticleEmitter::GetEmissionState() const noexcept
{
	return particleSystem->state;
}

ParticleEmitter& ParticleEmitter::SetEmissionState(ParticleSystem::State newState) noexcept
{
	if (particleSystem->state == ParticleSystem::Stopped && newState == ParticleSystem::Playing)
	{
		particleSystem->Start();
	}
	else if (particleSystem->state == ParticleSystem::WaitingToStart && newState == ParticleSystem::Playing)
	{
		particleSystem->Start();
	}
	else if (particleSystem->state == ParticleSystem::Playing && newState == ParticleSystem::Stopped)
	{
		particleSystem->Stop();
	}

	particleSystem->state = newState;

	return *this;
}
