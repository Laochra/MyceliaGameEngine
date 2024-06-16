#pragma once

#include "Texture.h"
#include "Shader.h"

#include "JsonIncludes.h"

using std::vector;

#include <map>
using std::map;

typedef unsigned int uint;
typedef unsigned char byte;

#include "Debug.h"

struct MaterialInput
{
	public:
	string name = "";
	ShaderInputType type = UndefinedTypeGL;

	template<typename T>
	MaterialInput(string nameInit, T valueInit, ShaderInputType typeInit = UndefinedTypeGL) :
		name(nameInit), type(typeInit), bytes(GetBytes((byte*)&valueInit, sizeof(T))) {}
	MaterialInput(string nameInit, vector<byte> bytesInit, ShaderInputType typeInit = UndefinedTypeGL) :
		name(nameInit), type(typeInit) { for (byte currentByte : bytesInit) { bytes.push_back(currentByte); } }
	MaterialInput(string nameInit, string filepathInit, ShaderInputType typeInit = UndefinedTypeGL) :
		name(nameInit), type(typeInit) { for (char currentChar : filepathInit) { bytes.push_back(currentChar); } bytes.push_back('\0'); }
	MaterialInput() = default;

	bool operator==(MaterialInput& other) const
	{
		return this->name == other.name && this->type == other.type && this->bytes.size() == other.bytes.size();
	}

	string GetAsFilepath() const
	{
		string filepath;
		filepath.reserve(bytes.size());
		for (byte currentChar : bytes) { filepath.push_back(currentChar); }
		return filepath;
	}

	vector<byte> GetRaw() { return bytes; }
	template<typename T> bool Get(T* valueOutput)
	{
		if (bytes.size() < sizeof(T))
		{
			if (bytes.size() > 0) // Only give an error if a value is already initialised (this is to avoid spamming errors any time a new material file is being created)
			{
				Debug::LogWarning(WRN151, "Attempted to access a value larger than contained bytes", locationinfo);
			}
			return false;
		}
		*valueOutput = *(T*)bytes.data();
		return true;
	}

	void SetRaw(byte* valueInit, int valueSize)
	{
		bytes = GetBytes(valueInit, valueSize);
	}
	template<typename T> void Set(T valueInit)
	{
		bytes = GetBytes((byte*)&valueInit, sizeof(T));
	}

	const int GetByteCount() const
	{
		return (int)bytes.size();
	}

	private:
	vector<byte> bytes;

	vector<byte> GetBytes(byte* valueBytes, uint valueSize)
	{
		vector<byte> result;
		for (uint i = 0; i < valueSize; i++) { result.push_back(valueBytes[i]); }
		return result;
	}
};

class Material
{
public:
	Material() noexcept = default;
	uncopyable(Material)

	bool LoadFromJSON(const char* filepathInit) noexcept;
	const char* GetFilePath() const noexcept;

protected:
	char* filepath;
	ShaderProgram* shaderProgram;

	vector<MaterialInput> attributes;
	vector<MaterialInput> uniforms;

	map<string, uint> boundTextureHandles;

	friend class MeshRenderer;
};