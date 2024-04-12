#pragma once

#ifndef del(pointer)
#define del(pointer) if (pointer != nullptr) { delete pointer; pointer = nullptr; }
#endif

#ifndef uncopyable(classname)
#define uncopyable(classname) classname(const classname&) = delete; classname& operator=(const classname&) = delete;
#endif

#ifndef immovable(classname)
#define immovable(classname) classname(classname&&) = delete; classname& operator=(classname&&) = delete;
#endif

#define MappedEnum(name, type, ...)\
enum name : type { __VA_ARGS__ };\
inline std::map<name, string> name##GenerateMap(string strings)\
{ \
	std::map<name, string> result;\
	name current##name = (name)0;\
	string currentString;\
	for (type i = 0; i < strings.size(); i++)\
	{\
		if (strings[i] == ' ') { continue; }\
		if (strings[i] == ',')\
		{\
			result.insert(std::pair<name, string>(current##name, currentString));\
			current##name = (name)(current##name + 1);\
			currentString.clear();\
			continue;\
		}\
		currentString.push_back(strings[i]);\
	}\
	if (currentString.size() > 0) result.insert(std::pair<name, string>(current##name, currentString));\
	return result;\
} \
std::map<name, string> name##Map = name##GenerateMap(#__VA_ARGS__);

#define InternalMappedEnum(name, type, currentAccessibility, ...)\
enum name : type { __VA_ARGS__ };\
protected:\
std::map<name, string> name##GenerateMap(string strings)\
{ \
	std::map<name, string> result;\
	name current##name = (name)0;\
	string currentString;\
	for (type i = 0; i < strings.size(); i++)\
	{\
		if (strings[i] == ' ') { continue; }\
		if (strings[i] == ',')\
		{\
			result.insert(std::pair<name, string>(current##name, currentString));\
			current##name = (name)(current##name + 1);\
			currentString.clear();\
			continue;\
		}\
		currentString.push_back(strings[i]);\
	}\
	if (currentString.size() > 0) result.insert(std::pair<name, string>(current##name, currentString));\
	return result;\
} \
std::map<name, string> name##Map = name##GenerateMap(#__VA_ARGS__);\
currentAccessibility##: