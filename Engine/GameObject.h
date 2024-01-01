#pragma once

#include "TimeManager.h"
#include "Updater.h"
#include "GuidGenerator.h"

class GameObject;
template<typename T>
concept GameObjectType = std::is_base_of<GameObject, T>::value;

class GameObject
{
public:
	enum GameObjectState { Active, Inactive, Destroyed };
	GameObjectState GetState() const noexcept;
	void SetState(GameObjectState value) noexcept;

	unsigned long int GetGUID() const noexcept;
	bool operator==(GameObject other) const noexcept;
	bool operator!=(GameObject other) const noexcept;
	bool operator==(GameObjectState stateToCheck) const noexcept;
	bool operator!=(GameObjectState stateToCheck) const noexcept;


	virtual void OnFrameStart();
	virtual void FixedUpdate();
	virtual void Update();
	virtual void Draw();

protected:
	GameObject() = default;
	~GameObject() = default;

	virtual void Initialise();
	virtual void OnDestroy();
	virtual void OnActivate();
	virtual void OnDeactivate();

private:
	unsigned long int guid;
	GameObjectState state = Active;


public: // Statics
	template <GameObjectType T>
	static T* Instantiate(GameObjectState stateInit = Active);
	template <GameObjectType T>
	static T* Instantiate(unsigned long int guidInit, GameObjectState stateInit = Active);

	template <GameObjectType T>
	static void Destroy(T* gameObject);
};

template<GameObjectType T>
inline T* GameObject::Instantiate(GameObjectState stateInit)
{
	T* gameObject = new T;
	gameObject->guid = GuidGenerator::NewGuid();
	gameObject->state = stateInit;

	gameObject->Initialise();
	Updater::UpdateAdd(gameObject);

	return gameObject;
}
template<GameObjectType T>
inline T* GameObject::Instantiate(unsigned long int guidInit, GameObjectState stateInit)
{
	T* gameObject = new T;
	gameObject->guid = guidInit;
	gameObject->state = stateInit;

	gameObject->Initialise();
	Updater::UpdateAdd(gameObject);

	return gameObject;
}

template<GameObjectType T>
inline void GameObject::Destroy(T* gameObject)
{
	if (gameObject == Destroyed) return;

	gameObject->OnDestroy();
	state = Destroyed;
}