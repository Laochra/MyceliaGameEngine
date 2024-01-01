#pragma once

#include "GameObject.h"
#include "MathIncludes.h"
#include "LinkedList.h"

class GameObject3D;
template<typename T>
concept GameObject3DType = std::is_base_of<GameObject3D, T>::value;

class GameObject3D : public GameObject
{
public:
	vec3 position = vec3{0, 0, 0};
	quat rotation = quat();
	vec3 scale = vec3(1, 1, 1);
	vec3 pivot = vec3(0, 0, 0);

	GameObject3D* parent = nullptr;
	LinkedList<GameObject3D*> children = LinkedList<GameObject3D*>();

	using GameObject::GameObject;


	mat4 GetMatrix() const noexcept;
	mat4 GetLocalMatrix() const noexcept;

	vec3 GetRotationEuler() const noexcept;
	void SetRotationEuler(float radians, vec3 axis) noexcept;

	vec3 GetGlobalPosition() const noexcept;
	void Translate(vec3 amountToTranslate) noexcept;

	quat GetGlobalRotation() const noexcept;
	void Rotate(float radians, vec3 axis) noexcept;
	void RotateTowards(quat targetRotation, float maxRadians) noexcept;
	void LookAt(vec3 positionToLookAt) noexcept;
	void LookTowards(vec3 positionToLookTowards, float maxRadians) noexcept;

	vec3 GetGlobalScale() const noexcept;
	void Scale(vec3 amountToScale) noexcept;
	void Scale(float amountToScale) noexcept;


public: // Statics
	template <GameObject3DType T>
	static T* Instantiate(vec3 positionInit, quat rotationInit = quat(), vec3 scaleInit = vec3(1), vec3 pivotInit = vec3(0),
						  T* parentInit = nullptr, GameObjectState stateInit = Active);
	template <GameObject3DType T>
	static T* Instantiate(unsigned long int guidInit,
						  vec3 positionInit, quat rotationInit = quat(), vec3 scaleInit = vec3(1), vec3 pivotInit = vec3(0),
						  T* parentInit = nullptr, GameObjectState stateInit = Active);
	template <GameObject3DType T>
	static T* Instantiate(T* parentInit,
						  vec3 positionInit, quat rotationInit = quat(), vec3 scaleInit = vec3(1), vec3 pivotInit = vec3(0),
						  GameObjectState stateInit = Active);
	template <GameObject3DType T>
	static T* Instantiate(unsigned long int guidInit, T* parentInit,
						  vec3 positionInit, quat rotationInit = quat(), vec3 scaleInit = vec3(1), vec3 pivotInit = vec3(0),
						  GameObjectState stateInit = Active);

	template <GameObject3DType T>
	static T* Instantiate(mat4 matrixInit, vec3 pivotInit = vec3(0), T* parentInit = nullptr,
						  GameObjectState stateInit = Active);
	template <GameObject3DType T>
	static T* Instantiate(unsigned long int guidInit, mat4 matrixInit, vec3 pivotInit = vec3(0), T* parentInit = nullptr,
						  GameObjectState stateInit = Active);
	template <GameObject3DType T>
	static T* Instantiate(T* parentInit, mat4 matrixInit = glm::identity(), vec3 pivotInit = vec3(0),
		GameObjectState stateInit = Active);
	template <GameObject3DType T>
	static T* Instantiate(unsigned long int guidInit, T* parentInit, mat4 matrixInit = glm::identity(), vec3 pivotInit = vec3(0),
		GameObjectState stateInit = Active);
};



template<GameObject3DType T>
inline T* GameObject3D::Instantiate(vec3 positionInit, quat rotationInit, vec3 scaleInit, vec3 pivotInit, T* parentInit, GameObjectState stateInit)
{
	T* gameObject = GameObject::Instantiate<T>(stateInit);

	gameObject->position = positionInit;
	gameObject->rotation = rotationInit;
	gameObject->scale = scaleInit;
	gameObject->pivot = pivotInit;
	gameObject->parent = parentInit;

	return gameObject;
}
template<GameObject3DType T>
inline T* GameObject3D::Instantiate(unsigned long int guidInit, vec3 positionInit, quat rotationInit, vec3 scaleInit, vec3 pivotInit, T* parentInit, GameObjectState stateInit)
{
	T* gameObject = GameObject::Instantiate<T>(guidInit, stateInit);

	gameObject->position = positionInit;
	gameObject->rotation = rotationInit;
	gameObject->scale = scaleInit;
	gameObject->pivot = pivotInit;
	gameObject->parent = parentInit;

	return gameObject;
}
template<GameObject3DType T>
inline T* GameObject3D::Instantiate(T* parentInit, vec3 positionInit, quat rotationInit, vec3 scaleInit, vec3 pivotInit, GameObjectState stateInit)
{
	T* gameObject = GameObject3D::Instantiate<T>(positionInit, rotationInit, scaleInit, pivotInit, parentInit, stateInit);

	return gameObject;
}
template<GameObject3DType T>
inline T* GameObject3D::Instantiate(unsigned long int guidInit, T* parentInit, vec3 positionInit, quat rotationInit, vec3 scaleInit, vec3 pivotInit, GameObjectState stateInit)
{
	T* gameObject = GameObject3D::Instantiate<T>(guidInit, positionInit, rotationInit, scaleInit, pivotInit, parentInit, stateInit);

	return gameObject;
}


template<GameObject3DType T>
inline T* GameObject3D::Instantiate(mat4 matrixInit, vec3 pivotInit, T* parentInit, GameObjectState stateInit)
{
	vec3 positionInit = matrixInit[3];
	quat rotationInit = glm::quat_cast(matrixInit);
	vec3 scaleInit = vec3(glm::length(vec3(matrixInit[0])), glm::length(vec3(matrixInit[1])), glm::length(vec3(matrixInit[2])));

	T* gameObject = GameObject3D::Instantiate<T>(positionInit, rotationInit, scaleInit, pivotInit, parentInit, stateInit);

	gameObject->pivot = pivotInit;
	gameObject->parent = parentInit;

	return gameObject;
}
template<GameObject3DType T>
inline T* GameObject3D::Instantiate(unsigned long int guidInit, mat4 matrixInit, vec3 pivotInit, T* parentInit, GameObjectState stateInit)
{
	vec3 positionInit = matrixInit[3];
	quat rotationInit = glm::quat_cast(matrixInit);
	vec3 scaleInit = vec3(glm::length(vec3(matrixInit[0])), glm::length(vec3(matrixInit[1])), glm::length(vec3(matrixInit[2])));

	T* gameObject = GameObject3D::Instantiate<T>(guidInit, positionInit, rotationInit, scaleInit, pivotInit, parentInit, stateInit);

	gameObject->pivot = pivotInit;
	gameObject->parent = parentInit;

	return gameObject;
}
template<GameObject3DType T>
inline T* GameObject3D::Instantiate(T* parentInit, mat4 matrixInit, vec3 pivotInit, GameObjectState stateInit)
{
	T* gameObject = GameObject3D::Instantiate<T>(matrixInit, pivotInit, parentInit, stateInit);

	return gameObject;
}
template<GameObject3DType T>
inline T* GameObject3D::Instantiate(unsigned long int guidInit, T* parentInit, mat4 matrixInit, vec3 pivotInit, GameObjectState stateInit)
{
	T* gameObject = GameObject3D::Instantiate<T>(guidInit, matrixInit, pivotInit, parentInit, stateInit);

	return gameObject;
}