#include "GameObjectManager.h"

//#include "GameObject2D.h"
#include "GameObject3D.h"

GameObjectManager* gameObjectManager;

GameObjectManager::~GameObjectManager() noexcept
{
	for (GameObject* gameObject : gameObjects)
	{
		DeleteGameObject(gameObject);
	}
	//for (GameObject* gameObject2D : gameObject2Ds)
	//{
	//	DeleteGameObject(gameObject2D);
	//}
	for (GameObject* gameObject3D : gameObject3Ds)
	{
		DeleteGameObject(gameObject3D);
	}
	for (GameObject* gameObject : graveyard)
	{
		DeleteGameObject(gameObject);
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

//void GameObjectManager::Add(GameObject2D& gameObject2D) noexcept
//{
//	gameObject2Ds.push_back(gameObject2D);
//}

//void GameObjectManager::Remove(GameObject2D& gameObject2D) noexcept
//{
//	for (int i = 0; i < gameObject2Ds.size(); i++)
//	{
//		if (gameObject2Ds[i] == gameObject2D)
//		{
//			gameObject2Ds.erase(gameObject2Ds.begin() + i);
//			break;
//		}
//	}
//}

void GameObjectManager::Add(GameObject3D* gameObject3D) noexcept
{
	gameObject3Ds.push_back(gameObject3D);
}

void GameObjectManager::Remove(GameObject3D* gameObject3D) noexcept
{
	for (int i = 0; i < gameObject3Ds.size(); i++)
	{
		if (*gameObject3Ds[i] == *gameObject3D)
		{
			gameObject3Ds.erase(gameObject3Ds.begin() + i);
			break;
		}
	}
}

void GameObjectManager::Bury(GameObject* gameObject) noexcept
{
	graveyard.push_back(gameObject);
}

void GameObjectManager::DeleteGameObject(GameObject* gameObject)
{
	if (*gameObject != GameObject::Destroyed)
	{
		GameObject::Destroy<GameObject>(gameObject);
	}
	delete gameObject;
}
//void GameObjectManager::DeleteGameObject(GameObject2D* gameObject2D)
//{
// if (*gameObject2D != GameObject::Destroyed)
//	{
//		GameObject::Destroy<GameObject2D>(gameObject2D);
//	}
// 
//	for (GameObject2D* child : gameObject2D->children)
//	{
//		DeleteGameObject(child);
//	}
//	
//	delete gameObject3D;
//}
void GameObjectManager::DeleteGameObject(GameObject3D* gameObject3D)
{
	if (*gameObject3D != GameObject::Destroyed)
	{
		GameObject::Destroy<GameObject3D>(gameObject3D);
	}

	for (GameObject3D* child : *gameObject3D->GetChildren())
	{
		DeleteGameObject(child);
	}

	delete gameObject3D;
}