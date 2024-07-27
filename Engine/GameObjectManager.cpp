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
void GameObjectManager::Unbury(GameObject* gameObject) noexcept
{
	for (int i = 0; i < graveyard.size(); i++)
	{
		if (*graveyard[i] == *gameObject)
		{
			graveyard.erase(graveyard.begin() + i);
			break;
		}
	}
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
GameObject* GameObjectManager::Find(unsigned long long guid) const noexcept
{
	for (int c = 0; c < gameObjects.size(); c++)
	{
		if (gameObjects[c]->guid == guid) return gameObjects[c];

		if (dynamic_cast<GameObject3D*>(gameObjects[c]) != nullptr)
		{
			GameObject3D* potentialMatch = FindChild((GameObject3D*)gameObjects[c], guid);
			if (potentialMatch != nullptr) return potentialMatch;
		}
	}
	return nullptr;
}
GameObject* GameObjectManager::FindInGraveyard(unsigned long long guid) const noexcept
{
	for (int c = 0; c < graveyard.size(); c++)
	{
		if (graveyard[c]->guid == guid) return graveyard[c];

		if (dynamic_cast<GameObject3D*>(graveyard[c]) != nullptr)
		{
			GameObject3D* potentialMatch = FindChild((GameObject3D*)graveyard[c], guid);
			if (potentialMatch != nullptr) return potentialMatch;
		}
	}
	return nullptr;
}
GameObject3D* GameObjectManager::FindChild(const GameObject3D* gameObject, unsigned long long guid) const noexcept
{
	const std::vector<GameObject3D*>& children = *gameObject->GetChildren();

	for (int c = 0; c < children.size(); c++)
	{
		if (children[c]->guid == guid) return children[c];
		GameObject3D* potentialMatch = FindChild(children[c], guid);
		if (potentialMatch != nullptr) return potentialMatch;
	}
	return nullptr;
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

	if (Camera::main != nullptr) Updater::updateList.push_back(Camera::main);

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
