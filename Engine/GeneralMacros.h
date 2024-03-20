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