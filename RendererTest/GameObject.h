#pragma once

class GameObject
{
public:
	GameObject();
	~GameObject();

	bool GetActive() { return active; }
	void SetActive(bool value)
	{
		if (value) OnActivate();
		else OnDeactivate();

		active = value;
	}

	unsigned long int GetGUID() { return guid; }
	bool operator==(GameObject other) { return guid == other.guid; }
	bool operator!=(GameObject other) { return guid != other.guid; }

	virtual void OnFrameStart();
	virtual void FixedUpdate();
	virtual void Update();
	virtual void Draw();

protected:
	virtual void Initialise();
	virtual void OnDestroy();
	virtual void OnActivate();
	virtual void OnDeactivate();

private:
	bool active;
	unsigned long int guid;
};