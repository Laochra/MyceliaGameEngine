#include "Shader.h"
#include <cstdio>
#include <cassert>

#include "JsonIncludes.h"

#include <string>
using std::string;

#include <fstream>
using std::ifstream;

#include "Debug.h"

const std::map<ShaderInputType, const char*> shaderInputTypeToString = {
   {UndefinedTypeGL, "Undefined"},
	{TextureGL, "Texture"},
	{TextureArrayGL, "Texture[]"},
   {CubemapGL, "Cubemap"},

	{FloatGL, "Float"},
	{Float2GL, "Float2"},
	{Float3GL, "Float3"},
	{Float4GL, "Float4"},
	{Float2x2GL, "Float2x2"},
	{Float2x3GL, "Float2x3"},
	{Float2x4GL, "Float2x4"},
	{Float3x2GL, "Float3x2"},
	{Float3x3GL, "Float3x3"},
	{Float3x4GL, "Float3x4"},
	{Float4x2GL, "Float4x2"},
	{Float4x3GL, "Float4x3"},
	{Float4x4GL, "Float4x4"},

	{DoubleGL, "Double"},
	{Double2GL, "Double2"},
	{Double3GL, "Double3"},
	{Double4GL, "Double4"},
	{Double2x2GL, "Double2x2"},
	{Double2x3GL, "Double2x3"},
	{Double2x4GL, "Double2x4"},
	{Double3x2GL, "Double3x2"},
	{Double3x3GL, "Double3x3"},
	{Double3x4GL, "Double3x4"},
	{Double4x2GL, "Double4x2"},
	{Double4x3GL, "Double4x3"},
	{Double4x4GL, "Double4x4"},

	{IntGL, "Int"},
	{Int2GL, "Int2"},
	{Int3GL, "Int3"},
	{Int4GL, "Int4"},
	{UIntGL,  "UInt"},
	{UInt2GL,  "UInt2"},
	{UInt3GL,  "UInt3"},
	{UInt4GL,  "UInt4"},
	{BoolGL,  "Bool"},
	{BoolGL2,  "Bool2"},
	{BoolGL3,  "Bool3"},
	{BoolGL4,  "Bool4"}
};
const std::map<string, ShaderInputType> cStringToShaderInputType = {
	{"Undefined", UndefinedTypeGL},
	{"Texture", TextureGL},
	{"Texture[]", TextureArrayGL},
	{"Cubemap", CubemapGL},

 	{"Float", FloatGL},
	{"Float2", Float2GL},
	{"Float3", Float3GL},
	{"Float4", Float4GL},
	{"Float2x2", Float2x2GL},
	{"Float2x3", Float2x3GL},
	{"Float2x4", Float2x4GL},
	{"Float3x2", Float3x2GL},
	{"Float3x3", Float3x3GL},
	{"Float3x4", Float3x4GL},
	{"Float4x2", Float4x2GL},
	{"Float4x3", Float4x3GL},
	{"Float4x4", Float4x4GL},

 	{"Double", DoubleGL},
	{"Double2", Double2GL},
	{"Double3", Double3GL},
	{"Double4", Double4GL},
	{"Double2x2", Double2x2GL},
	{"Double2x3", Double2x3GL},
	{"Double2x4", Double2x4GL},
	{"Double3x2", Double3x2GL},
	{"Double3x3", Double3x3GL},
	{"Double3x4", Double3x4GL},
	{"Double4x2", Double4x2GL},
	{"Double4x3", Double4x3GL},
	{"Double4x4", Double4x4GL},

 	{"Int", IntGL},
	{"Int2", Int2GL},
	{"Int3", Int3GL},
	{"Int4", Int4GL},
	{"UInt", UIntGL},
	{"UInt2", UInt2GL},
	{"UInt3", UInt3GL},
	{"UInt4", UInt4GL},
	{"Bool", BoolGL},
	{"Bool2", BoolGL2},
	{"Bool3", BoolGL3},
	{"Bool4", BoolGL4}
};
const char* GetShaderInputTypeName(ShaderInputType type)
{
	return shaderInputTypeToString.find(type)->second;
}
ShaderInputType GetShaderInputTypeFromName(const char* typeName)
{
	return cStringToShaderInputType.find(typeName)->second;
}
ShaderInputType GetShaderInputTypeFromName(string typeName)
{
	return cStringToShaderInputType.find(typeName)->second;
}

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

	if (file == nullptr) return false;

	fseek(file, 0, SEEK_END);
	unsigned int size = ftell(file);
	char* source = new char[(size + 1) * sizeof(char)];
	fseek(file, 0, SEEK_SET);
	fread_s(source, (size + 1) * sizeof(char), sizeof(char), size, file);
	fclose(file);
	source[size] = 0;

	glShaderSource(glHandle, 1, (const char**)&source, 0);
	glCompileShader(glHandle);

	delete[] source;

	int success = GL_TRUE;
	glGetShaderiv(glHandle, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetShaderiv(glHandle, GL_INFO_LOG_LENGTH, &infoLogLength);

		delete[] lastError;
		lastError = new char[infoLogLength];
		glGetShaderInfoLog(glHandle, infoLogLength, 0, lastError);
		Debug::LogError(LogID::ERR901, lastError, locationinfo);
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
	glGetShaderiv(glHandle, GL_COMPILE_STATUS, &success);
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

ShaderProgram::ShaderProgram() : program(0), lastError(nullptr)
{
	m_shaders[0] = m_shaders[1] = m_shaders[2] = m_shaders[3] = m_shaders[4] = 0;
}
ShaderProgram::ShaderProgram(const char* jsonFilename) : lastError(nullptr)
{
	m_shaders[0] = m_shaders[1] = m_shaders[2] = m_shaders[3] = m_shaders[4] = 0;

	LoadAndLinkFromJSON(jsonFilename);
}

ShaderProgram::ShaderProgram(const char* vertex, const char* fragment)
{
	m_shaders[1] = m_shaders[2] = m_shaders[3] = 0;

	LoadShader(VertexStage, vertex);
	LoadShader(FragmentStage, fragment);
	Link();
}

ShaderProgram::~ShaderProgram()
{
	delete[] lastError;
	if (program != 0) glDeleteProgram(program);
}

bool ShaderProgram::LoadAndLinkFromJSON(const char* filepathInit)
{
	if (!JSONFileIsValid(filepathInit)) return false;

	ifstream input(filepathInit);
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

	uint filepathLength = (uint)strlen(filepathInit);
	filepath = new char[filepathLength + 1];
	memcpy(filepath, filepathInit, filepathLength + 1);

	return Link();
}

bool ShaderProgram::LoadShader(ShaderStage stage, const char* filename)
{
	if (stage <= 0 || stage >= ShaderStagesCount) return false;

	m_shaders[stage] = std::make_shared<Shader>();
	return m_shaders[stage]->LoadShader(stage, filename);
}

bool ShaderProgram::CreateShader(ShaderStage stage, const char* string)
{
	if (stage <= 0 || stage >= ShaderStagesCount) return false;

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
		Debug::LogError(LogID::ERR901, lastError, locationinfo);
		return false;
	}
	return true;
}

