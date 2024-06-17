#pragma once

#include <string>

class LogID
{
public: 
	std::string name;
	std::string message;

	LogID(std::string nameInit, std::string messageInit) :
		name(nameInit),
		message(messageInit) {}
	LogID& operator=(const LogID& other) { name = other.name; message = other.message; return *this; }
	bool operator==(const LogID& other) const { return this->name == other.name; }

	static const LogID Undefined;	// Default ID

#pragma region Warnings

	static const LogID WRN;		// Undefined Warning

	/// Initialisation
	static const LogID WRN001;		// Camera Not Initialised

	/// Files
	static const LogID WRN100;		// Failed to Load File
	static const LogID WRN101;		// File Not Found
	static const LogID WRN102;		// File Corrupt
	static const LogID WRN103;		// File Missing Data
	static const LogID WRN104;		// Requested Default Doesn't Exist
	static const LogID WRN105;		// Invalid Load Path Given
	static const LogID WRN106;		// Assets should only be loaded from the Assets folder
	static const LogID WRN120;		// Failed to Save File
	static const LogID WRN121;		// Invalid Save Path Given

	/// Data & Serialisation
	static const LogID WRN150;		// Data Failure
	static const LogID WRN151;		// POD Data Size Mismatch
	static const LogID WRN152;		// Unknown Data Format Version

	/// 3rd Party
	static const LogID WRN901;		// OpenGL Warning
	static const LogID WRN902;		// OpenGL Warning (Severe)

#pragma endregion
#pragma region Errors

	static const LogID ERR;		// Undefined Error

	/// Initialisation
	static const LogID ERR001;		// Setup Failure
	static const LogID ERR002;		// Initialisation Failure

	/// Files
	static const LogID ERR100;		// Failed to Load Critical File
	static const LogID ERR101;		// Failed to Load a Default Asset

	/// Data & Serialisation
	static const LogID ERR150;		// Fatal Data Failure
	static const LogID ERR151;		// Unspecified Case for TypeID

	/// 3rd Party
	static const LogID ERR901;		// OpenGL Error
	static const LogID ERR902;		// GLFW Error

#pragma endregion

};
