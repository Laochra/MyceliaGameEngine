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

	ComputeShader(const char* filepath)
	{
		string computeCode;
		std::ifstream file;

		file.open(filepath);
		if (!file.good())
		{
			debug->Log({ "Compute shader file not found at: ", filepath, locationinfo }, Debug::Warning, Debug::WRN101);
			return;
		}

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
			glGetShaderiv(program, GL_INFO_LOG_LENGTH, &errorLogLength);
			char* errorLog = new char[errorLogLength];
			glGetShaderInfoLog(compute, errorLogLength, NULL, errorLog);
			debug->Log({ errorLog, locationinfo }, Debug::Error, Debug::ERR901);
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
			debug->Log({ errorLog, locationinfo }, Debug::Error, Debug::ERR901);
			return;
		}

		glDeleteShader(compute);
	}

	~ComputeShader()
	{
		glDeleteProgram(program);
	}

	void Bind()
	{
		glUseProgram(program);
	}

	void SetUniform(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
	}
	void SetUniform(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(program, name.c_str()), value);
	}
	void SetUniform(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(program, name.c_str()), value);
	}
	void SetUniform(const std::string& name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
	}
	void SetUniform(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
	}
	void SetUniform(const std::string& name, const glm::vec4& value) const
	{
		glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
	}
	void SetUniform(const std::string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void SetUniform(const std::string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void SetUniform(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
};