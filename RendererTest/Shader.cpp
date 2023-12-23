#include "Shader.h"
#include <cstdio>
#include <cassert>
#include "glad.h"
#include "glfw3.h"

Shader::~Shader() {
	glDeleteShader(m_handle);
}

bool Shader::loadShader(unsigned int stage, const char* filename) {
	assert(stage > 0 && stage < eShaderStage::SHADER_STAGE_Count);

	m_stage = stage;

	switch (stage) {
	case eShaderStage::VERTEX:	m_handle = glCreateShader(GL_VERTEX_SHADER);	break;
	case eShaderStage::TESSELLATION_EVALUATION:	m_handle = glCreateShader(GL_TESS_EVALUATION_SHADER);	break;
	case eShaderStage::TESSELLATION_CONTROL:	m_handle = glCreateShader(GL_TESS_CONTROL_SHADER);	break;
	case eShaderStage::GEOMETRY:	m_handle = glCreateShader(GL_GEOMETRY_SHADER);	break;
	case eShaderStage::FRAGMENT:	m_handle = glCreateShader(GL_FRAGMENT_SHADER);	break;
	default:	break;
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

	glShaderSource(m_handle, 1, (const char**)&source, 0);
	glCompileShader(m_handle);

	delete[] source;

	int success = GL_TRUE;
	glGetShaderiv(m_handle, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &infoLogLength);

		delete[] m_lastError;
		m_lastError = new char[infoLogLength];
		glGetShaderInfoLog(m_handle, infoLogLength, 0, m_lastError);
		return false;
	}

	return true;
}

bool Shader::createShader(unsigned int stage, const char* string) {
	assert(stage > 0 && stage < eShaderStage::SHADER_STAGE_Count);

	m_stage = stage;

	switch (stage) {
	case eShaderStage::VERTEX:	m_handle = glCreateShader(GL_VERTEX_SHADER);	break;
	case eShaderStage::TESSELLATION_EVALUATION:	m_handle = glCreateShader(GL_TESS_EVALUATION_SHADER);	break;
	case eShaderStage::TESSELLATION_CONTROL:	m_handle = glCreateShader(GL_TESS_CONTROL_SHADER);	break;
	case eShaderStage::GEOMETRY:	m_handle = glCreateShader(GL_GEOMETRY_SHADER);	break;
	case eShaderStage::FRAGMENT:	m_handle = glCreateShader(GL_FRAGMENT_SHADER);	break;
	default:	break;
	};

	glShaderSource(m_handle, 1, (const char**)&string, 0);
	glCompileShader(m_handle);
	
	int success = GL_TRUE;
	glGetShaderiv(m_handle, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &infoLogLength);

		delete[] m_lastError;
		m_lastError = new char[infoLogLength];
		glGetShaderInfoLog(m_handle, infoLogLength, 0, m_lastError);
		return false;
	}

	return true;
}

ShaderProgram::~ShaderProgram() {
	delete[] m_lastError;
	glDeleteProgram(m_program);
}

bool ShaderProgram::loadShader(unsigned int stage, const char* filename) {
	assert(stage > 0 && stage < eShaderStage::SHADER_STAGE_Count);
	m_shaders[stage] = std::make_shared<Shader>();
	return m_shaders[stage]->loadShader(stage, filename);
}

bool ShaderProgram::createShader(unsigned int stage, const char* string) {
	assert(stage > 0 && stage < eShaderStage::SHADER_STAGE_Count);
	m_shaders[stage] = std::make_shared<Shader>();
	return m_shaders[stage]->createShader(stage, string);
}

void ShaderProgram::attachShader(const std::shared_ptr<Shader>& shader) {
	assert(shader != nullptr);
	m_shaders[shader->getStage()] = shader;
}

bool ShaderProgram::link() {
	m_program = glCreateProgram();
	for (auto& s : m_shaders)
		if (s != nullptr)
			glAttachShader(m_program, s->getHandle());
	glLinkProgram(m_program);

	int success = GL_TRUE;
	glGetProgramiv(m_program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLogLength);

		delete[] m_lastError;
		m_lastError = new char[infoLogLength + 1];
		glGetProgramInfoLog(m_program, infoLogLength, 0, m_lastError);
		return false;
	}
	return true;
}

void ShaderProgram::bind() {
	assert(m_program > 0 && "Invalid shader program");
	glUseProgram(m_program);
}

int ShaderProgram::getUniform(const char* name) {
	return glGetUniformLocation(m_program, name);
}

