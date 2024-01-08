#include "Shader.h"
#include <cstdio>
#include <cassert>

#include "json.hpp"
using json = nlohmann::json;

#include <string>
using std::string;

#include <fstream>
using std::ifstream;

#include <iostream>

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
	case VertexStage:						glHandle = glCreateShader(GL_VERTEX_SHADER);				break;
	case TessEvaluationStage:			glHandle = glCreateShader(GL_TESS_EVALUATION_SHADER);	break;
	case TessControlStage:				glHandle = glCreateShader(GL_TESS_CONTROL_SHADER);		break;
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
	case VertexStage:						glHandle = glCreateShader(GL_VERTEX_SHADER);				break;
	case TessEvaluationStage:			glHandle = glCreateShader(GL_TESS_EVALUATION_SHADER);	break;
	case TessControlStage:				glHandle = glCreateShader(GL_TESS_CONTROL_SHADER);		break;
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
	if (program != 0) glDeleteProgram(program);
}

bool ShaderProgram::LoadAndLinkFromJSON(const char* filename)
{
	if (!JSONFileIsValid(filename)) return false;

	ifstream input(filename);
	json shaderProgram;
	input >> shaderProgram;

	string vertexFile = shaderProgram["Vertex"];
	if (LoadShaderFromJSON(VertexStage, vertexFile.c_str()) == false) return false;

	string tessellationEvaluationFile = shaderProgram["TessEvaluation"];
	if (LoadShaderFromJSON(TessEvaluationStage, tessellationEvaluationFile.c_str()) == false) return false;

	string tessellationControlFile = shaderProgram["TessControl"];
	if (LoadShaderFromJSON(TessControlStage, tessellationControlFile.c_str()) == false) return false;

	string geometryFile = shaderProgram["Geometry"];
	if (LoadShaderFromJSON(GeometryStage, geometryFile.c_str()) == false) return false;

	string fragmentFile = shaderProgram["Fragment"];
	if (LoadShaderFromJSON(FragmentStage, fragmentFile.c_str()) == false) return false;

	if (program != 0) glDeleteProgram(program);
	return Link();
}

bool ShaderProgram::LoadShader(ShaderStage stage, const char* filename)
{
	assert(stage > 0 && stage < ShaderStagesCount);
	m_shaders[stage] = std::make_shared<Shader>();
	return m_shaders[stage]->LoadShader(stage, filename);
}

bool ShaderProgram::CreateShader(ShaderStage stage, const char* string)
{
	assert(stage > 0 && stage < ShaderStagesCount);
	m_shaders[stage] = std::make_shared<Shader>();
	return m_shaders[stage]->CreateShader(stage, string);
}

void ShaderProgram::AttachShader(const std::shared_ptr<Shader>& shader)
{
	assert(shader != nullptr);
	m_shaders[shader->GetStage()] = shader;
}

void ShaderProgram::ClearShader(ShaderStage stage)
{
	if (m_shaders[stage] != 0)
	{
		m_shaders[stage].reset();
	}
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


bool ShaderProgram::LoadShaderFromJSON(ShaderStage stage, const char* filename)
{
	if (string(filename) == string("None"))
	{
		ClearShader(stage);
		return true;
	}

	if (LoadShader(stage, filename) == false)
	{
		std::cout << "\n" << filename << " is not a valid shader file\n";
		return false;
	}


	return true;
}

bool ShaderProgram::JSONFileIsValid(const char* filename)
{
	ifstream input(filename);
	if (!input.good())
	{
		std::cout << "\nInput file " << filename
			<< " is missing, it may have been moved, deleted, or renamed\n";
		return false;
	}

	json shaderProgram;
	input >> shaderProgram;

	bool incompleteFile = false;
	if (!shaderProgram.contains("Vertex"))
	{
		std::cout << "\n" << filename << " does not specify a Vertex shader, specify \"None\" to disregard it\n";
		incompleteFile = true;
	}
	if (!shaderProgram.contains("TessEvaluation"))
	{
		std::cout << "\n" << filename << " does not specify a Tessellation Evaluation shader, specify \"None\" to disregard it\n";
		incompleteFile = true;
	}
	if (!shaderProgram.contains("TessControl"))
	{
		std::cout << "\n" << filename << " does not specify a Tessellation Control shader, specify \"None\" to disregard it\n";
		incompleteFile = true;
	}
	if (!shaderProgram.contains("Geometry"))
	{
		std::cout << "\n" << filename << " does not specify a Geometry shader, specify \"None\" to disregard it\n";
		incompleteFile = true;
	}
	if (!shaderProgram.contains("Fragment"))
	{
		std::cout << "\n" << filename << " does not specify a Fragment shader, specify \"None\" to disregard it\n";
		incompleteFile = true;
	}
	if (incompleteFile) return false;

	return true;
}
