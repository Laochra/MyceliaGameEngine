#include "GameObject.h"
#include "GameObject3D.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "LightObject.h"
#include "ParticleEmitter.h"
#include "LinkedHexGrid.h"

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
	case Camera::classID: gameObject = new Camera(); break;
	case MeshRenderer::classID: gameObject = new MeshRenderer(); break;
	case LightObject::classID: gameObject = new LightObject(); break;
	case ParticleEmitter::classID: gameObject = new ParticleEmitter(); break;
	case LinkedHexGrid::classID: gameObject = new LinkedHexGrid(); break;
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
	SetName(nameStr.c_str());

	if (guidOptions == GuidGeneration::File) guid = jsonObj["GUID"];
	else guid = GuidGenerator::NewGuid();

	state = bool(jsonObj["Active"]) ? GameObject::Active : GameObject::Inactive;
}
bool GameObject::UpdateFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	const unsigned long long typeID = jsonObj["TypeID"];
	if (typeID != GetClassID())
	{
		Debug::LogError(LogID::ERR150, "TypeID of the GameObject does not match the serialised file it is being updated with.", locationinfo);
		
		// Possible way of handling this, it seems like a pain to interface with though
		//if (guidOptions == GuidGeneration::Keep)
		//{
		//	json jsonObjCopy = jsonObj;
		//	jsonObjCopy["GUID"] = GetGUID();
		//	GameObject::InstantiateFrom(jsonObjCopy, GuidGeneration::File);
		//}
		//else
		//{
		//	GameObject::InstantiateFrom(jsonObj, guidOptions);
		//}
		//GameObject::Destroy(this);
		
		return false;
	}

	string nameStr = string(jsonObj["Name"]);
	SetName(nameStr.c_str());
	if (guidOptions == GuidGeneration::File) guid = jsonObj["GUID"];

	state = bool(jsonObj["Active"]) ? GameObject::Active : GameObject::Inactive;

	return true;
}

GameObject::GameObjectState GameObject::GetState() const noexcept
{
	return state;
}

bool GameObject::IsActive() const noexcept
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
	Updater::UpdateRemove(this);
	gameObjectManager->Remove(this);
	gameObjectManager->Bury(this);
}
void GameObject::OnRestore()
{
	GameObject::Initialise();
	gameObjectManager->Unbury(this);
	gameObjectManager->Add(this);
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

	gameObject->state = Destroyed;

	gameObject->OnDestroy();
}

void GameObject::Restore(GameObject* gameObject, GameObjectState stateInit)
{
	if (gameObject == nullptr) return;
	if (gameObject != Destroyed) return;

	gameObject->state = stateInit;
	
	gameObject->OnRestore();
}

bool operator==(const GameObject* gameObject, GameObject::GameObjectState state) noexcept
{
	return gameObject->state == state;
}
bool operator!=(const GameObject* gameObject, GameObject::GameObjectState state) noexcept
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