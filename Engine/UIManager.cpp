#include "UIManager.h"

#include "GeneralMacros.h"

vector<UISprite*> UIManager::sprites;

static bool IsSpriteABehindB(const UISprite* a, const UISprite* b)
{
	return a->layer > b->layer;
}

void UIManager::Sort() noexcept
{
	std::sort(sprites.begin(), sprites.end(), IsSpriteABehindB);
}

void UIManager::CleanUp() noexcept
{
	for (UISprite* uiSprite : sprites)
	{
		del(uiSprite);
	}
	sprites.clear();
}
