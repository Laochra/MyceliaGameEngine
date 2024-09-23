#pragma once

#include "GameObject.h"

#include "MathIncludes.h"

#include <vector>
using std::vector;

#include "HeirarchicalDefinitions.h"

class GameObject3D;
template<class T> concept GameObject3DClass = std::is_base_of<GameObject3D, T>::value;

class GameObject3D : public GameObject, public Heirarchical<GameObject3D>
{
public:
	SerialiseAs(GameObject3D);

	using GameObject::GameObject;

	virtual void OnDestroy() override;
	virtual void OnRestore() override;

	bool IsActive() const noexcept override;


	void UpdateMatrices() noexcept;
	/// <returns>The global transform matrix of the GameObject</returns>
	mat4 GetMatrix() noexcept;
	/// <returns>The local transform matrix of the GameObject</returns>
	mat4 GetLocalMatrix() noexcept;
	void SetDirty();

	vec3 GetGlobalPosition() noexcept;
	vec3 GetGlobalPivot() noexcept;
	mat3 GetGlobalRotationMatrix() noexcept;
	quat GetGlobalRotationQuat() noexcept;
	vec3 GetGlobalScale() noexcept;


	vec3 GetPosition() const noexcept;
	void SetPosition(vec3 newPosition) noexcept;
	void Translate(vec3 amountToTranslate) noexcept;

	mat3 GetRotationMatrix() const noexcept;
	quat GetRotationQuat() const noexcept;
	void SetRotation(quat newRotation) noexcept;
	void Rotate(float radians, vec3 axis) noexcept;
	/// <summary>Interpolates between the current and target rotations, without rotating more than the max radians</summary>
	void RotateTowards(quat targetRotation, float maxRadians) noexcept;
	/// <summary>Rotate so that the GameObject faces the provided position</summary>
	void LookAt(vec3 positionToLookAt) noexcept;
	/// <summary>Rotate so that the GameObject faces the provided position, without rotating more than the max radians</summary>
	void LookTowards(vec3 positionToLookTowards, float maxRadians) noexcept;

	vec3 GetScale() const noexcept;
	void SetScale(vec3 newScale) noexcept;
	void SetScale(float newScale) noexcept;
	void Scale(vec3 amountToScale) noexcept;
	void Scale(float amountToScale) noexcept;

	vec3 GetPivot() const noexcept;
	void SetPivot(vec3 newPivot) noexcept;
	

	/// <summary>Creates a new instance of a GameObject3D</summary> <param name="positionInit"></param> <param name="rotationInit">(default: identity)</param> <param name="scaleInit">(default: 1,1,1)</param> <param name="pivotInit">(default: 0,0,0)</param> <param name="parentInit">(default: nullptr)</param> <param name="stateInit">(default: Active)</param> <returns>A pointer to the created instance</returns>
	template<GameObject3DClass T> static T* Instantiate(vec3 positionInit, quat rotationInit = glm::identity<quat>(), vec3 scaleInit = vec3(1), vec3 pivotInit = vec3(), GameObject3D* parentInit = nullptr, GameObjectState stateInit = Active);
	/// <summary>Creates a new instance of a GameObject3D</summary> <param name="parentInit"></param> <param name="positionInit"></param> <param name="rotationInit">(default: identity)</param> <param name="scaleInit">(default: 1,1,1)</param> <param name="pivotInit">(default: 0,0,0)</param> <param name="stateInit">(default: Active)</param> <returns>A pointer to the created instance</returns>
	template<GameObject3DClass T> static T* Instantiate(GameObject3D* parentInit, vec3 positionInit, quat rotationInit = glm::identity<quat>(), vec3 scaleInit = vec3(1), vec3 pivotInit = vec3(), GameObjectState stateInit = Active);

	/// <summary>Creates a new instance of a GameObject3D</summary> <param name="matrixInit"></param> <param name="parentInit">(default: nullptr)</param> <param name="stateInit">(default: Active)</param> <returns>A pointer to the created instance</returns>
	template<GameObject3DClass T> static T* Instantiate(mat4 matrixInit, vec3 pivotInit = vec3(), GameObject3D* parentInit = nullptr, GameObjectState stateInit = Active);
	/// <summary>Creates a new instance of a GameObject3D</summary> <param name="parentInit"></param> <param name="matrixInit">(default: identity)</param> <param name="stateInit">(default: Active)</param> <returns>A pointer to the created instance</returns>
	template<GameObject3DClass T> static T* Instantiate(GameObject3D* parentInit, mat4 matrixInit = glm::identity<mat4>(), vec3 pivotInit = vec3(), GameObjectState stateInit = Active);
	
private:
	vec3 position = vec3();
	quat rotation = glm::identity<quat>();
	vec3 scale = vec3(1);
	vec3 pivot = vec3();

	mat4 globalMatrix = glm::identity<mat4>();
	mat4 localMatrix = glm::identity<mat4>();
	bool dirty = true;


	// Friends
	//friend void to_json(json& jsonObject, const GameObject3D& gameObject) noexcept;
	//friend void from_json(const json& jsonObject, GameObject3D& gameObject) noexcept;
};



template<GameObject3DClass T> inline T* GameObject3D::Instantiate(vec3 positionInit, quat rotationInit, vec3 scaleInit, vec3 pivotInit, GameObject3D* parentInit, GameObjectState stateInit)
{
	T* gameObject = GameObject::Instantiate<T>(stateInit);

	// TODO Finish Seperating from GameObject's Instantiate

	//T* gameObject = new T;
	//gameObject->guid = GuidGenerator::NewGuid();
	//gameObject->state = stateInit;
	//
	//if (parentInit is nullptr) gameObjectManager->Add(gameObject);
	//
	//gameObject->Initialise();

	gameObject->position = positionInit;
	gameObject->rotation = rotationInit;
	gameObject->scale = scaleInit;
	gameObject->pivot = pivotInit;
	gameObject->SetParent(parentInit);

	return gameObject;
}
template<GameObject3DClass T> inline T* GameObject3D::Instantiate(GameObject3D* parentInit, vec3 positionInit, quat rotationInit, vec3 scaleInit, vec3 pivotInit, GameObjectState stateInit)
{
	T* gameObject = GameObject3D::Instantiate<T>(positionInit, rotationInit, scaleInit, pivotInit, parentInit, stateInit);

	return gameObject;
}

template<GameObject3DClass T> inline T* GameObject3D::Instantiate(mat4 matrixInit, vec3 pivotInit, GameObject3D* parentInit, GameObjectState stateInit)
{
	vec3 positionInit = matrixInit[3];
	vec3 scaleInit = vec3(glm::length(vec3(matrixInit[0])), glm::length(vec3(matrixInit[1])), glm::length(vec3(matrixInit[2])));
	const mat3 rotationMatrix(
		vec3(matrixInit[0]) / scaleInit[0],
		vec3(matrixInit[1]) / scaleInit[1],
		vec3(matrixInit[2]) / scaleInit[2]
	);
	quat rotationInit = glm::quat_cast(rotationMatrix);

	T* gameObject = GameObject3D::Instantiate<T>(positionInit, rotationInit, scaleInit, pivotInit, parentInit, stateInit);

	gameObject->pivot = pivotInit;
	gameObject->SetParent(parentInit);

	return gameObject;
}
template<GameObject3DClass T> inline T* GameObject3D::Instantiate(GameObject3D* parentInit, mat4 matrixInit, vec3 pivotInit, GameObjectState stateInit)
{
	T* gameObject = GameObject3D::Instantiate<T>(matrixInit, pivotInit, parentInit, stateInit);

	return gameObject;
}