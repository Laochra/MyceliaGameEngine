#pragma once

#ifndef del
#define del(pointer) if (pointer != nullptr) { delete pointer; pointer = nullptr; }
#endif

#ifndef as
#define as(type) *(type*)&
#endif

#ifndef sqr
#define sqr(x) (x*x)
#endif

#ifndef uncopyable
#define uncopyable(classname) classname(const classname&) = delete; classname& operator=(const classname&) = delete;
#endif

#ifndef immovable
#define immovable(classname) classname(classname&&) = delete; classname& operator=(classname&&) = delete;
#endif

#define expandedstringify(s) stringify(s)
#define stringify(s) #s
#define locationinfo " (" __FILE__ " " expandedstringify(__LINE__) ")"

#define mappedenum(name, type, ...)\
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

#define mappedenumi(name, type, currentAccessibility, ...)\
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