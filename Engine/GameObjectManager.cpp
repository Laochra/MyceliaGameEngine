#include "GameObjectManager.h"

//#include "GameObject2D.h"
#include "GameObject3D.h"
#include "MeshRenderer.h"
#include "LightObject.h"
#include "Camera.h"

#include "GeneralMacros.h"

#include "Debug.h"

GameObjectManager* gameObjectManager;

GameObjectManager::~GameObjectManager() noexcept
{
	Clear();
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

	char direction = (i < newIndex) ? 1 : -1;

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

	if (gameObject != GameObject::Destroyed)
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

void GameObjectManager::Clear() noexcept
{
	Updater::fixedUpdateList.clear();
	Updater::updateList.clear();
	Updater::drawList.clear();

	if (mainCamera != nullptr) Updater::updateList.push_back(mainCamera);

	while (gameObjects.size() > 0)
	{
		GameObject::Destroy(gameObjects[0]);
	}
	while (graveyard.size() > 0)
	{
		Delete(graveyard[0]);
		graveyard.erase(graveyard.begin() + 0);
	}
}
