#include "Updater.h"

#include "GameObject.h"
#include "MeshRenderer.h"

namespace Updater
{
	std::vector<GameObject*> fixedUpdateList;
	std::vector<GameObject*> updateList;
	std::vector<GameObject*> drawList;

	void FixedUpdateAdd(GameObject* gameObject)
	{
		fixedUpdateList.push_back(gameObject);
	}

	void UpdateAdd(GameObject* gameObject)
	{
		updateList.push_back(gameObject);
	}

	void DrawAdd(GameObject* gameObject)
	{
		drawList.push_back(gameObject);
	}

	void FixedUpdateRemove(GameObject* gameObject)
	{
		std::vector<GameObject*>::iterator it = std::find(fixedUpdateList.begin(), fixedUpdateList.end(), gameObject);
		if (it != fixedUpdateList.end()) fixedUpdateList.erase(it);
	}

	void UpdateRemove(GameObject* gameObject)
	{
		std::vector<GameObject*>::iterator it = std::find(updateList.begin(), updateList.end(), gameObject);
		if (it != updateList.end()) updateList.erase(it);
	}

	void DrawRemove(GameObject* gameObject)
	{
		std::vector<GameObject*>::iterator it = std::find(drawList.begin(), drawList.end(), gameObject);
		if (it != drawList.end()) drawList.erase(it);
	}


	void CallFixedUpdate()
	{
		int i = 0;
		while (i < fixedUpdateList.size())
		{
			if (fixedUpdateList[i]->IsActive())
			{
				fixedUpdateList[i]->FixedUpdate();
			}
			else if (fixedUpdateList[i] == GameObject::Destroyed)
			{
				fixedUpdateList.erase(fixedUpdateList.begin() + i);
				continue;
			}

			i++;
		}
	}
	void CallUpdate()
	{
		int i = 0;
		while (i < updateList.size())
		{
			if (updateList[i]->IsActive())
			{
				updateList[i]->Update();
			}
			else if (updateList[i] == GameObject::Destroyed)
			{
				updateList.erase(updateList.begin() + i);
				continue;
			}

			i++;
		}
	}
	static bool ShouldSwapDrawnObjects(const GameObject* a, const GameObject* b)
	{
		if (b->GetClassID() != MeshRenderer::classID) return false;
		if (a->GetClassID() != MeshRenderer::classID) return true;

		return (intptr_t)((MeshRenderer*)a)->GetMaterial() < (intptr_t)((MeshRenderer*)b)->GetMaterial();
	}
	void CallDraw()
	{
		std::sort(drawList.begin(), drawList.end(), ShouldSwapDrawnObjects);

		int i = 0;
		const Material* lastUsedMaterial = nullptr;
		while (i < drawList.size())
		{
			if (drawList[i]->IsActive())
			{
				drawList[i]->Draw((intptr_t)lastUsedMaterial);
				if (drawList[i]->GetClassID() == MeshRenderer::classID)
				{
					lastUsedMaterial = ((MeshRenderer*)drawList[i])->GetMaterial();
				}
			}
			else if (drawList[i] == GameObject::Destroyed)
			{
				drawList.erase(drawList.begin() + i);
				continue;
			}

			i++;
		}
	}
	void CallDrawDepth(mat4 PVMatrix)
	{
		int i = 0;
		while (i < drawList.size())
		{
			if (drawList[i]->IsActive())
			{
				drawList[i]->DrawDepth(PVMatrix);
			}
			else if (drawList[i] == GameObject::Destroyed)
			{
				drawList.erase(drawList.begin() + i);
				continue;
			}

			i++;
		}
	}
	void CallDrawGUIDs()
	{
		int i = 0;
		while (i < drawList.size())
		{
			if (drawList[i]->IsActive())
			{
				drawList[i]->DrawGUID();
			}
			else if (drawList[i] == GameObject::Destroyed)
			{
				drawList.erase(drawList.begin() + i);
				continue;
			}

			i++;
		}
	}
}