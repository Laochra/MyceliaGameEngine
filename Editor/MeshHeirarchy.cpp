#include "MeshHeirarchy.h"

MeshHeirarchy::MeshHeirarchy(const char* filepathInit, const char* nameInit, const char* materialPathInit) noexcept :
	filepath(filepathInit),
	name(nameInit),
	materialPath(materialPathInit)
{

}
MeshHeirarchy::MeshHeirarchy(MeshHeirarchy& other) noexcept :
	filepath(other.filepath),
	name(other.name),
	materialPath(other.materialPath)
{
	for (int i = 0; i < other.children.size(); i++)
	{
		children.push_back(new MeshHeirarchy(*other.children[i]));
	}
}
MeshHeirarchy& MeshHeirarchy::operator=(MeshHeirarchy& other) noexcept
{
	Clear();
	filepath = other.filepath;
	name = other.name;
	materialPath = other.materialPath;
	for (int i = 0; i < other.children.size(); i++)
	{
		children.push_back(new MeshHeirarchy(*other.children[i]));
	}
	return *this;
}

MeshHeirarchy::~MeshHeirarchy() noexcept
{
	Clear();
}


MeshHeirarchy& MeshHeirarchy::Add(const char* filepathInit, const char* nameInit) noexcept
{
	MeshHeirarchy* newMeshHeirachy = new MeshHeirarchy();
	newMeshHeirachy->name = nameInit;
	children.push_back(newMeshHeirachy);
	return *newMeshHeirachy;
}

void MeshHeirarchy::Clear() noexcept
{
	for (int i = 0; i < children.size(); i++)
	{
		delete children[i];
	}
	children.clear();
}
