#pragma once

#include "UISprite.h"
#include <vector>
using std::vector;

class UIManager
{
public:
	static vector<UISprite*> sprites;

	static void Sort() noexcept;

	static void CleanUp() noexcept;
};
