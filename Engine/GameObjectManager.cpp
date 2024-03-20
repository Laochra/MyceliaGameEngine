#include "GameObjectManager.h"

//#include "GameObject2D.h"
#include "GameObject3D.h"

#include "GeneralMacros.h"

GameObjectManager* gameObjectManager;

GameObjectManager::~GameObjectManager() noexcept
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		GameObject::Destroy(gameObjects[i]);
	}
	for (int i = 0; i < graveyard.size(); i++)
	{
		Delete(graveyard[i]);
		graveyard.erase(graveyard.begin() + i);
	}
}

void GameObjectManager::Add(GameObject* gameObject) noexcept
{
	gameObjects.push_back(gameObject);
}
void GameObjectManager::Remove(GameObject* gameObject) noexcept
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		if (*gameObjects[i] == *gameObject)
		{
			gameObjects.erase(gameObjects.begin() + i);
			break;
		}
	}
}
void GameObjectManager::Bury(GameObject* gameObject) noexcept
{
	graveyard.push_back(gameObject);
}
void GameObjectManager::Move(GameObject* gameObject, int newIndex) noexcept
{
	int i = GetIndexOf(gameObject);

	if (i == newIndex) return;

	char direction;
	if (i < newIndex) direction = 1;
	else direction = -1;

	GameObject* current;
	while (i != newIndex)
	{
		if (i + direction < 0 || i + direction >= gameObjects.size()) return;

		current = gameObjects[i];
		gameObjects[i] = gameObjects[i + direction];
		gameObjects[i + direction] = current;

		i += direction;
	}
}
void GameObjectManager::Delete(GameObject* gameObject)
{
	if (gameObject == nullptr) return;

	if (*gameObject != GameObject::Destroyed)
	{
		GameObject::Destroy(gameObject);
	}

	del(gameObject);
}

int GameObjectManager::GetIndexOf(GameObject* gameObject) const noexcept
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i] == gameObject)
		{
			return i;
		}
	}

	return -1;
}