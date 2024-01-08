#pragma once

#include <string>
#include <fstream>
#include <sstream>

inline std::string LoadFileAsString(std::string filename)
{
	std::stringstream fileContents;
	std::ifstream inputFile(filename);

	if (inputFile.is_open())
	{
		while (!inputFile.eof())
		{
			std::string currentLine;
			std::getline(inputFile, currentLine);
			fileContents << currentLine << "\n";
		}
		return fileContents.str();
	}
}