void ShaderProgram::Bind()
{
	assert(program > 0 && "Invalid shader program");
	glUseProgram(program);
}

const char* const ShaderProgram::GetFilepath() const noexcept
{
	return filepath;
}

void ShaderProgram::GetFields(vector<ShaderInput>& attributes, vector<ShaderInput>& uniforms)
{
	vector<ShaderInput> oldAttributes = attributes;
	vector<ShaderInput> oldUniforms = uniforms;

	attributes.clear();
	uniforms.clear();

	int count, length, size;
	const int buffer = 32;
	char name[buffer];

	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &count);
	for (int i = 0; i < count; i++)
	{
		attributes.push_back(ShaderInput());
		glGetActiveAttrib(program, i, buffer, &length, &size, (unsigned int*)&attributes.back().type, name);
		for (int i = 0; i < length; i++) { attributes.back().name += name[i]; }
		attributes.back().exposed = false;
		for (ShaderInput input : oldAttributes) { if (attributes.back().name == input.name) { attributes.back().exposed = input.exposed; } }
	}

	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);
	for (int i = 0; i < count; i++)
	{
		uniforms.push_back(ShaderInput());
		glGetActiveUniform(program, i, buffer, &length, &size, (unsigned int*)&uniforms.back().type, name);
		for (int i = 0; i < length; i++) { uniforms.back().name += name[i]; }
		uniforms.back().exposed = false;
		for (ShaderInput input : oldUniforms) { if (uniforms.back().name == input.name) { uniforms.back().exposed = input.exposed; } }
	}
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
	if (i < 0) return false;
	glUniform1i(i, value);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, const glm::uvec2& value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) return false;
	glUniform2ui(i, value.x, value.y);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, float value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) return false;
	glUniform1f(i, value);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, const vec2& value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) return false;
	glUniform2f(i, value.x, value.y);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, const vec3& value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) return false;
	glUniform3f(i, value.x, value.y, value.z);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, const vec4& value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) return false;
	glUniform4f(i, value.x, value.y, value.z, value.w);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, const mat2& value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) return false;
	glUniformMatrix2fv(i, 1, GL_FALSE, &value[0][0]);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, const mat3& value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) return false;
	glUniformMatrix3fv(i, 1, GL_FALSE, &value[0][0]);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, const mat4& value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) return false;
	glUniformMatrix4fv(i, 1, GL_FALSE, &value[0][0]);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, int count, int* value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) return false;
	glUniform1iv(i, count, value);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, int count, float* value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) return false;
	glUniform1fv(i, count, value);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, int count, const vec2* value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) return false;
	glUniform2fv(i, count, (float*)value);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, int count, const vec3* value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) return false;
	glUniform3fv(i, count, (float*)value);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, int count, const vec4* value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) return false;
	glUniform4fv(i, count, (float*)value);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, int count, const mat2* value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) return false;
	glUniformMatrix2fv(i, count, GL_FALSE, (float*)value);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, int count, const mat3* value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) return false;
	glUniformMatrix3fv(i, count, GL_FALSE, (float*)value);
	return true;
}

