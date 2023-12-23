#pragma once

#include <glm.hpp>
#include <memory>

// simplified render pipeline shader stages
enum eShaderStage : unsigned int {
	UNDEFINED = 0,

	VERTEX,
	TESSELLATION_EVALUATION,
	TESSELLATION_CONTROL,
	GEOMETRY,
	FRAGMENT,

	SHADER_STAGE_Count,
};

// individual sharable shader stages
class Shader {
public:

	Shader() : m_stage(0), m_handle(0), m_lastError(nullptr) {}
	Shader(unsigned int stage, const char* filename)
		: m_stage(0), m_handle(0), m_lastError(nullptr) {
		loadShader(stage, filename);
	}
	~Shader();

	bool loadShader(unsigned int stage, const char* filename);
	bool createShader(unsigned int stage, const char* string);

	unsigned int getStage() const { return m_stage; }
	unsigned int getHandle() const { return m_handle; }

	const char* getLastError() const { return m_lastError; }

protected:

	unsigned int	m_stage;
	unsigned int	m_handle;
	char*			m_lastError;
};

// combines shaders together into a single program for the GPU
class ShaderProgram {
public:

	ShaderProgram() : m_program(0), m_lastError(nullptr) {
		m_shaders[0] = m_shaders[1] = m_shaders[2] = m_shaders[3] = m_shaders[4] = 0;
	}
	~ShaderProgram();

	bool loadShader(unsigned int stage, const char* filename);
	bool createShader(unsigned int stage, const char* string);
	void attachShader(const std::shared_ptr<Shader>& shader);

	bool link();

	const char* getLastError() const { return m_lastError; }

	void bind();

	unsigned int getHandle() const { return m_program; }

	int getUniform(const char* name);

	void bindUniform(int ID, int value);
	void bindUniform(int ID, float value);
	void bindUniform(int ID, const glm::vec2& value);
	void bindUniform(int ID, const glm::vec3& value);
	void bindUniform(int ID, const glm::vec4& value);
	void bindUniform(int ID, const glm::mat2& value);
	void bindUniform(int ID, const glm::mat3& value);
	void bindUniform(int ID, const glm::mat4& value);
	void bindUniform(int ID, int count, int* value);
	void bindUniform(int ID, int count, float* value);
	void bindUniform(int ID, int count, const glm::vec2* value);
	void bindUniform(int ID, int count, const glm::vec3* value);
	void bindUniform(int ID, int count, const glm::vec4* value);
	void bindUniform(int ID, int count, const glm::mat2* value);
	void bindUniform(int ID, int count, const glm::mat3* value);
	void bindUniform(int ID, int count, const glm::mat4* value);

	// these calls should be avoided, but wraps up opengl a little
	bool bindUniform(const char* name, int value);
	bool bindUniform(const char* name, float value);
	bool bindUniform(const char* name, const glm::vec2& value);
	bool bindUniform(const char* name, const glm::vec3& value);
	bool bindUniform(const char* name, const glm::vec4& value);
	bool bindUniform(const char* name, const glm::mat2& value);
	bool bindUniform(const char* name, const glm::mat3& value);
	bool bindUniform(const char* name, const glm::mat4& value);
	bool bindUniform(const char* name, int count, int* value);
	bool bindUniform(const char* name, int count, float* value);
	bool bindUniform(const char* name, int count, const glm::vec2* value);
	bool bindUniform(const char* name, int count, const glm::vec3* value);
	bool bindUniform(const char* name, int count, const glm::vec4* value);
	bool bindUniform(const char* name, int count, const glm::mat2* value);
	bool bindUniform(const char* name, int count, const glm::mat3* value);
	bool bindUniform(const char* name, int count, const glm::mat4* value);

private:

	unsigned int	m_program;

	std::shared_ptr<Shader> m_shaders[eShaderStage::SHADER_STAGE_Count];

	char*			m_lastError;
};