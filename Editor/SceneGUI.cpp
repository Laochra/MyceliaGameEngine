#include "SceneGUI.h"

#include "JsonIncludes.h"
#include "Inspector.h"
#include "GameObjectManager.h"
#include "GameObject.h"

#include "GeneralMacros.h"

#include "Debug.h"

#define VMillions  000
#define VThousands 000
#define VHundreds  001
#define FormatVersion(vM, vT, vH) vM##vT##vH

#define LoadSceneFormat(vM, vT, vH, scene) LoadScene_v ## vM ##_## vT ##_## vH(scene)
#define LoadSceneLatest(scene) exindent(LoadScene_v) ## exindent(VMillions) ## exindent(_) ## exindent(VThousands) ## exindent(_) ## exindent(VHundreds)(scene)

#define SaveSceneFormat(vM, vT, vH, scene) SaveScene_v ## vM ##_## vT ##_## vH(scene)
#define SaveSceneLatest(scene) exindent(SaveScene_v) ## exindent(VMillions) ## exindent(_) ## exindent(VThousands) ## exindent(_) ## exindent(VHundreds)(scene)

namespace SceneGUI
{
	const char* currentScenePath = nullptr;
	constexpr uint currentFormatVersion = (VMillions << 6) + (VThousands << 3) + (VHundreds);
	bool dirty = false;

	void SceneGUI::DrawFileDropdown() noexcept
	{
		if (ImGui::MenuItem(" New Scene", "Ctrl+N"))
		{
			NewScene();
		}
		if (ImGui::MenuItem(" Open Scene", "Ctrl+O"))
		{
			OpenScene();
		}
		if (ImGui::MenuItem(" Save Scene", "Ctrl+S"))
		{
			SaveScene();
		}
		if (ImGui::MenuItem(" Save Scene As", "Ctrl+Shift+S"))
		{
			SaveSceneAs();
		}
	}

	bool ClearScene() noexcept
	{
		const char* const dialogueTitle = "Unsaved scene data will be lost!";
		const char* const dialogueMessage = "Would you like to save your changes?";
		const char* const dialogueType = "yesnocancel";
		const char* const dialogueIcon = "warning";

		int selectedOption = tinyfd_messageBox(dialogueTitle, dialogueMessage, dialogueType, dialogueIcon, 0);

		switch (selectedOption)
		{
		default: debug->Log("Unexpected return value from save message box", Debug::Warning); return false;
		case 0: return false;			// int 0 means Cancel
		case 1: SaveScene(); break;	// int 1 means Yes, do save changes
		case 2: break;						// int 2 means No, don't save changes
		}

		inspector->SetTarget(nullptr);
		gameObjectManager->Clear();
		return true;
	}
	bool NewScene() noexcept
	{
		if (!ClearScene()) return false;
		currentScenePath = nullptr;
		dirty = false;
		return true;
	}
	bool OpenScene() noexcept
	{
		const char* const windowTitle = "Open Scene";
		const uint defaultPathLength = 15;
		const char defaultPath[defaultPathLength] = "Assets\\Scenes\\";
		const uint filterPatternCount = 1;
		const char* const filterPatterns[filterPatternCount] = { "*.scene" };

		const char* const filePath = tinyfd_openFileDialog(windowTitle, defaultPath, filterPatternCount, filterPatterns, nullptr, false);
		if (filePath == nullptr) return false;
		const uint filePathLength = (uint)strlen(filePath);

		const uint startOffset = (uint)string(filePath).find("Assets\\");

		if (startOffset == string::npos)
		{
			debug->Log
			(
				"Assets should only be loaded from inside the Assets folder. "
				"Assets outside this folder won't be included in builds. "
				"Scene was not saved!",
				Debug::Warning, Debug::WRN105
			);
			return false;
		}

		currentScenePath = filePath + startOffset;

		ifstream input(currentScenePath);

		json scene;
		try { input >> scene; }
		catch (parse_error)
		{
			debug->Log
			(
				{
					currentScenePath, " was corrupt. "
					"Scene could not be opened. "
					locationinfo
				},
				Debug::Warning, Debug::WRN102
			);
			return false;
		}

		if (!ClearScene()) return false;

		LoadScene(scene);

		dirty = false;
		return true;
	}
#pragma region LoadVersions
	static bool LoadScene_v000_000_001(json scene)
	{
		vector<json> gameObjects = scene["GameObjects"];
		for (int i = 0; i < gameObjects.size(); i++)
		{
			gameObjectManager->Add(GameObject::InstantiateFrom(gameObjects[i]));
		}

		return true;
	}
#pragma endregion
	static bool LoadScene(json scene) noexcept // Select the correct version of load to run
	{
		uint formatVersion = scene["FormatVersion"];

		switch (formatVersion)
		{
		case FormatVersion(000,000,000):	return LoadSceneLatest(scene);
		case FormatVersion(000,000,001):	return LoadSceneFormat(000,000,001, scene);
		default:
		{
			debug->Log
			(
				{
					"Unknown Scene Format Version couldn't be loaded: ",
					std::to_string(formatVersion), " "
					locationinfo
				},
				Debug::Warning, Debug::WRN152
			);
			return false;
		}
		}
	}
#pragma region SaveVersions // Purely keeping old versions for archival reasons
	static bool SaveScene_v000_000_001(json& scene)
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
	bool SaveScene() noexcept
	{
		if (currentScenePath == nullptr)
		{
			return SaveSceneAs();
		}

		json scene;
		scene["FormatVersion"] = currentFormatVersion;
		if (!SaveSceneLatest(scene)) return false;

		ofstream output(currentScenePath);
		output << std::setw(2) << scene << "\n";

		dirty = false;
		return true;
	}
	bool SaveSceneAs() noexcept
	{
		const char* const windowTitle = "Save Scene As";
		const uint defaultPathLength = 23;
		const char defaultPath[defaultPathLength] = "Assets\\Scenes\\NewScene";
		const uint filterPatternCount = 1;
		const char* const filterPatterns[filterPatternCount] = { "*.scene" };

		const char* const filePath = tinyfd_saveFileDialog(windowTitle, defaultPath, filterPatternCount, filterPatterns, nullptr);
		if (filePath == nullptr) return false;
		const uint filePathLength = (uint)strlen(filePath);

		const uint startOffset = (uint)string(filePath).find("Assets\\");

		if (startOffset == string::npos)
		{
			debug->Log
			(
				"Assets should only be saved inside the Assets folder. "
				"Assets outside this folder won't be included in builds. "
				"Scene was not saved! "
				locationinfo,
				Debug::Warning, Debug::WRN121
			);
			return false;
		}

		currentScenePath = filePath + startOffset;

		if (!SaveScene()) return false;

		return true;
	}
}
