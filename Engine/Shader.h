#pragma once

#include "GLIncludes.h"

#include <memory>

#include <string>
using std::string;

#include <vector>
using std::vector;

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

enum ShaderInputType : unsigned int
{
	UndefinedTypeGL = 0,

	TextureGL = GL_SAMPLER_2D,
	CubemapGL = GL_SAMPLER_CUBE,

	FloatGL = GL_FLOAT,
	Float2GL = GL_FLOAT_VEC2,
	Float3GL = GL_FLOAT_VEC3,
	Float4GL = GL_FLOAT_VEC4,
	Float2x2GL = GL_FLOAT_MAT2,
	Float2x3GL = GL_FLOAT_MAT2x3,
	Float2x4GL = GL_FLOAT_MAT2x4,
	Float3x2GL = GL_FLOAT_MAT3x2, 
	Float3x3GL = GL_FLOAT_MAT3,
	Float3x4GL = GL_FLOAT_MAT3x4,
	Float4x2GL = GL_FLOAT_MAT4x2,
	Float4x3GL = GL_FLOAT_MAT4x3,
	Float4x4GL = GL_FLOAT_MAT4,

	DoubleGL = GL_DOUBLE,
	Double2GL = GL_DOUBLE_VEC2,
	Double3GL = GL_DOUBLE_VEC3,
	Double4GL = GL_DOUBLE_VEC4,
	Double2x2GL = GL_DOUBLE_MAT2,
	Double2x3GL = GL_DOUBLE_MAT2x3,
	Double2x4GL = GL_DOUBLE_MAT2x4,
	Double3x2GL = GL_DOUBLE_MAT3x2,
	Double3x3GL = GL_DOUBLE_MAT3,
	Double3x4GL = GL_DOUBLE_MAT3x4,
	Double4x2GL = GL_DOUBLE_MAT4x2,
	Double4x3GL = GL_DOUBLE_MAT4x3,
	Double4x4GL = GL_DOUBLE_MAT4,

	IntGL = GL_INT,
	Int2GL = GL_INT_VEC2,
	Int3GL = GL_INT_VEC3,
	Int4GL = GL_INT_VEC4,
	UIntGL = GL_UNSIGNED_INT,
	UInt2GL = GL_UNSIGNED_INT_VEC2,
	UInt3GL = GL_UNSIGNED_INT_VEC3,
	UInt4GL = GL_UNSIGNED_INT_VEC4,

	BoolGL = GL_BOOL,
	BoolGL2 = GL_BOOL_VEC2,
	BoolGL3 = GL_BOOL_VEC3,
	BoolGL4 = GL_BOOL_VEC4,
};
const char* GetShaderInputTypeName(ShaderInputType type);
ShaderInputType GetShaderInputTypeFromName(const char* typeName);
ShaderInputType GetShaderInputTypeFromName(string typeName);

struct ShaderInput
{
	string name = "";
	ShaderInputType type = UndefinedTypeGL;
	bool exposed = false;
};



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

	const char* const GetFilepath() const noexcept;
	unsigned int GetGLHandle() const { return program; }

	void GetFields(vector<ShaderInput>& attributes, vector<ShaderInput>& uniforms);

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
	bool BindUniform(const char* name, const glm::uvec2& value);
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
	char* filepath;

	bool LoadShaderFromJSON(ShaderStage stage, const char* filename);
	bool JSONFileIsValid(const char* filename);
};