#pragma once

#include "GUI.h"

class GameObject;
//class GameObject2D;
class GameObject3D;

namespace Heirarchy
{
	enum DragDropTargetOffset : char
	{
		ChildToFront = -2,
		ChildTo = -1,
		Before = 0,
		After = 1
	};

	struct RightClickMenu
	{
		bool open = false;
		ImVec2 position = ImVec2(0.0f, 0.0f);
		GameObject* target = nullptr;

		void Open(GameObject* targetInit, ImVec2 positionInit);

		void Close();
	};

	extern void Draw();
	extern void DrawEntry(GameObject3D* gameObject3D);
	extern void DragDropTarget(const char* id, GameObject3D* target, DragDropTargetOffset offset = ChildTo);
	extern void AcceptInputFor(GameObject3D* gameObject3D);
}
