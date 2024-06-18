#pragma once

#include "GLIncludes.h"

#include "StringIncludes.h"
#include <fstream>
#include <sstream>

#include "Debug.h"

class ComputeShader
{
public:
	unsigned int program;
	char* filepath;

	ComputeShader(const char* filepathInit)
	{
		string computeCode;
		std::ifstream file;

		file.open(filepathInit);
		if (!file.good())
		{
			Debug::LogWarning(LogID::WRN101, filepathInit, locationinfo);
			return;
		}

		uint filepathLength = (uint)strlen(filepathInit);
		filepath = new char[filepathLength + 1];
		memcpy(filepath, filepathInit, filepathLength + 1);

		std::stringstream filestream;
		filestream << file.rdbuf();
		file.close();
		computeCode = filestream.str();

		const char* shaderCode = computeCode.c_str();

		unsigned int compute = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(compute, 1, &shaderCode, NULL);
		glCompileShader(compute);

		int success;
		glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			int errorLogLength = 0;
			glGetShaderiv(compute, GL_INFO_LOG_LENGTH, &errorLogLength);
			char* errorLog = new char[errorLogLength];
			glGetShaderInfoLog(compute, errorLogLength, NULL, errorLog);
			Debug::LogError(LogID::ERR901, errorLog, locationinfo);
			return;
		}


		program = glCreateProgram();
		glAttachShader(program, compute);
		glLinkProgram(program);

		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			int errorLogLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &errorLogLength);
			char* errorLog = new char[errorLogLength];
			glGetProgramInfoLog(program, errorLogLength, NULL, errorLog);
			Debug::LogError(LogID::ERR901, errorLog, locationinfo);
			return;
		}

		glDeleteShader(compute);
	}

	~ComputeShader()
	{
		glDeleteProgram(program);
	}

	void Bind() const
	{
		glUseProgram(program);
	}

	void BindUniform(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
	}
	void BindUniform(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(program, name.c_str()), value);
	}
	void BindUniform(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(program, name.c_str()), value);
	}
	void BindUniform(const std::string& name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
	}
	void BindUniform(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
	}
	void BindUniform(const std::string& name, const glm::vec4& value) const
	{
		glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
	}
	void BindUniform(const std::string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void BindUniform(const std::string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void BindUniform(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
};