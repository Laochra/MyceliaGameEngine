#include "ParticleEmitter.h"

#include "Camera.h"
#include "TextureManager.h"

#include "Debug.h"

void ParticleEmitter::SerialiseTo(json& jsonObj) const
{
	GameObject3D::SerialiseTo(jsonObj);

	// Particle Properties
	{
		json properties;

		ParticleSystem::Properties& p = particleSystem->properties;

		properties["Autoplay"] = p.autoplay;
		properties["Loop"] = p.loop;
		properties["Duration"] = p.duration;
		properties["Delay"] = p.delay;
		properties["MaxCount"] = p.maxCount;
		properties["Gravity"] = p.gravity;

		properties["SizeRange"] = vector{ p.sizeRange[0], p.sizeRange[1] };
		properties["LifetimeRange"] = vector{ p.lifetimeRange[0], p.lifetimeRange[1] };
		properties["SpeedRange"] = vector{ p.speedRange[0], p.speedRange[1] };
		properties["ColourRange"] = vector{ p.colourRange[0], p.colourRange[1], p.colourRange[2], p.colourRange[3], p.colourRange[4],
														p.colourRange[5], p.colourRange[6], p.colourRange[7], p.colourRange[8], p.colourRange[9] };

		properties["EmitOverTime"] = p.emitOverTime;
		properties["ParticlesPerSecond"] = p.particlesPerSecond;
		properties["ShapeID"] = (uint)p.shape;
		properties["ShapeData"] = vector{ p.shapeData0, p.shapeData1, p.shapeData2 };

		properties["Sprite"] = string(p.spriteFilepath, p.spriteFilepathLength);
		properties["AlphaThreshold"] = p.alphaClippingThreshold;

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
		ParticleSystem::Properties& p = particleSystem->properties;

		p.autoplay = properties["Autoplay"];
		p.loop = properties["Loop"];
		p.duration = properties["Duration"];
		p.delay = properties["Delay"];
		p.maxCount = properties["MaxCount"];
		p.gravity = properties["Gravity"];

		vector<float> sizeRange = properties["SizeRange"];
		memcpy(p.sizeRange, sizeRange.data(), sizeRange.size() * sizeof(float));
		vector<float> lifetimeRange = properties["LifetimeRange"];
		memcpy(p.lifetimeRange, lifetimeRange.data(), lifetimeRange.size() * sizeof(float));
		vector<float> speedRange = properties["SpeedRange"];
		memcpy(p.speedRange, speedRange.data(), speedRange.size() * sizeof(float));
		vector<float> colour = properties["ColourRange"];
		memcpy(p.colourRange, colour.data(), colour.size() * sizeof(float));

		p.emitOverTime = properties["EmitOverTime"];
		p.particlesPerSecond = properties["ParticlesPerSecond"];
		p.shape = (ParticleSystem::Shape)(uint)properties["ShapeID"];
		vector<float> shapeData = properties["ShapeData"];
		memcpy(&p.shapeData0, shapeData.data(), shapeData.size() * sizeof(float));

		string spriteFilepath = properties["Sprite"];
		p.spriteFilepathLength = (uint)spriteFilepath.size();
		memcpy(p.spriteFilepath, spriteFilepath.c_str(), spriteFilepath.size() + 1);
		p.alphaClippingThreshold = properties["AlphaThreshold"];
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
void ParticleEmitter::UpdateFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	GameObject3D::UpdateFrom(jsonObj, guidOptions);

	// Particle Properties
	{
		json properties = jsonObj["ParticleProperties"];

		del(particleSystem);
		particleSystem = new ParticleSystem();
		ParticleSystem::Properties& p = particleSystem->properties;

		p.autoplay = properties["Autoplay"];
		p.loop = properties["Loop"];
		p.duration = properties["Duration"];
		p.delay = properties["Delay"];
		p.maxCount = properties["MaxCount"];
		p.gravity = properties["Gravity"];

		vector<float> sizeRange = properties["SizeRange"];
		memcpy(p.sizeRange, sizeRange.data(), sizeRange.size() * sizeof(float));
		vector<float> lifetimeRange = properties["LifetimeRange"];
		memcpy(p.lifetimeRange, lifetimeRange.data(), lifetimeRange.size() * sizeof(float));
		vector<float> speedRange = properties["SpeedRange"];
		memcpy(p.speedRange, speedRange.data(), speedRange.size() * sizeof(float));
		vector<float> colour = properties["ColourRange"];
		memcpy(p.colourRange, colour.data(), colour.size() * sizeof(float));

		p.emitOverTime = properties["EmitOverTime"];
		p.particlesPerSecond = properties["ParticlesPerSecond"];
		p.shape = (ParticleSystem::Shape)(uint)properties["ShapeID"];
		vector<float> shapeData = properties["ShapeData"];
		memcpy(&p.shapeData0, shapeData.data(), shapeData.size() * sizeof(float));

		string spriteFilepath = properties["Sprite"];
		p.spriteFilepathLength = (uint)spriteFilepath.size();
		memcpy(p.spriteFilepath, spriteFilepath.c_str(), spriteFilepath.size() + 1);
		p.alphaClippingThreshold = properties["AlphaThreshold"];
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

	glBindBuffer(GL_ARRAY_BUFFER, particleSystem->sizeBuffer);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);

	// Bind Program and View Projection Matrix
	shaderProgram->Bind();
	shaderProgram->BindUniform("Projection", AppInfo::ActiveCamera()->GetProjectionMatrix());
	shaderProgram->BindUniform("ModelView", AppInfo::ActiveCamera()->GetViewMatrix() * GetMatrix());
	textureManager->GetTexture(particleSystem->properties.spriteFilepath, Texture::NonLinear)->Bind(0);
	shaderProgram->BindUniform("Sprite", 0);
	shaderProgram->BindUniform("AlphaCutoff", particleSystem->properties.alphaClippingThreshold);


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
		AppInfo::debug->lines.AddSphere(GetGlobalPosition(), properties->innerRadius, 32);
		AppInfo::debug->lines.AddSphere(GetGlobalPosition(), properties->outerRadius, 32);
		break;
	}
	case Shape::Cone:
	{
		vec3 direction = (vec3)glm::normalize(glm::inverse(GetMatrix())[2]);

		AppInfo::debug->lines.AddConicalFrustum(
			GetGlobalPosition(),
			direction,
			properties->innerRadius,
			properties->outerRadius,
			properties->coneAngle,
			32
		);
		break;
	}
	case Shape::Box:
	{
		AppInfo::debug->lines.AddCuboid(GetGlobalPosition(), vec3(properties->width, properties->height, properties->depth));
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

	Updater::DrawRemove(this);
}
void ParticleEmitter::OnRestore()
{
	GameObject3D::OnRestore();

	glGenVertexArrays(1, &vao);
	glPointSize(5.0f);
	if (shaderProgram == nullptr) shaderProgram = new ShaderProgram();
	shaderProgram->LoadAndLinkFromJSON("Assets\\Shaders\\ParticleSystem.gpu");
	if (computeShader == nullptr) computeShader = new ComputeShader("Assets\\Shaders\\Particles.comp");
	if (particleSystem == nullptr) particleSystem = new ParticleSystem();

	Updater::DrawAdd(this);
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
