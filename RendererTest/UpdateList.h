#pragma once

#include <vector>

class GameObject;

namespace UpdateList
{
	extern std::vector<GameObject*> updateList;

	extern void Subscribe(GameObject* gameObject);
	extern void Unsubscribe(GameObject* gameObject);

	extern void OnFrameStart();
	extern void FixedUpdate();
	extern void Update();
	extern void Draw();
}