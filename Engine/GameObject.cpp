#include "GameObject.h"
#include "GameObject3D.h"
#include "MeshRenderer.h"
#include "LightObject.h"
#include "ParticleEmitter.h"

#include "GeneralMacros.h"

#include "Debug.h"

GameObject* GameObject::InstantiateFrom(json jsonObj, GuidGeneration guidOptions) noexcept
{
	GameObject* gameObject;
	const unsigned long long typeID = jsonObj["TypeID"];
	switch (typeID)
	{
	case GameObject::classID: gameObject = new GameObject(); break;
	case GameObject3D::classID: gameObject = new GameObject3D(); break;
	case MeshRenderer::classID: gameObject = new MeshRenderer(); break;
	case LightObject::classID: gameObject = new LightObject(); break;
	case ParticleEmitter::classID: gameObject = new ParticleEmitter(); break;
	default:
		gameObject = new GameObject();
		Debug::LogError(LogID::ERR151, "'", typeID, "' DO NOT SAVE! Data will be lost. ", locationinfo);
		break;
	}

	gameObject->DeserialiseFrom(jsonObj, guidOptions);

	gameObject->Initialise();

	return gameObject;
}

void GameObject::SerialiseTo(json& jsonObj) const
{
	jsonObj["Name"] = name;
	jsonObj["GUID"] = guid;
	jsonObj["Active"] = state == Active;
	jsonObj["TypeID"] = GetClassID();
}
void GameObject::DeserialiseFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	string nameStr = string(jsonObj["Name"]);
	name = new char[nameStr.size() + 1];
	memcpy(name, nameStr.c_str(), nameStr.size() + 1);

	if (guidOptions == GuidGeneration::Keep) guid = jsonObj["GUID"];
	else guid = GuidGenerator::NewGuid();

	state = bool(jsonObj["Active"]) ? GameObject::Active : GameObject::Inactive;
}

GameObject::GameObjectState GameObject::GetState() const noexcept
{
	return state;
}

bool GameObject::IsActive() noexcept
{
	return this == Active;
}

void GameObject::SetState(GameObjectState newState) noexcept
{
	if (state == Destroyed || newState == Destroyed) return;
	
	state = newState;
	
	if (newState == Active) OnActivate();
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
	unsigned int newNameLength = (unsigned int)strlen(newName);
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

void GameObject::DrawDepth(mat4 PVMatrix)
{

}

void GameObject::DrawGUID()
{

}

void GameObject::DrawDebug()
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

void GameObject::Destroy(GameObject* gameObject)
{
	if (gameObject == nullptr) return;
	if (gameObject == Destroyed) return;

	gameObject->OnDestroy();

	gameObject->state = Destroyed;
}

GameObject* GameObject::RestoreFrom(GameObject* gameObject, json jsonObj, GuidGeneration guidOptions) noexcept
{
	const unsigned long long typeID = jsonObj["TypeID"];
	if (typeID != gameObject->GetClassID())
	{
		Debug::LogError(LogID::ERR150, "TypeID of restored GameObject does not match the serialised file. ", locationinfo);
		return gameObject;
	}

	gameObjectManager->Unbury(gameObject);

	gameObject->DeserialiseFrom(jsonObj, guidOptions);

	gameObject->Initialise();

	return gameObject;
}

void GameObject::Restore(GameObject* gameObject, GameObjectState stateInit)
{
	if (gameObject == nullptr) return;
	if (gameObject != Destroyed) return;

	gameObjectManager->Unbury(gameObject);

	gameObject->state = stateInit;
}

bool operator==(GameObject* gameObject, GameObject::GameObjectState state) noexcept
{
	return gameObject->state == state;
}
bool operator!=(GameObject* gameObject, GameObject::GameObjectState state) noexcept
{
	return gameObject->state != state;
}

// Friends
void to_json(json& jsonObject, const GameObject* gameObject) noexcept
{
	gameObject->SerialiseTo(jsonObject);
}
void from_json(const json& jsonObject, GameObject* gameObject) noexcept
{
	gameObject->DeserialiseFrom(jsonObject);
}