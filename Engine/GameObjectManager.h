#pragma once

#include <vector>
using std::vector;

class GameObject;
class GameObject3D;

class GameObjectManager
{
public:
	GameObjectManager() noexcept = default;
	~GameObjectManager() noexcept;
	GameObjectManager(const GameObjectManager&) noexcept = delete;
	GameObjectManager& operator=(const GameObjectManager&) noexcept = delete;

	//GameObject2D* root2D;
	//GameObject3D* root3D;

	vector<GameObject*> gameObjects = vector<GameObject*>();
	void Add(GameObject* gameObject) noexcept;
	void Remove(GameObject* gameObject) noexcept;
	void Bury(GameObject* gameObject) noexcept;
	void Move(GameObject* gameObject, int newIndex) noexcept;

	GameObject* Find(unsigned long long guid) const noexcept;
	GameObject3D* FindChild(const GameObject3D* gameObject, unsigned long long guid) const noexcept;

	int GetIndexOf(GameObject* gameObject) const noexcept;

	void Clear() noexcept;

private:
	vector<GameObject*> graveyard = vector<GameObject*>();

	void Delete(GameObject* gameObject);
};

extern GameObjectManager* gameObjectManager;