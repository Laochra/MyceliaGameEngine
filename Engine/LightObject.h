#pragma once

#include "GameObject3D.h"
#include "Light.h"

class LightObject : public GameObject3D, public Light
{
	SerialiseAs(LightObject);

	using GameObject3D::GameObject3D;

public:
	float range = 10.0f;
	float angle = 1.0f;
};