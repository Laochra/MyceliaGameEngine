#pragma once

#include "GameObject.h"
#include "Heirarchical.h"

class GameObjectFolder : public GameObject, public Heirarchical<GameObject>
{
public:
	virtual void OnDestroy() override;
};