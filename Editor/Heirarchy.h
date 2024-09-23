#pragma once

#include "GUI.h"

class GameObject;
class UISprite;
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
		InspectableObject* target = nullptr;

		void Open(InspectableObject* targetInit, ImVec2 positionInit);

		void Close();
	};

	extern void Draw(const char* const name, bool& open);
	extern void DrawEntry(GameObject3D* gameObject3D);
	extern void DrawEntry(UISprite* uiSprite, int index);
	extern void DragDropTarget(const char* id, GameObject3D* target, DragDropTargetOffset offset = ChildTo);
	extern void BeginPayload(GameObject3D* gameObject3D);
	extern void AcceptInputFor(InspectableObject* object);
	extern void Duplicate(GameObject3D* gameObject3D) noexcept;
}
