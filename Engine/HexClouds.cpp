#include "HexClouds.h"

#include "HexGrid.h"
#include "MeshRenderer.h"

#include "TimeManager.h"
#include "RandomGen.h"

HexCloudData HexClouds::data;
std::vector<std::string> HexClouds::prefabFilepaths;
std::vector<json> HexClouds::prefabs;

std::vector<MeshRenderer*> clouds;

void HexClouds::Initialise() noexcept
{
	float minDistanceSqr = data.minDistance * data.minDistance;

	for (int i = 0; i < data.maxClouds; i++)
	{
		vec3 newSpawnPos = vec3(
			Random::Float(-HEX_GRID_RADIUS * 2, HEX_GRID_RADIUS * 2),
			data.cloudHeight,
			Random::Float(-HEX_GRID_RADIUS * 2, HEX_GRID_RADIUS * 2)
		);
		if (glm::length2(vec2(newSpawnPos.x, newSpawnPos.z)) > HEX_GRID_RADIUS * HEX_GRID_RADIUS * 2)
		{
			continue;
		}

		bool isAbleToSpawn = true;
		for (int c = 0; c < (int)clouds.size(); c++)
		{
			if (glm::length2(clouds[c]->GetPosition() - newSpawnPos) < minDistanceSqr)
			{
				isAbleToSpawn = false;
				break;
			}
		}
		if (isAbleToSpawn)
		{
			MeshRenderer* cloud;

			if (prefabs.size() > 0)
			{
				cloud = (MeshRenderer*)GameObject::InstantiateFrom(prefabs[Random::Int32(0, prefabs.size() - 1)], GuidGeneration::New);
			}
			else
			{
				cloud = GameObject::Instantiate<MeshRenderer>();
				cloud->SetMesh("Hexagon");
			}
			cloud->SetPosition(newSpawnPos);
			cloud->SetScale(Random::Float(data.sizeRange[0], data.sizeRange[1]));
			cloud->Rotate(glm::radians(Random::Float(0, 359.99f)), vec3(0, 1, 0));

			clouds.push_back(cloud);
		}
	}
}

void HexClouds::Update() noexcept
{
	for (MeshRenderer*& cloud : clouds)
	{
		cloud->SetPosition(cloud->GetPosition() + vec3(data.moveDir[0], 0, data.moveDir[1]) * data.moveSpeed * Time::delta);
		if (glm::length2(vec2(cloud->GetPosition().x, cloud->GetPosition().z)) > HEX_GRID_RADIUS * HEX_GRID_RADIUS * 2)
		{
			cloud->SetPosition(vec3(-cloud->GetPosition().x, data.cloudHeight, -cloud->GetPosition().z));
		}
	}
}

void HexClouds::LoadPrefab(int index, std::string filepath) noexcept
{
	while (prefabFilepaths.size() <= index)
	{
		prefabFilepaths.push_back(std::string());
	}
	while (prefabs.size() <= index)
	{
		prefabs.push_back(json());
	}

	json& prefab = prefabs[index];
	std::string& prefabFilepath = prefabFilepaths[index];

	if (filepath != "None")
	{
		ifstream prefabFile(filepath);

		if (!prefabFile.good())
		{
			Debug::LogWarning(LogID::WRN101, "Cloud Prefab ", index, ".", locationinfo);
			return;
		}

		try { prefabFile >> prefab; }
		catch (parse_error)
		{
			Debug::LogWarning(LogID::WRN102, "Cloud Prefab ", index, ".", locationinfo);
			return;
		}

		prefabFilepath = filepath;
	}
}

void HexClouds::CleanUp() noexcept
{
	for (MeshRenderer* cloud : clouds)
	{
		gameObjectManager->Delete(cloud);
	}
	clouds.clear();
}
