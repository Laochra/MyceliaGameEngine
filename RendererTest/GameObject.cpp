#include "GameObject.h"

#include <combaseapi.h>

#include "UpdateList.h"

GameObject::GameObject()
{
	GUID newGuid;
	CoCreateGuid(&newGuid);
	guid = newGuid.Data1;

	UpdateList::Subscribe(this);

	Initialise();
}
GameObject::~GameObject()
{
	UpdateList::Unsubscribe(this);

	OnDestroy();
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

}
void GameObject::OnDestroy()
{

}
void GameObject::OnActivate()
{

}
void GameObject::OnDeactivate()
{

}