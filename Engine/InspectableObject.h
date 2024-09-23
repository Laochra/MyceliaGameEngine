#pragma once

class InspectableObject
{
public:
	bool selected = false;

	virtual const char* GetName() const noexcept = 0;
};
