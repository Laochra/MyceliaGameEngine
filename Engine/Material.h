#pragma once

#include "Texture.h"
#include "Shader.h"

#include "JsonIncludes.h"

#include <iostream>

using std::vector;

typedef unsigned int uint;
typedef unsigned char byte;

struct MaterialInput
{
	public:
	string name = "";
	ShaderInputType type = UndefinedTypeGL;

	template<typename T>
	MaterialInput(string nameInit, T valueInit, ShaderInputType typeInit) : name(nameInit), type(typeInit), bytes(GetBytes((byte*)&valueInit, sizeof(T))) {}
	MaterialInput() = default;

	vector<byte> GetRaw() { return bytes; }
	template<typename T> bool Get(T* valueOutput)
	{
		if (bytes.size() < sizeof(T))
		{
			std::cout << "\nError: Attempted to access value larger than contained bytes\n";
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
		return bytes.size();
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
	Texture GetTexture() const noexcept;
	const char* GetTextureFilename() const noexcept;

	bool LoadFromJSON(const char* filename);

private:
	Texture* texture = nullptr;
	ShaderProgram* shaderProgram;

	vector<MaterialInput> attributes;
	vector<MaterialInput> uniforms;
};