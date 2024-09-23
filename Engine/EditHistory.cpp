#include "EditHistory.h"

#include "GameObject3D.h"

#pragma region struct EditGameObject
	EditHistory::EditGameObject::EditGameObject(GameObject* gameObjectInit) noexcept :
		gameObjectEditRef(gameObjectInit)
	{
		if (gameObjectEditRef != nullptr)
		{
			gameObjectEditRef->SerialiseTo(before);

			const GameObject3D* gameObject3D = dynamic_cast<GameObject3D*>(gameObjectEditRef);
			if (gameObject3D != nullptr)
			{
				const GameObject3D* parent = gameObject3D->GetParent();
				parentingData.oldParentGUID = parent == nullptr ? 0 : parent->GetGUID();
				parentingData.oldIndex = gameObject3D->GetIndex();
			}
		}
	}
	bool EditHistory::EditGameObject::Finalise() noexcept
	{
		if (gameObjectEditRef == nullptr) // This isn't checking for deleted GameObject, its checking for uninitialised GameObject
		{
			Debug::LogWarning("EditHistory::Begin/End(GameObject*) failed because GameObject* was nullptr");
			return false;
		}

		if (gameObjectEditRef != GameObject::Destroyed)
		{
			gameObjectEditRef->SerialiseTo(after);

			const GameObject3D* gameObject3D = dynamic_cast<GameObject3D*>(gameObjectEditRef);
			if (gameObject3D != nullptr)
			{
				const GameObject3D* parent = gameObject3D->GetParent();
				parentingData.newParentGUID = parent == nullptr ? 0 : parent->GetGUID();
				parentingData.newIndex = gameObject3D->GetIndex();
			}
		}

		bool noChange = (
			!before.empty() &&
			!after.empty() &&
			before == after &&
			parentingData.oldParentGUID == parentingData.newParentGUID &&
			parentingData.oldIndex == parentingData.newIndex
		);

		guid = gameObjectEditRef->GetGUID(); // From this point gameObjectEditRef becomes invalid and guid becomes valid

		return !noChange;
	}

	void EditHistory::EditGameObject::Undo() noexcept
	{
		GameObject* gameObject = gameObjectManager->Find(guid);
		if (gameObject == nullptr) gameObject = gameObjectManager->FindInGraveyard(guid);

		if (before.empty()) // Object was instantiated
		{
			GameObject::Destroy(gameObject);
		}
		else
		{
			if (after.empty()) // Object was destroyed
			{
				GameObject::Restore(gameObject);
				gameObject->UpdateFrom(before, GuidGeneration::File);
			}
			else
			{
				gameObject->UpdateFrom(before, GuidGeneration::File);
			}

			GameObject3D* gameObject3D = dynamic_cast<GameObject3D*>(gameObject);
			if (gameObject3D != nullptr)
			{
				const GameObject3D* currentParent = gameObject3D->GetParent();
				bool parentChanged = false;
				if (currentParent == nullptr)
				{
					if (parentingData.oldParentGUID != 0) parentChanged = true;
				}
				else
				{
					if (currentParent->GetGUID() != parentingData.oldParentGUID) parentChanged = true;
				}

				if (parentChanged)
				{
					gameObject3D->SetParent((GameObject3D*)gameObjectManager->Find(parentingData.oldParentGUID));
					gameObject3D->MoveTo((int)parentingData.oldIndex);
				}
				else if (gameObject3D->GetIndex() != parentingData.oldIndex)
				{
					gameObject3D->MoveTo((int)parentingData.oldIndex);
				}
			}
		}
	}
	void EditHistory::EditGameObject::Redo() noexcept
	{
		GameObject* gameObject = gameObjectManager->Find(guid);
		if (gameObject == nullptr) gameObject = gameObjectManager->FindInGraveyard(guid);

		if (after.empty()) // Object was destroyed
		{
			GameObject::Destroy(gameObject);
		}
		else
		{
			if (before.empty()) // Object was instantiated
			{
				GameObject::Restore(gameObject);
				gameObject->UpdateFrom(after, GuidGeneration::File);
			}
			else
			{
				gameObject->UpdateFrom(after, GuidGeneration::File);
			}

			GameObject3D* gameObject3D = dynamic_cast<GameObject3D*>(gameObject);
			if (gameObject3D != nullptr)
			{
				const GameObject3D* currentParent = gameObject3D->GetParent();
				bool parentChanged = false;
				if (currentParent == nullptr)
				{
					if (parentingData.newParentGUID != 0) parentChanged = true;
				}
				else
				{
					if (currentParent->GetGUID() != parentingData.newParentGUID) parentChanged = true;
				}

				if (parentChanged)
				{
					gameObject3D->SetParent((GameObject3D*)gameObjectManager->Find(parentingData.newParentGUID));
					gameObject3D->MoveTo((int)parentingData.newIndex);
				}
				else if (gameObject3D->GetIndex() != parentingData.newIndex)
				{
					gameObject3D->MoveTo((int)parentingData.newIndex);
				}
			}
		}
	}
#pragma endregion

void EditHistory::End() noexcept
{
	if (currentEdit == nullptr)
	{
		Debug::LogWarning("EditHistory::Begin/End() Mismatch! Begin an edit before trying to end it.", locationinfo);
		return;
	}

	if (CanRedo())
	{
		for (ulong i = currentStackIndex; i < editStack.size(); i++)
		{
			delete editStack[i];
		}
		editStack.erase(editStack.begin() + currentStackIndex, editStack.end());
	}

	if (currentEdit->Finalise())
	{
		editStack.push_back(currentEdit);
		currentStackIndex++;
	}
	else delete currentEdit;
	currentEdit = nullptr;
}

void EditHistory::Clear() noexcept
{
	editStack.clear();
}

bool EditHistory::CanUndo() noexcept
{
	return currentStackIndex > 0Ui64;
}
bool EditHistory::CanRedo() noexcept
{
	return currentStackIndex < editStack.size();
}

void EditHistory::Undo() noexcept
{
	assert(CanUndo());
	currentStackIndex--;
	editStack[currentStackIndex]->Undo();
}
void EditHistory::Redo() noexcept
{
	assert(CanRedo());
	editStack[currentStackIndex]->Redo();
	currentStackIndex++;
}
