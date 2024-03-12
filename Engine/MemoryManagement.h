#pragma once

#ifndef del(pointer)
#define del(pointer) if (pointer != nullptr) { delete pointer; pointer = nullptr; }
#endif