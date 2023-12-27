#include "Shader.h"
#include <cstdio>
#include <cassert>

Shader::~Shader()
{
	glDeleteShader(glHandle);
}

bool Shader::LoadShader(ShaderStage stage, const char* filename)
{
	assert(stage > 0 && stage < ShaderStagesCount);

	stage = stage;

	switch (stage)
	{
	case VertexStage:					glHandle = glCreateShader(GL_VERTEX_SHADER);			break;
	case TessellationEvaluationStage:	glHandle = glCreateShader(GL_TESS_EVALUATION_SHADER);	break;
	case TessellationControlStage:		glHandle = glCreateShader(GL_TESS_CONTROL_SHADER);		break;
	case GeometryStage:					glHandle = glCreateShader(GL_GEOMETRY_SHADER);			break;
	case FragmentStage:					glHandle = glCreateShader(GL_FRAGMENT_SHADER);			break;
	default: break;
	};
	
	// open file
	FILE* file = nullptr;
	fopen_s(&file, filename, "rb");
	fseek(file, 0, SEEK_END);
	unsigned int size = ftell(file);
	char* source = new char[size + 1];
	fseek(file, 0, SEEK_SET);
	fread_s(source, size + 1, sizeof(char), size, file);
	fclose(file);
	source[size] = 0;

	glShaderSource(glHandle, 1, (const char**)&source, 0);
	glCompileShader(glHandle);

	delete[] source;

	int success = GL_TRUE;
	glGetShaderiv(glHandle, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetShaderiv(glHandle, GL_INFO_LOG_LENGTH, &infoLogLength);

		delete[] lastError;
		lastError = new char[infoLogLength];
		glGetShaderInfoLog(glHandle, infoLogLength, 0, lastError);
		return false;
	}

	return true;
}

bool Shader::CreateShader(ShaderStage stageInit, const char* string)
{
	assert(stage > 0 && stage < ShaderStagesCount);

	stage = stageInit;

	switch (stage)
	{
	case VertexStage:					glHandle = glCreateShader(GL_VERTEX_SHADER);			break;
	case TessellationEvaluationStage:	glHandle = glCreateShader(GL_TESS_EVALUATION_SHADER);	break;
	case TessellationControlStage:		glHandle = glCreateShader(GL_TESS_CONTROL_SHADER);		break;
	case GeometryStage:					glHandle = glCreateShader(GL_GEOMETRY_SHADER);			break;
	case FragmentStage:					glHandle = glCreateShader(GL_FRAGMENT_SHADER);			break;
	default: break;
	};

	glShaderSource(glHandle, 1, (const char**)&string, 0);
	glCompileShader(glHandle);
	
	int success = GL_TRUE;
	glGetShaderiv(glHandle, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetShaderiv(glHandle, GL_INFO_LOG_LENGTH, &infoLogLength);

		delete[] lastError;
		lastError = new char[infoLogLength];
		glGetShaderInfoLog(glHandle, infoLogLength, 0, lastError);
		return false;
	}

	return true;
}

ShaderProgram::~ShaderProgram()
{
	delete[] lastError;
	glDeleteProgram(program);
}

bool ShaderProgram::LoadShader(ShaderStage stageInit, const char* filename)
{
	assert(stageInit > 0 && stageInit < ShaderStagesCount);
	m_shaders[stageInit] = std::make_shared<Shader>();
	return m_shaders[stageInit]->LoadShader(stageInit, filename);
}

bool ShaderProgram::CreateShader(ShaderStage stageInit, const char* string)
{
	assert(stageInit > 0 && stageInit < ShaderStagesCount);
	m_shaders[stageInit] = std::make_shared<Shader>();
	return m_shaders[stageInit]->CreateShader(stageInit, string);
}

void ShaderProgram::AttachShader(const std::shared_ptr<Shader>& shader)
{
	assert(shader != nullptr);
	m_shaders[shader->GetStage()] = shader;
}

