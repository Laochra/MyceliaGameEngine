#pragma once

#include <vector>
#include "MathIncludes.h"

class GameObject;

namespace Updater
{
	extern std::vector<GameObject*> fixedUpdateList;
	extern std::vector<GameObject*> updateList;
	extern std::vector<GameObject*> drawList;

	extern void FixedUpdateAdd(GameObject* gameObject);
	extern void UpdateAdd(GameObject* gameObject);
	extern void DrawAdd(GameObject* gameObject);


	extern void CallFixedUpdate();
	extern void CallUpdate();
	extern void CallDraw();
	extern void CallDrawDepth(mat4 PVMatrix);
}