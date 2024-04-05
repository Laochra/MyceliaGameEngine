#include "GameObject.h"

#include "GeneralMacros.h"

GameObject::GameObjectState GameObject::GetState() const noexcept
{
	return state;
}

bool GameObject::IsActive() noexcept
{
	return *this == Active;
}

void GameObject::SetState(GameObjectState value) noexcept
{
	if (state == Destroyed || value == Destroyed) return;
	
	state = value;
	
	if (value == Active) OnActivate();
	else OnDeactivate();
}

unsigned long long int GameObject::GetGUID() const noexcept
{
	return guid;
}

const char* GameObject::GetName() const noexcept
{
	return name;
}

void GameObject::SetName(const char* newName) noexcept
{
	del(name);
	unsigned int newNameLength = strlen(newName);
	name = new char[newNameLength + 1];
	name[newNameLength] = '\0';
	memcpy(name, newName, newNameLength);
}

bool GameObject::operator==(GameObject& other) const noexcept
{
	return guid == other.guid;
}

bool GameObject::operator!=(GameObject& other) const noexcept
{
	return guid != other.guid;
}

bool GameObject::operator==(GameObjectState stateToCheck) const noexcept
{
	return state == stateToCheck;
}

bool GameObject::operator!=(GameObjectState stateToCheck) const noexcept
{
	return state != stateToCheck;
}



void GameObject::OnFrameStart()
{

}
void GameObject::FixedUpdate()
{
	
}
void GameObject::Update()
{

}
void GameObject::Draw()
{

}

void GameObject::Initialise()
{
	Updater::UpdateAdd(this);
}
void GameObject::OnDestroy()
{
	gameObjectManager->Remove(this);
	gameObjectManager->Bury(this);
}
void GameObject::OnActivate()
{

}
void GameObject::OnDeactivate()
{

}