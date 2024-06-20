#include "Scene.h"

#include "GameObjectManager.h"
#include "GameObject.h"

#include "GeneralMacros.h"

#include "StackTimer.h"

#include "Debug.h"

#define VMillions  000
#define VThousands 000
#define VHundreds  001
#define FormatVersion(vM, vT, vH) vM##vT##vH

#define LoadFormat(vM, vT, vH, scene) Load_v ## vM ##_## vT ##_## vH(scene)
#define LoadLatest(scene) exindent(Load_v) ## exindent(VMillions) ## exindent(_) ## exindent(VThousands) ## exindent(_) ## exindent(VHundreds)(scene)

#define SaveFormat(vM, vT, vH, scene) Save_v ## vM ##_## vT ##_## vH(scene)
#define SaveLatest(scene) exindent(Save_v) ## exindent(VMillions) ## exindent(_) ## exindent(VThousands) ## exindent(_) ## exindent(VHundreds)(scene)

namespace Scene
{
	const char* currentPath = nullptr;
	constexpr uint currentFormatVersion = (VMillions << 6) + (VThousands << 3) + (VHundreds);
	bool dirty = false;

	void Clear() noexcept
	{
		gameObjectManager->Clear();
	}

	bool Open(const char* filepath) noexcept
	{
		ifstream input(filepath);

		json scene;
		try { input >> scene; }
		catch (parse_error)
		{
			Debug::LogWarning(LogID::WRN102, filepath, " Scene could not be opened. ", locationinfo);
			return false;
		}

		currentPath = filepath;

		bool loadStatus = Load(scene);
		return loadStatus;
	}

#pragma region LoadVersions
	static bool Load_v000_000_001(json scene)
	{
		vector<json> gameObjects = scene["GameObjects"];
		for (int i = 0; i < gameObjects.size(); i++)
		{
			gameObjectManager->Add(GameObject::InstantiateFrom(gameObjects[i]));
		}

		return true;
	}
#pragma endregion
	bool Load(json scene) noexcept
	{
		Clear();

		uint formatVersion = scene["FormatVersion"];

		switch (formatVersion)
		{
		case FormatVersion(000, 000, 000):	return LoadLatest(scene);
		case FormatVersion(000, 000, 001):	return LoadFormat(000, 000, 001, scene);
		default:
		{
			Debug::LogWarning(LogID::WRN152, " Scene Loading Failed ", formatVersion, " ", locationinfo);
			return false;
		}
		}

		dirty = false;
	}

#pragma region SaveVersions // Purely keeping old versions for archival reasons
	static bool Save_v000_000_001(json& scene)
	{
		vector<json> gameObjects;
		for (int i = 0; i < gameObjectManager->gameObjects.size(); i++)
		{
			gameObjects.push_back(gameObjectManager->gameObjects[i]);
		}
		scene["GameObjects"] = gameObjects;

		return true;
	}
#pragma endregion
	bool Save() noexcept
	{
		json scene;
		scene["FormatVersion"] = currentFormatVersion;
		if (!SaveLatest(scene)) return false;

		ofstream output(currentPath);
		output << std::setw(2) << scene << "\n";

		dirty = false;
		return true;
	}
}