bool ShaderProgram::BindUniform(const char* name, int count, const mat4* value)
{
	assert(program > 0 && "Invalid shader program");
	int i = glGetUniformLocation(program, name);
	if (i < 0) return false;
	glUniformMatrix4fv(i, count, GL_FALSE, (float*)value);
	return true;
}


bool ShaderProgram::LoadShaderFromJSON(ShaderStage stage, const char* filename)
{
	if (memcmp(filename, "None", 5) == 0)
	{
		ClearShader(stage);
		return true;
	}

	if (memchr(filename, '.', strlen(filename)) == nullptr)
	{
		switch (stage)
		{
			case VertexStage:
				if (LoadShader(stage, ("Engine\\DefaultAssets\\" + string(filename) + ".vert").c_str()) == false)
				{
					Debug::LogWarning(LogID::WRN102, filename, " is not a valid shader file", locationinfo);
					return false;
				}
				break;
			case FragmentStage:
				if (LoadShader(stage, ("Engine\\DefaultAssets\\" + string(filename) + ".frag").c_str()) == false)
				{
					Debug::LogWarning(LogID::WRN102, filename, " is not a valid shader file", locationinfo);
					return false;
				}
				break;
			default: Debug::LogWarning(LogID::WRN104, "Shader stage ", stage, " has no default", locationinfo);
				return false;
		}
	}
	else
	{
		if (LoadShader(stage, filename) == false)
		{
			Debug::LogWarning(LogID::WRN102, filename, " is not a valid shader file", locationinfo);
			return false;
		}
	}


	return true;
}

bool ShaderProgram::JSONFileIsValid(const char* filename)
{
	if (memchr(filename, '.', strlen(filename)) == nullptr) { return true; }

	ifstream input(filename);
	if (!input.good())
	{
		Debug::LogWarning(LogID::WRN101, filename, " It may have been moved, deleted, or renamed ", locationinfo);
		return false;
	}

	json shaderProgram;
	try { input >> shaderProgram; }
	catch (parse_error)
	{
		Debug::LogWarning(LogID::WRN102, filename, " is not a valid shader file", locationinfo);
		return false;
	}

	bool incompleteFile = false;
	if (!shaderProgram.contains("Vertex"))
	{
		Debug::LogWarning(LogID::WRN103, filename, " does not specify a Vertex shader, specify \"None\" to disregard it", locationinfo);
		incompleteFile = true;
	}
	if (!shaderProgram.contains("TessEvaluation"))
	{
		Debug::LogWarning(LogID::WRN103, filename, " does not specify a Tess Evaluation shader, specify \"None\" to disregard it", locationinfo);
		incompleteFile = true;
	}
	if (!shaderProgram.contains("TessControl"))
	{
		Debug::LogWarning(LogID::WRN103, filename, " does not specify a Tess Control shader, specify \"None\" to disregard it", locationinfo);
		incompleteFile = true;
	}
	if (!shaderProgram.contains("Geometry"))
	{
		Debug::LogWarning(LogID::WRN103, filename, " does not specify a Geometry shader, specify \"None\" to disregard it", locationinfo);
		incompleteFile = true;
	}
	if (!shaderProgram.contains("Fragment"))
	{
		Debug::LogWarning(LogID::WRN103, filename, " does not specify a Fragment shader, specify \"None\" to disregard it", locationinfo);
		incompleteFile = true;
	}
	if (incompleteFile) return false;

	return true;
}
