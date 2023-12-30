#pragma once

#include "GameObject.h"
#include "MathIncludes.h"
#include "LinkedList.h"

class GameObject3D : GameObject
{
public:
	vec3 position = vec3{0, 0, 0};
	quat rotation = quat();
	vec3 scale = vec3{1, 1, 1};
	vec3 pivot = vec3{0, 0, 0};

	GameObject3D* parent = nullptr;
	LinkedList<GameObject3D*> children = LinkedList<GameObject3D*>();

	using GameObject::GameObject;

	mat4 GetMatrix();
	mat4 GetLocalMatrix();

	vec3 GetRotationEuler();
	void SetRotationEuler(float radians, vec3 axis);

	void Translate(vec3 amountToTranslate);

	void Rotate(float radians, vec3 axis);
	void RotateTowards(quat targetRotation, float maxRadians);
	void LookAt(vec3 positionToLookAt);
	void LookTowards(vec3 positionToLookTowards, float maxRadians);

	void Scale(vec3 amountToScale);
	void Scale(float amountToScale);
};