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

#ifndef stringify
	#define expandedstringify(s) stringify(s)
	#define stringify(s) #s
#endif

#ifndef locationinfo
	#define locationinfo " (" __FILE__ " " expandedstringify(__LINE__) ")"
#endif

#ifndef mappedenum
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
#endif

#ifndef exindent
	#define exindent(x) x
#endif

#ifndef foreachf
	#define foreachf_it0(function)
	#define foreachf_it1(function, current)		function(current)
	#define foreachf_it2(function, current, ...) function(current) exindent(foreachf_it1(function, __VA_ARGS__))
	#define foreachf_it3(function, current, ...) function(current) exindent(foreachf_it2(function, __VA_ARGS__))
	#define foreachf_it4(function, current, ...) function(current) exindent(foreachf_it3(function, __VA_ARGS__))
	#define foreachf_it5(function, current, ...) function(current) exindent(foreachf_it4(function, __VA_ARGS__))
	#define foreachf_it6(function, current, ...) function(current) exindent(foreachf_it5(function, __VA_ARGS__))
	#define foreachf_it7(function, current, ...) function(current) exindent(foreachf_it6(function, __VA_ARGS__))
	#define foreachf_it8(function, current, ...) function(current) exindent(foreachf_it7(function, __VA_ARGS__))
	#define foreachf_it9(function, current, ...) function(current) exindent(foreachf_it8(function, __VA_ARGS__))
	//... repeat if needed
	#define foreachf_getmacro(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,NAME,...) NAME 
	#define foreachf(action,...) \
				exindent(foreachf_getmacro(_0, __VA_ARGS__, \
				exindent(foreachf_it9),	\
				exindent(foreachf_it8),	\
				exindent(foreachf_it7),	\
				exindent(foreachf_it6),	\
				exindent(foreachf_it5),	\
				exindent(foreachf_it4),	\
				exindent(foreachf_it3),	\
				exindent(foreachf_it2),	\
				exindent(foreachf_it1),	\
				exindent(foreachf_it0))	\
				(action,__VA_ARGS__))
#endif

#ifndef multicase
	#define foreachf_case(value) case value:
	#define cases(...) foreachf(foreachf_case, __VA_ARGS__)
#endif