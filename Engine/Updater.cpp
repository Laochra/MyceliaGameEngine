#include "Updater.h"

#include "GameObject.h"

namespace Updater
{
	std::vector<GameObject*> onFrameStartList;
	std::vector<GameObject*> fixedUpdateList;
	std::vector<GameObject*> updateList;
	std::vector<GameObject*> drawList;

	void OnFrameStartAdd(GameObject* gameObject)
	{
		onFrameStartList.push_back(gameObject);
	}

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


	void CallOnFrameStart()
	{
		int i = 0;
		while (i < onFrameStartList.size())
		{
			if (onFrameStartList[i]->IsActive())
			{
				onFrameStartList[i]->OnFrameStart();
			}
			else if (*onFrameStartList[i] == GameObject::Destroyed)
			{
				onFrameStartList.erase(onFrameStartList.begin() + i);
				continue;
			}

			i++;
		}
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
			else if (*fixedUpdateList[i] == GameObject::Destroyed)
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
			else if (*updateList[i] == GameObject::Destroyed)
			{
				updateList.erase(updateList.begin() + i);
				continue;
			}

			i++;
		}
	}
	void CallDraw()
	{
		int i = 0;
		while (i < drawList.size())
		{
			if (drawList[i]->IsActive())
			{
				drawList[i]->Draw();
			}
			else if (*drawList[i] == GameObject::Destroyed)
			{
				drawList.erase(drawList.begin() + i);
				continue;
			}

			i++;
		}
	}
}