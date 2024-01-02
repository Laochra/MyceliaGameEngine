#pragma once

#include "GameObject.h"
#include "MathIncludes.h"
#include "LinkedList.h"

class GameObject3D;
template<class T> concept GameObject3DClass = std::is_base_of<GameObject3D, T>::value;

class GameObject3D : public GameObject
{
public:
	using GameObject::GameObject;

	vec3 position = vec3();
	quat rotation = quat();
	vec3 scale = vec3(1);
	vec3 pivot = vec3();

	GameObject3D* parent = nullptr;
	LinkedList<GameObject3D*> children = LinkedList<GameObject3D*>();


	/// <returns>The global transform matrix of the GameObject</returns>
	mat4 GetMatrix() const noexcept;
	/// <returns>The local transform matrix of the GameObject</returns>
	mat4 GetLocalMatrix() const noexcept;

	vec3 GetGlobalPosition() const noexcept;
	quat GetGlobalRotation() const noexcept;
	vec3 GetGlobalScale() const noexcept;


	void Translate(vec3 amountToTranslate) noexcept;

	void Rotate(float radians, vec3 axis) noexcept;
	/// <summary>Interpolates between the current and target rotations, without rotating more than the max radians</summary>
	void RotateTowards(quat targetRotation, float maxRadians) noexcept;
	/// <summary>Rotate so that the GameObject faces the provided position</summary>
	void LookAt(vec3 positionToLookAt) noexcept;
	/// <summary>Rotate so that the GameObject faces the provided position, without rotating more than the max radians</summary>
	void LookTowards(vec3 positionToLookTowards, float maxRadians) noexcept;

	void Scale(vec3 amountToScale) noexcept;
	void Scale(float amountToScale) noexcept;


	/// <summary>Creates a new instance of a GameObject3D</summary> <param name="positionInit"></param> <param name="rotationInit">(default: identity)</param> <param name="scaleInit">(default: 1,1,1)</param> <param name="pivotInit">(default: 0,0,0)</param> <param name="parentInit">(default: nullptr)</param> <param name="stateInit">(default: Active)</param> <returns>A pointer to the created instance</returns>
	template<GameObject3DClass T> static T* Instantiate(vec3 positionInit, quat rotationInit = quat(), vec3 scaleInit = vec3(1), vec3 pivotInit = vec3(), T* parentInit = nullptr, GameObjectState stateInit = Active);
	/// <summary>Creates a new instance of a GameObject3D</summary> <param name="parentInit"></param> <param name="positionInit"></param> <param name="rotationInit">(default: identity)</param> <param name="scaleInit">(default: 1,1,1)</param> <param name="pivotInit">(default: 0,0,0)</param> <param name="stateInit">(default: Active)</param> <returns>A pointer to the created instance</returns>
	template<GameObject3DClass T> static T* Instantiate(T* parentInit, vec3 positionInit, quat rotationInit = quat(), vec3 scaleInit = vec3(1), vec3 pivotInit = vec3(), GameObjectState stateInit = Active);

	/// <summary>Creates a new instance of a GameObject3D</summary> <param name="matrixInit"></param> <param name="parentInit">(default: nullptr)</param> <param name="stateInit">(default: Active)</param> <returns>A pointer to the created instance</returns>
	template<GameObject3DClass T> static T* Instantiate(mat4 matrixInit, vec3 pivotInit = vec3(), T* parentInit = nullptr, GameObjectState stateInit = Active);
	/// <summary>Creates a new instance of a GameObject3D</summary> <param name="parentInit"></param> <param name="matrixInit">(default: identity)</param> <param name="stateInit">(default: Active)</param> <returns>A pointer to the created instance</returns>
	template<GameObject3DClass T> static T* Instantiate(T* parentInit, mat4 matrixInit = glm::identity(), vec3 pivotInit = vec3(), GameObjectState stateInit = Active);
};



template<GameObject3DClass T> inline T* GameObject3D::Instantiate(vec3 positionInit, quat rotationInit, vec3 scaleInit, vec3 pivotInit, T* parentInit, GameObjectState stateInit)
{
	T* gameObject = GameObject::Instantiate<T>(stateInit);

	gameObject->position = positionInit;
	gameObject->rotation = rotationInit;
	gameObject->scale = scaleInit;
	gameObject->pivot = pivotInit;
	gameObject->parent = parentInit;

	return gameObject;
}
template<GameObject3DClass T> inline T* GameObject3D::Instantiate(T* parentInit, vec3 positionInit, quat rotationInit, vec3 scaleInit, vec3 pivotInit, GameObjectState stateInit)
{
	T* gameObject = GameObject3D::Instantiate<T>(positionInit, rotationInit, scaleInit, pivotInit, parentInit, stateInit);

	return gameObject;
}

template<GameObject3DClass T> inline T* GameObject3D::Instantiate(mat4 matrixInit, vec3 pivotInit, T* parentInit, GameObjectState stateInit)
{
	vec3 positionInit = matrixInit[3];
	quat rotationInit = glm::quat_cast(matrixInit);
	vec3 scaleInit = vec3(glm::length(vec3(matrixInit[0])), glm::length(vec3(matrixInit[1])), glm::length(vec3(matrixInit[2])));

	T* gameObject = GameObject3D::Instantiate<T>(positionInit, rotationInit, scaleInit, pivotInit, parentInit, stateInit);

	gameObject->pivot = pivotInit;
	gameObject->parent = parentInit;

	return gameObject;
}
template<GameObject3DClass T> inline T* GameObject3D::Instantiate(T* parentInit, mat4 matrixInit, vec3 pivotInit, GameObjectState stateInit)
{
	T* gameObject = GameObject3D::Instantiate<T>(matrixInit, pivotInit, parentInit, stateInit);

	return gameObject;
}