#pragma once

class GameObject;
//class GameObject2D;
class GameObject3D;

#include <vector>
using std::vector;

class GameObjectManager
{
public:
	GameObjectManager() noexcept = default;
	~GameObjectManager() noexcept;
	GameObjectManager(const GameObjectManager&) noexcept = delete;
	GameObjectManager& operator=(const GameObjectManager&) noexcept = delete;

	vector<GameObject*> gameObjects = vector<GameObject*>();
	void Add(GameObject* gameObject) noexcept;
	void Remove(GameObject* gameObject) noexcept;

	//vector<GameObject2D*> gameObject2Ds = vector<GameObject2D*>();
	//void Add(GameObject2D* gameObject2D) noexcept;
	//void Remove(GameObject2D* gameObject2D) noexcept;

	vector<GameObject3D*> gameObject3Ds = vector<GameObject3D*>();
	void Add(GameObject3D* gameObject3D) noexcept;
	void Remove(GameObject3D* gameObject3D) noexcept;

	void Bury(GameObject* gameObject) noexcept;

private:
	vector<GameObject*> graveyard = vector<GameObject*>();

	void DeleteGameObject(GameObject*);
	//void DeleteGameObject(GameObject2D*);
	void DeleteGameObject(GameObject3D*);
};

extern GameObjectManager* gameObjectManager;