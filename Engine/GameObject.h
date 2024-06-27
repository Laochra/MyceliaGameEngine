#pragma once

#include "TimeManager.h"
#include "Updater.h"
#include "GuidGenerator.h"

#include "GameObjectManager.h"

#include "StringIncludes.h"

#include <type_traits>

#include "GeneralMacros.h"

#include "json.hpp"
using nlohmann::json;

class GameObject;
template<class T> concept GameObjectClass = std::is_base_of<GameObject, T>::value;

consteval unsigned long long HashIDBits(const char a)
{
	unsigned long long bits;
	switch (a)
	{
	default:							bits = 0b0000; break;
	cases('a', 'A', 'j', 'J')	bits = 0b0001; break;
	cases('c', 'C', 'y', 'Y')	bits = 0b0010; break;
	cases('d', 'D', 'b', 'B')	bits = 0b0011; break;
	cases('e', 'E', 'z', 'Z')	bits = 0b0100; break;
	cases('i', 'I', 'x', 'X')	bits = 0b0101; break;
	cases('l', 'L', 'k', 'K')	bits = 0b0110; break;
	cases('m', 'M') 		 		bits = 0b0111; break;
	cases('n', 'N', 'f', 'F')	bits = 0b1000; break;
	cases('o', 'O', 'v', 'V')	bits = 0b1001; break;
	cases('r', 'R') 		 		bits = 0b1010; break;
	cases('s', 'S', 'w', 'W')	bits = 0b1011; break;
	cases('t', 'T', 'h', 'H')	bits = 0b1100; break;
	cases('u', 'U', 'q', 'Q')	bits = 0b1101; break;
	cases('2', 'g', 'G')			bits = 0b1110; break;
	cases('3', 'p', 'P')			bits = 0b1111; break;
	}

	return bits;
}

consteval unsigned long long HashID(const char* const string)
{
	unsigned long long strLength = 0;
	for (strLength = 0; strLength < 128 && string[strLength] != '\0'; strLength++) { }

	unsigned long long id = 0;
	unsigned long long offset = strLength >= 16 ? strLength - 16 : 0;
	for (unsigned long long i = 0; i < strLength - offset; i++)
	{
		id |= HashIDBits(string[offset + i]) << (4 * i);
	}

	return id;
}

#define SerialiseAs(nameInit) \
static constexpr const char* const className = #nameInit; \
virtual const char* GetClassName() const noexcept { return #nameInit; } \
static constexpr const unsigned long long classID = HashID(#nameInit); \
virtual const unsigned long long GetClassID() const noexcept { return nameInit::classID; } \
virtual void SerialiseTo(json& serialisedObject) const; \
virtual void DeserialiseFrom(const json& serialisedObject, GuidGeneration guidOptions = GuidGeneration::Keep);

enum class GuidGeneration : bool { New = true, Keep = false };

class GameObject
{
public:
	SerialiseAs(GameObject);

	enum GameObjectState : unsigned char
	{
		Active,		// The GameObject can recieve updates
		Inactive,	// The GameObject cannot recieve updates
		Destroyed	// The GameObject has been destroyed and should no longer be referenced
	};

	/// <summary>Check if GameObjects are equal based on their GUIDs</summary> <returns>True if GameObjects are equal</returns>
	bool operator==(GameObject& other) const noexcept;
	/// <summary>Check if GameObjects are equal based on their GUIDs</summary> <returns>False if GameObjects are equal</returns>
	bool operator!=(GameObject& other) const noexcept;
	/// <summary>Check the state of the GameObject</summary> <returns>True if state is equal</returns>
	bool operator==(GameObjectState stateToCheck) const noexcept;
	/// <summary>Check the state of the GameObject</summary> <returns>False if state is equal</returns>
	bool operator!=(GameObjectState stateToCheck) const noexcept;


	/// <returns>The Globally Unique Identifier of the GameObject</returns>
	unsigned long long int GetGUID() const noexcept;
	const char* GetName() const noexcept;
	void SetName(const char* newName) noexcept;
	/// <returns>The current state of the GameObject</returns>
	GameObjectState GetState() const noexcept;
	virtual bool IsActive() noexcept;
	/// <param name="newState">: The state to set the GameObject to</param>
	void SetState(GameObjectState newState) noexcept;
	

	virtual void OnFrameStart();
	virtual void FixedUpdate();
	virtual void Update();
	virtual void Draw();
	virtual void DrawDepth(mat4 PVMatrix);
	virtual void DrawGUID();
	virtual void DrawDebug();
	virtual void Initialise();
	virtual void OnDestroy();


	/// <summary>Creates a new instance of a GameObject</summary> <param name="stateInit">(default: Active)</param> <returns>A pointer to the created instance</returns>
	template<GameObjectClass T> static T* Instantiate(GameObjectState stateInit = Active);
	/// <summary>Creates a new instance of a GameObject by deserialising from json</summary> <returns>A pointer to the created instance</returns>
	static GameObject* InstantiateFrom(json serialisedObject, GuidGeneration guidOptions = GuidGeneration::Keep) noexcept;
	/// <summary>Calls OnDestroy() on the GameObject instance and permanently sets its state to Destroyed</summary> <param name="gameObject"></param>
	static void Destroy(GameObject* gameObject);

protected:
	GameObject() = default;
	~GameObject() = default;
	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;

	virtual void OnActivate();
	virtual void OnDeactivate();

private:
	unsigned long long int guid = 0;
	char* name;
	GameObjectState state = Active;


	// Friends
	friend class GameObjectManager;

	friend bool operator==(GameObject* gameObject, GameObjectState state) noexcept;
	friend bool operator!=(GameObject* gameObject, GameObjectState state) noexcept;
	friend void to_json(json& jsonObject, const GameObject* gameObject) noexcept;
	friend void from_json(const json& jsonObject, GameObject* gameObject) noexcept;
};



template<GameObjectClass T> inline T* GameObject::Instantiate(GameObjectState stateInit)
{
	GameObject* gameObject = (new T);
	gameObject->guid = GuidGenerator::NewGuid();
	gameObject->state = stateInit;

	std::string className = gameObject->GetClassName();
	AssignStringToCString(gameObject->name, className);

	gameObjectManager->Add(gameObject);

	gameObject->Initialise();

	return (T*)gameObject;
}
inline void GameObject::Destroy(GameObject* gameObject)
{
	if (gameObject == nullptr) return;
	if (gameObject == Destroyed) return;

	gameObject->OnDestroy();

	gameObject->state = Destroyed;
}