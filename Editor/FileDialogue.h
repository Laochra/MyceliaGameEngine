#pragma once

#include <string>
#include <vector>

typedef unsigned int uint;

namespace FileDialogue
{
	enum class LimitToAssetFolder : bool
	{
		False = false,
		True = true
	};

	struct PathDetails
	{
		const char* const title;
		const char* const defaultPath;
		const std::vector<const char*> extensions;

		PathDetails(
			const char* const titleInit = "File",
			const char* const defaultPathInit = "Assets\\",
			std::vector<const char*> extensionsInit = {}) noexcept :
			title(titleInit),
			defaultPath(defaultPathInit),
			extensions(extensionsInit) { }
	};

	enum class MBType { Ok, OkCancel, YesNo, YesNoCancel };
	enum class MBIcon { Info, Warning, Error, Question };

	struct MessageBox
	{
		const char* const title;
		const char* const message;
		MBType type;
		MBIcon icon;

		MessageBox(
			const char* const titleInit = "Message Box",
			const char* const messageInit = "This is a message box.",
			MBType typeInit = MBType::Ok,
			MBIcon iconInit = MBIcon::Info) noexcept :
			title(titleInit),
			message(messageInit),
			type(typeInit),
			icon(iconInit)
		{}

		const char* const GetType() const noexcept
		{
			switch (type)
			{
			default:							return "ok";
			case MBType::OkCancel:		return "okcancel";
			case MBType::YesNo:			return "yesno";
			case MBType::YesNoCancel:	return "yesnocancel";
			}
		}
		const char* const GetIcon() const noexcept
		{
			switch (icon)
			{
			default:						return "info";
			case MBIcon::Warning:	return "warning";
			case MBIcon::Error:		return "error";
			case MBIcon::Question:	return "question";
			}
		}
	};

	// Returns string of size 0 on failure
	std::string GetSavePath(const PathDetails details, const LimitToAssetFolder limitToAssetsFolder) noexcept;

	// Returns string of size 0 on failure
	std::string GetLoadPath(const PathDetails details, const LimitToAssetFolder limitToAssetsFolder) noexcept;
	// Returns vector of size 0 on failure
	std::vector<std::string> GetLoadPaths(const PathDetails details, const LimitToAssetFolder limitToAssetsFolder) noexcept;

	// Returns string of size 0 on failure, doesn't use extensions
	std::string GetFolderPath(const PathDetails details, const LimitToAssetFolder limitToAssetsFolder);

	int DisplayMessageBox(const MessageBox messageBox) noexcept;
}
