#pragma once

#include "GameObject3D.h"
#include "Light.h"

class LightObject : public GameObject3D, public Light
{
	SerialiseAs(LightObject);

	using GameObject3D::GameObject3D;

public:
	float range = 1.0f;
	vec2 angle = { 1.0f, 1.0f };

	virtual void Draw() override;

	virtual void Initialise() override;
	virtual void OnDestroy() override;
};