bool ShaderProgram::bindUniform(const char* name, int value) {
	assert(m_program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(m_program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniform1i(i, value);
	return true;
}

bool ShaderProgram::bindUniform(const char* name, float value) {
	assert(m_program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(m_program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniform1f(i, value);
	return true;
}

bool ShaderProgram::bindUniform(const char* name, const glm::vec2& value) {
	assert(m_program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(m_program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniform2f(i, value.x, value.y);
	return true;
}

bool ShaderProgram::bindUniform(const char* name, const glm::vec3& value) {
	assert(m_program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(m_program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniform3f(i, value.x, value.y, value.z);
	return true;
}

bool ShaderProgram::bindUniform(const char* name, const glm::vec4& value) {
	assert(m_program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(m_program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniform4f(i, value.x, value.y, value.z, value.w);
	return true;
}

bool ShaderProgram::bindUniform(const char* name, const glm::mat2& value) {
	assert(m_program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(m_program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniformMatrix2fv(i, 1, GL_FALSE, &value[0][0]);
	return true;
}

bool ShaderProgram::bindUniform(const char* name, const glm::mat3& value) {
	assert(m_program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(m_program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniformMatrix3fv(i, 1, GL_FALSE, &value[0][0]);
	return true;
}

bool ShaderProgram::bindUniform(const char* name, const glm::mat4& value) {
	assert(m_program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(m_program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniformMatrix4fv(i, 1, GL_FALSE, &value[0][0]);
	return true;
}

bool ShaderProgram::bindUniform(const char* name, int count, int* value) {
	assert(m_program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(m_program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniform1iv(i, count, value);
	return true;
}

bool ShaderProgram::bindUniform(const char* name, int count, float* value) {
	assert(m_program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(m_program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniform1fv(i, count, value);
	return true;
}

bool ShaderProgram::bindUniform(const char* name, int count, const glm::vec2* value) {
	assert(m_program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(m_program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniform2fv(i, count, (float*)value);
	return true;
}

bool ShaderProgram::bindUniform(const char* name, int count, const glm::vec3* value) {
	assert(m_program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(m_program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniform3fv(i, count, (float*)value);
	return true;
}

bool ShaderProgram::bindUniform(const char* name, int count, const glm::vec4* value) {
	assert(m_program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(m_program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniform4fv(i, count, (float*)value);
	return true;
}

bool ShaderProgram::bindUniform(const char* name, int count, const glm::mat2* value) {
	assert(m_program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(m_program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniformMatrix2fv(i, count, GL_FALSE, (float*)value);
	return true;
}

bool ShaderProgram::bindUniform(const char* name, int count, const glm::mat3* value) {
	assert(m_program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(m_program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniformMatrix3fv(i, count, GL_FALSE, (float*)value);
	return true;
}

bool ShaderProgram::bindUniform(const char* name, int count, const glm::mat4* value) {
	assert(m_program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(m_program, name);
	if (i < 0) {
		printf("Shader uniform [%s] not found! Is it being used?\n", name);
		return false;
	}
	glUniformMatrix4fv(i, count, GL_FALSE, (float*)value);
	return true;
}

void ShaderProgram::bindUniform(int ID, int value) {
	assert(m_program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniform1i(ID, value);
}

void ShaderProgram::bindUniform(int ID, float value) {
	assert(m_program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniform1f(ID, value);
}

void ShaderProgram::bindUniform(int ID, const glm::vec2& value) {
	assert(m_program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniform2f(ID, value.x, value.y);
}

void ShaderProgram::bindUniform(int ID, const glm::vec3& value) {
	assert(m_program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniform3f(ID, value.x, value.y, value.z);
}

void ShaderProgram::bindUniform(int ID, const glm::vec4& value) {
	assert(m_program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniform4f(ID, value.x, value.y, value.z, value.w);
}

void ShaderProgram::bindUniform(int ID, const glm::mat2& value) {
	assert(m_program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniformMatrix2fv(ID, 1, GL_FALSE, &value[0][0]);
}

void ShaderProgram::bindUniform(int ID, const glm::mat3& value) {
	assert(m_program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniformMatrix3fv(ID, 1, GL_FALSE, &value[0][0]);
}

void ShaderProgram::bindUniform(int ID, const glm::mat4& value) {
	assert(m_program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniformMatrix4fv(ID, 1, GL_FALSE, &value[0][0]);
}

void ShaderProgram::bindUniform(int ID, int count, int* value) {
	assert(m_program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniform1iv(ID, count, value);
}

void ShaderProgram::bindUniform(int ID, int count, float* value) {
	assert(m_program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniform1fv(ID, count, value);
}

void ShaderProgram::bindUniform(int ID, int count, const glm::vec2* value) {
	assert(m_program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniform2fv(ID, count, (float*)value);
}

void ShaderProgram::bindUniform(int ID, int count, const glm::vec3* value) {
	assert(m_program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniform3fv(ID, count, (float*)value);
}

void ShaderProgram::bindUniform(int ID, int count, const glm::vec4* value) {
	assert(m_program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniform4fv(ID, count, (float*)value);
}

void ShaderProgram::bindUniform(int ID, int count, const glm::mat2* value) {
	assert(m_program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniformMatrix2fv(ID, count, GL_FALSE, (float*)value);
}

void ShaderProgram::bindUniform(int ID, int count, const glm::mat3* value) {
	assert(m_program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniformMatrix3fv(ID, count, GL_FALSE, (float*)value);
}

void ShaderProgram::bindUniform(int ID, int count, const glm::mat4* value) {
	assert(m_program > 0 && "Invalid shader program");
	assert(ID >= 0 && "Invalid shader uniform");
	glUniformMatrix4fv(ID, count, GL_FALSE, (float*)value);
}