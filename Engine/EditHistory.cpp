#include "EditHistory.h"

EditHistory::Edit* EditHistory::currentWatch = nullptr;
vector<EditHistory::Edit*> EditHistory::historyStack = vector<EditHistory::Edit*>();
ulong EditHistory::currentStackIndex = 0Ui64;

#include "GameObject3D.h"

#pragma region struct EditGameObject
	EditHistory::EditGameObject::EditGameObject(GameObject*& gameObjectInit) noexcept :
		gameObject(gameObjectInit)
	{
		if (gameObject != nullptr)
		{
			before = new json();
			gameObject->SerialiseTo(*before);

			if (dynamic_cast<GameObject3D*>(gameObject) != nullptr)
			{
				parentingData.oldParent = ((GameObject3D*)gameObject)->GetParent();
				parentingData.oldIndex = ((GameObject3D*)gameObject)->GetIndex();
			}
		}
	}
	EditHistory::EditGameObject::~EditGameObject() noexcept
	{
		if (before == nullptr) delete before;
		if (after == nullptr) delete after;
	}
	bool EditHistory::EditGameObject::Finalise() noexcept
	{
		if (gameObject == nullptr) // This isn't checking for deleted GameObject, its checking for uninitialised GameObject
		{
			Debug::LogWarning("EditHistory::Begin/End(GameObject*) failed because GameObject* was nullptr");
			return false;
		}

		if (gameObject != GameObject::Destroyed)
		{
			after = new json();
			gameObject->SerialiseTo(*after);

			if (dynamic_cast<GameObject3D*>(gameObject) != nullptr)
			{
				parentingData.newParent = ((GameObject3D*)gameObject)->GetParent();
				parentingData.newIndex = ((GameObject3D*)gameObject)->GetIndex();
			}
		}
		

		bool noChange = (
			before != nullptr &&
			after != nullptr &&
			*before == *after &&
			parentingData.oldParent == parentingData.newParent &&
			parentingData.oldIndex == parentingData.newIndex
		);

		return !noChange;
	}

	void EditHistory::EditGameObject::Undo() noexcept
	{
		if (before == nullptr) // Object was instantiated
		{
			GameObject::Destroy(gameObject);
		}
		else
		{
			if (after == nullptr) // Object was destroyed
			{
				GameObject::RestoreFrom(gameObject, *before, GuidGeneration::Keep);
			}
			else
			{
				gameObject->DeserialiseFrom(*before, GuidGeneration::Keep);
			}

			GameObject3D* gameObject3D = dynamic_cast<GameObject3D*>(gameObject);
			if (gameObject3D != nullptr)
			{
				if (gameObject3D->GetParent() != parentingData.oldParent)
				{
					gameObject3D->SetParent(parentingData.oldParent);
					gameObject3D->MoveTo(parentingData.oldIndex);
				}
				else if (gameObject3D->GetIndex() != parentingData.oldIndex)
				{
					gameObject3D->MoveTo(parentingData.oldIndex);
				}
			}
		}
	}
	void EditHistory::EditGameObject::Redo() noexcept
	{
		if (after == nullptr) // Object was destroyed
		{
			GameObject::Destroy(gameObject);
		}
		else
		{
			if (before == nullptr) // Object was instantiated
			{
				GameObject::RestoreFrom(gameObject, *after, GuidGeneration::Keep);
			}
			else
			{
				gameObject->DeserialiseFrom(*after, GuidGeneration::Keep);
			}

			GameObject3D* gameObject3D = dynamic_cast<GameObject3D*>(gameObject);
			if (gameObject3D != nullptr)
			{
				if (gameObject3D->GetParent() != parentingData.newParent)
				{
					gameObject3D->SetParent(parentingData.newParent);
					gameObject3D->MoveTo(parentingData.newIndex);
				}
				else if (gameObject3D->GetIndex() != parentingData.newIndex)
				{
					gameObject3D->MoveTo(parentingData.newIndex);
				}
			}
		}
	}
#pragma endregion

void EditHistory::End() noexcept
{
	if (currentWatch == nullptr)
	{
		Debug::LogWarning("EditHistory::Begin/End() Mismatch! Begin an edit before trying to end it.", locationinfo);
		return;
	}

	if (CanRedo())
	{
		for (ulong i = currentStackIndex; i < historyStack.size(); i++)
		{
			delete historyStack[i];
		}
		historyStack.erase(historyStack.begin() + currentStackIndex, historyStack.end());
	}

	if (currentWatch->Finalise())
	{
		historyStack.push_back(currentWatch);
		currentStackIndex++;
	}
	else delete currentWatch;
	currentWatch = nullptr;
}

bool EditHistory::CanUndo() noexcept
{
	return currentStackIndex > 0Ui64;
}
bool EditHistory::CanRedo() noexcept
{
	return currentStackIndex < historyStack.size();
}

void EditHistory::Undo() noexcept
{
	assert(CanUndo());
	currentStackIndex--;
	historyStack[currentStackIndex]->Undo();
}
void EditHistory::Redo() noexcept
{
	assert(CanRedo());
	historyStack[currentStackIndex]->Redo();
	currentStackIndex++;
}
