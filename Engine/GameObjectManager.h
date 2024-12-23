#pragma once

#include <vector>
using std::vector;

class GameObject;
class GameObject3D;

typedef unsigned int uint;

class GameObjectManager
{
public:
	GameObjectManager() noexcept = default;
	~GameObjectManager() noexcept;
	GameObjectManager(const GameObjectManager&) noexcept = delete;
	GameObjectManager& operator=(const GameObjectManager&) noexcept = delete;

	//GameObject2D* root2D;
	GameObject3D* root3D;

	vector<GameObject*> gameObjects = vector<GameObject*>();
	void Add(GameObject* gameObject) noexcept;
	void Remove(GameObject* gameObject) noexcept;
	void Bury(GameObject* gameObject) noexcept;
	void Unbury(GameObject* gameObject) noexcept;
	void Move(GameObject* gameObject, int newIndex) noexcept;
	void Delete(GameObject* gameObject);

	GameObject* Find(unsigned long long guid) const noexcept;
	GameObject* FindInGraveyard(unsigned long long guid) const noexcept;
	GameObject3D* FindChild(const GameObject3D* gameObject, unsigned long long guid) const noexcept;

	int GetIndexOf(GameObject* gameObject) const noexcept;

	void Clear() noexcept;

	typedef void(*DrawFunc)() noexcept;
	static void DrawGUIDs(uint& framebuffer, uint& texture, uint& depth, DrawFunc drawTransformIDsFunction = nullptr) noexcept;
	static void DrawScene(uint& framebuffer, uint& texture, uint& depth, uint& bloomTexture, uint& gizmosTexture, DrawFunc drawTransformsFunction = nullptr) noexcept;

private:
	vector<GameObject*> graveyard = vector<GameObject*>();

};

extern GameObjectManager* gameObjectManager;