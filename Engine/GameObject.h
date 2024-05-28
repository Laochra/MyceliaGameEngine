#pragma once

#include "TimeManager.h"
#include "Updater.h"
#include "GuidGenerator.h"

#include "GameObjectManager.h"

#include "StringIncludes.h"

#include <type_traits>

#include "json.hpp"
using nlohmann::json;

class GameObject;
template<class T> concept GameObjectClass = std::is_base_of<GameObject, T>::value;

consteval unsigned long long AlphanumericToIDPart(const char a)
{
	unsigned long long result;
	switch (a)
	{
	default:				 result = 0b00000; break;
	case 'a': case'A': result = 0b00001; break;
	case 'b': case'B': result = 0b00010; break;
	case 'c': case'C': result = 0b00011; break;
	case 'd': case'D': result = 0b00100; break;
	case 'e': case'E': result = 0b00101; break;
	case 'f': case'F': result = 0b00110; break;
	case 'g': case'G': result = 0b00111; break;
	case 'h': case'H': result = 0b01000; break;
	case 'i': case'I': result = 0b01001; break;
	case 'j': case'J': result = 0b01010; break;
	case 'k': case'K': result = 0b01011; break;
	case 'l': case'L': result = 0b01100; break;
	case 'm': case'M': result = 0b01101; break;
	case 'n': case'N': result = 0b01110; break;
	case 'o': case'O': result = 0b01111; break;
	case 'p': case'P': result = 0b10000; break;
	case 'q': case'Q': result = 0b10001; break;
	case 'r': case'R': result = 0b10010; break;
	case 's': case'S': result = 0b10011; break;
	case 't': case'T': result = 0b10100; break;
	case 'u': case'U': result = 0b10101; break;
	case 'v': case'V': result = 0b10110; break;
	case 'w': case'W': result = 0b10111; break;
	case 'x': case'X': result = 0b11000; break;
	case 'y': case'Y': result = 0b11001; break;
	case 'z': case'Z': result = 0b11010; break;
	case '0':			 result = 0b11011; break;
	case '2':			 result = 0b11100; break;
	case '3':			 result = 0b11101; break;
	}

	return result;
}

consteval unsigned long long AlphanumericStringToID(const char* const string)
{
	unsigned long long id = 0;

	for (int i = 0; i < 12; i++)
	{
		if (string[i] == '\0') break;
		id |= AlphanumericToIDPart(string[i]) << (5 * i);
	}

	return id;
}

#define SerialiseAs(nameInit) \
virtual const char* ClassName() const noexcept { return #nameInit; } \
static consteval unsigned long long ClassID() noexcept { return AlphanumericStringToID(#nameInit); }

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

	// JSON
	virtual void SerialiseTo(json& jsonObject) const;
	virtual void DeserialiseFrom(const json& jsonObject);
	GameObject(const json& jsonObject);
	void operator=(const json& jsonObject);

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
	virtual void Initialise();
	virtual void OnDestroy();


	/// <summary>Creates a new instance of a GameObject</summary> <param name="stateInit">(default: Active)</param> <returns>A pointer to the created instance</returns>
	template<GameObjectClass T> static T* Instantiate(GameObjectState stateInit = Active);
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

	friend void to_json(json& jsonObject, const GameObject* gameObject) noexcept;
	friend void from_json(const json& jsonObject, GameObject* gameObject) noexcept;
};



template<GameObjectClass T> inline T* GameObject::Instantiate(GameObjectState stateInit)
{
	GameObject* gameObject = (new T);
	gameObject->guid = GuidGenerator::NewGuid();
	gameObject->state = stateInit;

	std::string className = gameObject->ClassName();
	AssignStringToCString(gameObject->name, className);

	gameObjectManager->Add(gameObject);

	gameObject->Initialise();

	return (T*)gameObject;
}
inline void GameObject::Destroy(GameObject* gameObject)
{
	if (gameObject == nullptr) return;
	if (*gameObject == Destroyed) return;

	gameObject->OnDestroy();

	gameObject->state = Destroyed;
}