bool ShaderProgram::Link()
{
	program = glCreateProgram();
	for (auto& s : m_shaders)
		if (s != nullptr)
			glAttachShader(program, s->GetGLHandle());
	glLinkProgram(program);

	int success = GL_TRUE;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		delete[] lastError;
		lastError = new char[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, 0, lastError);
		return false;
	}
	return true;
}

void ShaderProgram::Bind()
{
	assert(program > 0 && "Invalid shader program");
	glUseProgram(program);
}

int ShaderProgram::GetUniform(const char* name)
{
	return glGetUniformLocation(program, name);
}

void ShaderProgram::BindUniform(int ID, int value)
{
	assert(program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniform1i(ID, value);
}

void ShaderProgram::BindUniform(int ID, float value)
{
	assert(program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniform1f(ID, value);
}

void ShaderProgram::BindUniform(int ID, const vec2& value)
{
	assert(program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniform2f(ID, value.x, value.y);
}

void ShaderProgram::BindUniform(int ID, const vec3& value)
{
	assert(program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniform3f(ID, value.x, value.y, value.z);
}

void ShaderProgram::BindUniform(int ID, const vec4& value)
{
	assert(program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniform4f(ID, value.x, value.y, value.z, value.w);
}

void ShaderProgram::BindUniform(int ID, const mat2& value)
{
	assert(program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniformMatrix2fv(ID, 1, GL_FALSE, &value[0][0]);
}

void ShaderProgram::BindUniform(int ID, const mat3& value)
{
	assert(program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniformMatrix3fv(ID, 1, GL_FALSE, &value[0][0]);
}

void ShaderProgram::BindUniform(int ID, const mat4& value)
{
	assert(program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniformMatrix4fv(ID, 1, GL_FALSE, &value[0][0]);
}

void ShaderProgram::BindUniform(int ID, int count, int* value)
{
	assert(program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniform1iv(ID, count, value);
}

void ShaderProgram::BindUniform(int ID, int count, float* value)
{
	assert(program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniform1fv(ID, count, value);
}

void ShaderProgram::BindUniform(int ID, int count, const vec2* value)
{
	assert(program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniform2fv(ID, count, (float*)value);
}

void ShaderProgram::BindUniform(int ID, int count, const vec3* value)
{
	assert(program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniform3fv(ID, count, (float*)value);
}

void ShaderProgram::BindUniform(int ID, int count, const vec4* value)
{
	assert(program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniform4fv(ID, count, (float*)value);
}

void ShaderProgram::BindUniform(int ID, int count, const mat2* value)
{
	assert(program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniformMatrix2fv(ID, count, GL_FALSE, (float*)value);
}

void ShaderProgram::BindUniform(int ID, int count, const mat3* value)
{
	assert(program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniformMatrix3fv(ID, count, GL_FALSE, (float*)value);
}

void ShaderProgram::BindUniform(int ID, int count, const mat4* value)
{
	assert(program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniformMatrix4fv(ID, count, GL_FALSE, (float*)value);
}

bool ShaderProgram::BindUniform(const char* name, int value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniform1i(i, value);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, float value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniform1f(i, value);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, const vec2& value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniform2f(i, value.x, value.y);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, const vec3& value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniform3f(i, value.x, value.y, value.z);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, const vec4& value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniform4f(i, value.x, value.y, value.z, value.w);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, const mat2& value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniformMatrix2fv(i, 1, GL_FALSE, &value[0][0]);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, const mat3& value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniformMatrix3fv(i, 1, GL_FALSE, &value[0][0]);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, const mat4& value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniformMatrix4fv(i, 1, GL_FALSE, &value[0][0]);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, int count, int* value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniform1iv(i, count, value);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, int count, float* value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniform1fv(i, count, value);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, int count, const vec2* value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniform2fv(i, count, (float*)value);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, int count, const vec3* value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniform3fv(i, count, (float*)value);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, int count, const vec4* value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniform4fv(i, count, (float*)value);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, int count, const mat2* value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniformMatrix2fv(i, count, GL_FALSE, (float*)value);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, int count, const mat3* value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniformMatrix3fv(i, count, GL_FALSE, (float*)value);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, int count, const mat4* value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniformMatrix4fv(i, count, GL_FALSE, (float*)value);
	return true;
}