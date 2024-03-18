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
	MaterialInput(string nameInit, T valueInit, ShaderInputType typeInit = UndefinedTypeGL) :
		name(nameInit), type(typeInit), bytes(GetBytes((byte*)&valueInit, sizeof(T))) {}
	MaterialInput(string nameInit, vector<byte> bytesInit, ShaderInputType typeInit = UndefinedTypeGL) :
		name(nameInit), type(typeInit) { for (byte currentByte : bytesInit) { bytes.push_back(currentByte); } }
	MaterialInput(string nameInit, string filepathInit, ShaderInputType typeInit = UndefinedTypeGL) :
		name(nameInit), type(typeInit) { for (char currentChar : filepathInit) { bytes.push_back(currentChar); } bytes.push_back('\0'); }
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
	bool LoadFromJSON(const char* filepathInit);
	const char* GetFilePath();

protected:
	char* filepath;
	ShaderProgram* shaderProgram;

	vector<MaterialInput> attributes;
	vector<MaterialInput> uniforms;

	friend class MeshRenderer;
};