#pragma once

#include "GLIncludes.h"

#include <memory>

enum ShaderStage
{
	UndefinedStage,

	VertexStage,
	TessEvaluationStage,
	TessControlStage,
	GeometryStage,
	FragmentStage
};
const int ShaderStagesCount = 6;

class Shader
{
public:
	Shader() : stage(UndefinedStage), glHandle(0), lastError(nullptr) {}
	Shader(ShaderStage stageInit, const char* filename) : stage(UndefinedStage), glHandle(0), lastError(nullptr) { LoadShader(stageInit, filename); }
	~Shader();

	bool LoadShader(ShaderStage stageInit, const char* filename);
	bool CreateShader(ShaderStage stageInit, const char* string);

	ShaderStage GetStage() const { return stage; }
	unsigned int GetGLHandle() const { return glHandle; }

	const char* GetLastError() const { return lastError; }

protected:

	ShaderStage	stage;
	unsigned int glHandle;
	char* lastError;
};



class ShaderProgram
{
public:

	ShaderProgram() : program(0), lastError(nullptr)
	{
		m_shaders[0] = m_shaders[1] = m_shaders[2] = m_shaders[3] = m_shaders[4] = 0;
	}
	~ShaderProgram();

	bool LoadAndLinkFromJSON(const char* filename);

	bool LoadShader(ShaderStage stage, const char* filename);
	bool CreateShader(ShaderStage stage, const char* string);
	void AttachShader(const std::shared_ptr<Shader>& shader);
	void ClearShader(ShaderStage stage);

	bool Link();

	const char* GetLastError() const { return lastError; }

	void Bind();

	unsigned int GetGLHandle() const { return program; }

	int GetUniform(const char* name);

	void BindUniform(int ID, int value);
	void BindUniform(int ID, float value);
	void BindUniform(int ID, const vec2& value);
	void BindUniform(int ID, const vec3& value);
	void BindUniform(int ID, const vec4& value);
	void BindUniform(int ID, const mat2& value);
	void BindUniform(int ID, const mat3& value);
	void BindUniform(int ID, const mat4& value);
	void BindUniform(int ID, int count, int* value);
	void BindUniform(int ID, int count, float* value);
	void BindUniform(int ID, int count, const vec2* value);
	void BindUniform(int ID, int count, const vec3* value);
	void BindUniform(int ID, int count, const vec4* value);
	void BindUniform(int ID, int count, const mat2* value);
	void BindUniform(int ID, int count, const mat3* value);
	void BindUniform(int ID, int count, const mat4* value);

	// Find a better way of doing this. Name searching may be expensive
	bool BindUniform(const char* name, int value);
	bool BindUniform(const char* name, float value);
	bool BindUniform(const char* name, const vec2& value);
	bool BindUniform(const char* name, const vec3& value);
	bool BindUniform(const char* name, const vec4& value);
	bool BindUniform(const char* name, const mat2& value);
	bool BindUniform(const char* name, const mat3& value);
	bool BindUniform(const char* name, const mat4& value);
	bool BindUniform(const char* name, int count, int* value);
	bool BindUniform(const char* name, int count, float* value);
	bool BindUniform(const char* name, int count, const vec2* value);
	bool BindUniform(const char* name, int count, const vec3* value);
	bool BindUniform(const char* name, int count, const vec4* value);
	bool BindUniform(const char* name, int count, const mat2* value);
	bool BindUniform(const char* name, int count, const mat3* value);
	bool BindUniform(const char* name, int count, const mat4* value);

private:
	unsigned int program;
	std::shared_ptr<Shader> m_shaders[ShaderStagesCount];
	char* lastError;

	bool LoadShaderFromJSON(ShaderStage stage, const char* filename);
	bool JSONFileIsValid(const char* filename);
};