#include "GameObjectManager.h"

#include "GameObject.h"
//#include "GameObject2D.h"
#include "GameObject3D.h"

#include "GameObjectFolder.h"

#include "MemoryManagement.h"

GameObjectManager* gameObjectManager;

GameObjectManager::GameObjectManager() noexcept
{
	//root2D = new GameObject2D;
	root3D = new GameObject3D;

	other = new GameObjectFolder;
}

GameObjectManager::~GameObjectManager() noexcept
{
	//GameObject::Destroy(root2D);
	GameObject::Destroy(root3D);

	for (int i = 0; i < graveyard.size(); i++)
	{
		Delete(graveyard[i]);
		graveyard.erase(graveyard.begin() + i);
	}
}

void GameObjectManager::Bury(GameObject* gameObject) noexcept
{
	graveyard.push_back(gameObject);
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