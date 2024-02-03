#pragma once

#include <vector>
using std::vector;

class GameObject;

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
	void Bury(GameObject* gameObject) noexcept;
	void Move(GameObject* gameObject, int newIndex) noexcept;

	int GetIndexOf(GameObject* gameObject) const noexcept;

private:
	vector<GameObject*> graveyard = vector<GameObject*>();

	void Delete(GameObject* gameObject);
};

extern GameObjectManager* gameObjectManager;