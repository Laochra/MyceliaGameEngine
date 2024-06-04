#include "ParticleEmitter.h"

#include "Camera.h"

#include "Debug.h";

void ParticleEmitter::SerialiseTo(json& jsonObj) const
{
	GameObject3D::SerialiseTo(jsonObj);

	// Particle Properties
	{
		json properties;

		properties["Autoplay"] = particleSystem->properties.autoplay;
		properties["Loop"] = particleSystem->properties.loop;
		properties["Duration"] = particleSystem->properties.duration;
		properties["Delay"] = particleSystem->properties.delay;
		properties["MaxCount"] = particleSystem->properties.maxCount;
		properties["Gravity"] = particleSystem->properties.gravity;

		float* sizeRange = particleSystem->properties.sizeRange;
		properties["SizeRange"] = vector{ sizeRange[0], sizeRange[1] };
		float* lifetimeRange = particleSystem->properties.lifetimeRange;
		properties["LifetimeRange"] = vector{ lifetimeRange[0], lifetimeRange[1] };
		float* speedRange = particleSystem->properties.speedRange;
		properties["SpeedRange"] = vector{ speedRange[0], speedRange[1] };
		float* colour = particleSystem->properties.colour;
		properties["Colour"] = vector{ colour[0], colour[1], colour[2], colour[3] };

		properties["EmitOverTime"] = particleSystem->properties.emitOverTime;
		properties["ParticlesPerSecond"] = particleSystem->properties.particlesPerSecond;
		properties["ShapeID"] = (uint)particleSystem->properties.shape;
		float* shapeData = particleSystem->properties.shapeData;
		properties["ShapeData"] = vector{ shapeData[0], shapeData[1], shapeData[2] };

		jsonObj["ParticleProperties"] = properties;
	}

	jsonObj["ComputeShader"] = computeShader->filepath;
	
	jsonObj["ShaderProgram"] = shaderProgram->GetFilepath();
}
void ParticleEmitter::DeserialiseFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	GameObject3D::DeserialiseFrom(jsonObj, guidOptions);

	// Particle Properties
	{
		json properties = jsonObj["ParticleProperties"];
		del(particleSystem);
		particleSystem = new ParticleSystem();
		particleSystem->properties.autoplay = properties["Autoplay"];
		particleSystem->properties.loop = properties["Loop"];
		particleSystem->properties.duration = properties["Duration"];
		particleSystem->properties.delay = properties["Delay"];
		particleSystem->properties.maxCount = properties["MaxCount"];
		particleSystem->properties.gravity = properties["Gravity"];

		vector<float> sizeRange = properties["SizeRange"];
		memcpy(particleSystem->properties.sizeRange, sizeRange.data(), sizeRange.size());
		vector<float> lifetimeRange = properties["LifetimeRange"];
		memcpy(particleSystem->properties.lifetimeRange, lifetimeRange.data(), lifetimeRange.size());
		vector<float> speedRange = properties["SpeedRange"];
		memcpy(particleSystem->properties.speedRange, speedRange.data(), speedRange.size());
		vector<float> colour = properties["SizeRange"];
		memcpy(particleSystem->properties.colour, colour.data(), colour.size());

		particleSystem->properties.emitOverTime = properties["EmitOverTime"];
		particleSystem->properties.particlesPerSecond = properties["ParticlesPerSecond"];
		particleSystem->properties.shape = (ParticleSystem::Shape)(uint)properties["ShapeID"];
		vector<float> shapeData = properties["ShapeData"];
		memcpy(particleSystem->properties.shapeData, shapeData.data(), shapeData.size());
	}

	string computeShaderFilepath = jsonObj["ComputeShader"];
	del(computeShader);
	computeShader = new ComputeShader(computeShaderFilepath.c_str());
	computeShader->filepath = new char[computeShaderFilepath.size() + 1];
	memcpy(computeShader->filepath, computeShaderFilepath.c_str(), computeShaderFilepath.size() + 1);

	string shaderProgramFilepath = jsonObj["ShaderProgram"];
	del(shaderProgram);
	shaderProgram = new ShaderProgram();
	shaderProgram->LoadAndLinkFromJSON(shaderProgramFilepath.c_str());
}

void ParticleEmitter::Draw()
{	
	if (this != Active) return;
	if (GetEmissionState() == ParticleSystem::WaitingToStart) return;
	if (GetEmissionState() == ParticleSystem::Stopped) return;
	glDisable(GL_CULL_FACE);
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
	shaderProgram->BindUniform("CameraPosition", mainCamera->GetGlobalPosition());
	shaderProgram->BindUniform("CameraUp", (vec3)glm::inverse(mainCamera->GetMatrix())[1]);

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
	if (this != Active) return;
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
	if (shaderProgram == nullptr) shaderProgram = new ShaderProgram();
	shaderProgram->LoadAndLinkFromJSON("Assets\\Shaders\\ParticleSystem.gpu");
	if (computeShader == nullptr) computeShader = new ComputeShader("Assets\\Shaders\\Particles.comp");
	if (particleSystem == nullptr) particleSystem = new ParticleSystem();

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
