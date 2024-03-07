#include "GameObjectFolder.h"

void GameObjectFolder::OnDestroy()
{
	for (GameObject* child : children)
	{
		GameObject::Destroy(child);
	}

	if (parent != nullptr) SetParent(nullptr);

	GameObject::OnDestroy();
}