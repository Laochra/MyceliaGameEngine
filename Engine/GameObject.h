#pragma once

#include "TimeManager.h"
#include "Updater.h"
#include "GuidGenerator.h"

#include "GameObjectManager.h"

#include <string>

#include <type_traits>

class GameObject;
template<class T> concept GameObjectClass = std::is_base_of<GameObject, T>::value;

#define SerialiseAs(nameInit) virtual const char* ClassName() noexcept { return #nameInit; }


class GameObject
{
public:
	SerialiseAs(GameObject);
	std::string name;

	enum GameObjectState : unsigned char
	{
		/// <summary>The GameObject can recieve updates</summary>
		Active,
		/// <summary>The GameObject cannot recieve updates</summary>
		Inactive,
		/// <summary>The GameObject has been destroyed and should no longer be referenced</summary>
		Destroyed
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
	unsigned long int GetGUID() const noexcept;
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
	unsigned long int guid = 0;
	GameObjectState state = Active;


	// Friends
	friend class GameObjectManager;
};



template<GameObjectClass T> inline T* GameObject::Instantiate(GameObjectState stateInit)
{
	T* gameObject = new T;
	gameObject->guid = GuidGenerator::NewGuid();
	gameObject->state = stateInit;
	gameObject->name = gameObject->ClassName();

	gameObjectManager->Add(gameObject);

	gameObject->Initialise();

	return gameObject;
}
inline void GameObject::Destroy(GameObject* gameObject)
{
	if (gameObject == nullptr) return;
	if (*gameObject == Destroyed) return;

	gameObject->OnDestroy();

	gameObject->state = Destroyed;
}