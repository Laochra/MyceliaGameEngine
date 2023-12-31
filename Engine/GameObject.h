#pragma once

#include "Time.h"

class GameObject
{
public:
	GameObject();
	GameObject(unsigned long int guidInit);
	~GameObject();

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
	void Destroy() noexcept;

protected:
	virtual void Initialise();
	virtual void OnDestroy();
	virtual void OnActivate();
	virtual void OnDeactivate();

private:
	GameObjectState state = Active;
	unsigned long int guid;
};