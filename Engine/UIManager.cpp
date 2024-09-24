#include "UIManager.h"

#include "GeneralMacros.h"

vector<UISprite*> UIManager::sprites;

void UIManager::CleanUp() noexcept
{
	for (UISprite* uiSprite : sprites)
	{
		del(uiSprite);
	}
}
