#include "LogID.h"

#define ID(idName, idMessage) const LogID LogID::idName = LogID(#idName, idMessage)

ID(Undefined, "");

ID(WRN, "");

// Initialisation
ID(WRN001, "Camera Not Initialised. ");

// Files
ID(WRN100, "Failed to Load File. ");
ID(WRN101, "File Not Found. ");
ID(WRN102, "File Corrupt. ");
ID(WRN103, "File Missing Data. ");
ID(WRN104, "File Missing Data. ");
ID(WRN105, "Requested Default Doesn't Exist. ");
ID(WRN106, "Assets should only be loaded from the Assets folder. ");
ID(WRN120, "Failed to Save File. ");
ID(WRN121, "Invalid Save Path Given. ");

// Data & Serialisation
ID(WRN150, "Data Failure. ");
ID(WRN151, "POD Data Size Mismatch. ");
ID(WRN152, "Unknown Data Format Version. ");

// 3rd Party
ID(WRN901, "OpenGL Warning. ");
ID(WRN902, "OpenGL Warning (Severe). ");

#pragma endregion
#pragma region Errors

ID(ERR, "");
ID(ERR001, "Setup Failure. ");
ID(ERR002, "Initialisation Failure. ");

// Files
ID(ERR100, "Failed to Load Critical File. ");
ID(ERR101, "Failed to Load a Default Asset. ");

// Data & Serialisation
ID(ERR150, "Fatal Data Failure. ");
ID(ERR151, "Unspecified Case for TypeID. ");

// 3rd Party
ID(ERR901, "OpenGL Error. ");
ID(ERR902, "GLFW Error. ");

#undef ID
