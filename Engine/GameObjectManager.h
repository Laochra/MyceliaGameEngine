#pragma once

#include <vector>
using std::vector;

class GameObject;
class GameObject3D;
class GameObjectFolder;

class GameObjectManager
{
public:
	//GameObject2D* root2D;
	GameObject3D* root3D;

	GameObjectFolder* other;

	GameObjectManager() noexcept;
	~GameObjectManager() noexcept;
	GameObjectManager(const GameObjectManager&) noexcept = delete;
	GameObjectManager& operator=(const GameObjectManager&) noexcept = delete;

	void Bury(GameObject* gameObject) noexcept;

private:
	vector<GameObject*> graveyard = vector<GameObject*>();
	void Delete(GameObject* gameObject);
};

extern GameObjectManager* gameObjectManager;