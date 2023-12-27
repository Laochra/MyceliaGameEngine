#include "UpdateList.h"

#include "GameObject.h"

namespace UpdateList
{
	std::vector<GameObject*> updateList;

	void Subscribe(GameObject* gameObject)
	{
		updateList.push_back(gameObject);
	}
	void Unsubscribe(GameObject* gameObject)
	{
		std::remove(updateList.begin(), updateList.end(), gameObject);
	}

	void OnFrameStart()
	{
		for (GameObject* gameObject : updateList)
		{
			if (gameObject->GetActive()) gameObject->OnFrameStart();
		}
	}
	void FixedUpdate()
	{
		for (GameObject* gameObject : updateList)
		{
			if (gameObject->GetActive()) gameObject->FixedUpdate();
		}
	}
	void Update()
	{
		for (GameObject* gameObject : updateList)
		{
			if (gameObject->GetActive()) gameObject->Update();
		}
	}
	void Draw()
	{
		for (GameObject* gameObject : updateList)
		{
			if (gameObject->GetActive()) gameObject->Draw();
		}
	}
}