#include "FileDialogue.h"

#include "tinyfiledialogs.h"
#include "Debug.h"

std::string FileDialogue::GetSavePath(const PathDetails details, const LimitToAssetFolder limitToAssetsFolder) noexcept
{
	const char* const bytes = tinyfd_saveFileDialog(details.title, details.defaultPath, details.extensions.size(), details.extensions.data(), nullptr);
	if (bytes == nullptr) return std::string(); // Not an error. This happens if user cancels when selecting files.
	
	std::string filepath = bytes;
	
	const uint startOffset = (bool)limitToAssetsFolder ? (uint)filepath.find("Assets\\") : 0;
	if (startOffset == std::string::npos)
	{
		Debug::LogWarning(LogID::WRN121, "Assets outside the Assets folder won't be included in builds. ", locationinfo);
		return std::string();
	}
	filepath.erase(filepath.begin(), filepath.begin() + startOffset);
	
	return filepath;
}

std::string FileDialogue::GetLoadPath(PathDetails details, LimitToAssetFolder limitToAssetsFolder) noexcept
{
	const char* const bytes = tinyfd_openFileDialog(details.title, details.defaultPath, details.extensions.size(), details.extensions.data(), nullptr, false);
	if (bytes == nullptr) return std::string(); // Not an error. This happens if user cancels when selecting files.
	
	std::string filepath = bytes;

	const uint startOffset = (bool)limitToAssetsFolder ? (uint)filepath.find("Assets\\") : 0;
	if (startOffset == std::string::npos)
	{
		Debug::LogWarning(LogID::WRN106, filepath, " won't be included in builds. ", locationinfo);
		return std::string();
	}
	filepath.erase(filepath.begin(), filepath.begin() + startOffset);

	return filepath;
}

std::vector<std::string> FileDialogue::GetLoadPaths(PathDetails details, LimitToAssetFolder limitToAssetsFolder) noexcept
{
	std::vector<std::string> filepaths;
	{
		char* bytes = tinyfd_openFileDialog(details.title, details.defaultPath, details.extensions.size(), details.extensions.data(), nullptr, true);
		if (bytes == nullptr) return std::vector<std::string>(); // Not an error. This happens if user cancels when selecting files.
		
		std::string filepathPack = bytes;

		std::string str;
		for (uint i = 0; i < filepathPack.size(); i++)
		{
			if (filepathPack[i] == '|')
			{
				filepaths.push_back(str);
				str.clear();
				continue;
			}
			str.push_back(filepathPack[i]);
		}
		if (str.size() > 0) filepaths.push_back(str);
	}

	for (uint i = 0; i < filepaths.size(); i++)
	{
		const uint startOffset = (bool)limitToAssetsFolder ? (uint)filepaths[i].find("Assets\\") : 0;
		if (startOffset == std::string::npos)
		{
			Debug::LogWarning(LogID::WRN106, filepaths[i], " won't be included in builds. ", locationinfo);
			filepaths[i].clear();
			continue;
		}
		filepaths[i].erase(filepaths[i].begin(), filepaths[i].begin() + startOffset);
	}

	uint i = 0;
	while (i < filepaths.size())
	{
		if (filepaths[i].size() > 0) i++;
		else filepaths.erase(filepaths.begin() + i);
	}

	return filepaths;
}

int FileDialogue::DisplayMessageBox(const MessageBox messageBox) noexcept
{
	return tinyfd_messageBox(messageBox.title, messageBox.message, messageBox.GetType(), messageBox.GetIcon(), 0);
}
