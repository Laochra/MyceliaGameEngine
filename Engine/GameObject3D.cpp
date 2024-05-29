#include "GameObject3D.h"

#include "MeshRenderer.h"
#include "LightObject.h"

void GameObject3D::SerialiseTo(json& jsonObject) const
{
	GameObject::SerialiseTo(jsonObject);
	
	vector<float> positionData(3);
	memcpy(positionData.data(), &position[0], 3 * sizeof(float));
	jsonObject["Position"] = positionData;
	
	vector<float> rotationData(4);
	memcpy(rotationData.data(), &rotation[0], 4 * sizeof(float));
	jsonObject["Rotation"] = rotationData;
	
	vector<float> scaleData(3);
	memcpy(scaleData.data(), &scale[0], 3 * sizeof(float));
	jsonObject["Scale"] = scaleData;
	
	vector<float> pivotData(3);
	memcpy(pivotData.data(), &pivot[0], 3 * sizeof(float));
	jsonObject["Pivot"] = pivotData;
	
	vector<json> childrenData;
	for (int i = 0; i < children.size(); i++)
	{
		json child = children[i];
		childrenData.push_back(child);
	}
	jsonObject["Children"] = childrenData;
}
void GameObject3D::DeserialiseFrom(const json& jsonObject)
{
	GameObject::DeserialiseFrom(jsonObject);

	vector<float> positionData = jsonObject["Position"];
	memcpy(&position[0], positionData.data(), 3 * sizeof(float));

	vector<float> rotationData = jsonObject["Rotation"];
	memcpy(&rotation[0], rotationData.data(), 4 * sizeof(float));

	vector<float> scaleData = jsonObject["Scale"];
	memcpy(&scale[0], scaleData.data(), 3 * sizeof(float));

	vector<float> pivotData = jsonObject["Pivot"];
	memcpy(&pivot[0], pivotData.data(), 3 * sizeof(float));

	vector<json> childrenData = jsonObject["Children"];
	for (int i = 0; i < childrenData.size(); i++)
	{		
		GameObject3D* child;

		unsigned long long childClassID = childrenData[i]["TypeID"];
		switch (childClassID)
		{
		case GameObject3D::classID: child = new GameObject3D(childrenData[i]); break;
		case MeshRenderer::classID: child = new MeshRenderer(childrenData[i]); break;
		case LightObject::classID: child = new LightObject(childrenData[i]); break;
		}

		children.push_back(child);
		child->parent = this;
	}

	// Deserialise 3D Stuff
}

void GameObject3D::OnDestroy()
{
	for (GameObject3D* child : children)
	{
		GameObject::Destroy(child);
	}

	if (parent != nullptr) SetParent(nullptr);
	
	GameObject::OnDestroy();
}



bool GameObject3D::IsActive() noexcept
{
	if (parent == nullptr) { return *this == Active; }
	else { return *this == Active && parent->IsActive(); }
}



void GameObject3D::UpdateMatrices() noexcept
{
	mat4 centrer = mat4{
		1,			0,			0,			0,
		0,			1,			0,			0,
		0,			0,			1,			0,
		-pivot.x,	-pivot.y,	-pivot.z,	1
	};
	mat4 scaler = mat4{
		scale.x,	0,			0,			0,
		0,			scale.y,	0,			0,
		0,			0,			scale.z,	0,
		0,			0,			0,			1
	};
	mat4 rotator = glm::mat4_cast(rotation);
	mat4 translater = mat4{
		1,			0,			0,			0,
		0,			1,			0,			0,
		0,			0,			1,			0,
		position.x,	position.y,	position.z, 1
	};

	localMatrix = translater * rotator * scaler * centrer;

	if (parent == nullptr)
	{
		globalMatrix = localMatrix;
	}
	else
	{
		if (parent->dirty) parent->UpdateMatrices();
		globalMatrix = parent->globalMatrix * localMatrix;
	}

	dirty = false;
}

mat4 GameObject3D::GetMatrix() noexcept
{
	if (dirty) UpdateMatrices();
    
	return globalMatrix;
}
mat4 GameObject3D::GetLocalMatrix() noexcept
{
	if (dirty) UpdateMatrices();

	return localMatrix;
}

void GameObject3D::SetDirty()
{
	dirty = true;
	for (GameObject3D* child : children) { child->SetDirty(); }
}


vec3 GameObject3D::GetGlobalPosition() const noexcept
{
	if (parent == nullptr) return position - pivot;
	return parent->GetGlobalPosition() + position - pivot;
}

quat GameObject3D::GetGlobalRotation() const noexcept
{
	if (parent == nullptr) return glm::normalize(rotation);
	return glm::normalize(parent->GetGlobalRotation() * rotation);
}

vec3 GameObject3D::GetGlobalScale() const noexcept
{
	if (parent == nullptr) return scale;
	return parent->GetGlobalScale() * scale;
}


vec3 GameObject3D::GetPosition() const noexcept
{
	return position;
}

void GameObject3D::SetPosition(vec3 newPosition) noexcept
{
	if (position != newPosition)
	{
		position = newPosition;

		if (!dirty) SetDirty();
	}
}
void GameObject3D::Translate(vec3 amountToTranslate) noexcept
{
	position += amountToTranslate;

	if (!dirty) SetDirty();
}

quat GameObject3D::GetRotation() const noexcept
{
	return rotation;
}

void GameObject3D::SetRotation(quat newRotation) noexcept
{
	if (rotation != newRotation)
	{
		rotation = newRotation;

		if (!dirty) SetDirty();
	}
}
void GameObject3D::Rotate(float radians, vec3 axis) noexcept
{
	rotation = glm::normalize(glm::rotate(rotation, radians, axis));

	if (!dirty) SetDirty();
}
void GameObject3D::RotateTowards(quat targetRotation, float maxRadians) noexcept
{
	if (maxRadians == 0) return;

	float dot = abs(glm::dot(rotation, targetRotation));
	
	if (dot > 1) return;

	float difference = acos(dot);

	float radians = glm::min(difference, maxRadians);
	if (radians == 0.0f) return;
	
	rotation = glm::normalize(glm::slerp(rotation, targetRotation, glm::min(radians / difference, 1.0f)));

	if (!dirty) SetDirty();
}
void GameObject3D::LookAt(vec3 positionToLookAt) noexcept
{
	vec3 direction = glm::normalize(positionToLookAt + pivot - position);
	rotation = glm::normalize(glm::quatLookAt(direction, vec3(0, 1, 0 )));

	if (!dirty) SetDirty();
}
void GameObject3D::LookTowards(vec3 positionToLookTowards, float maxRadians) noexcept
{
	vec3 direction = glm::normalize(positionToLookTowards + pivot - position);
	quat targetRotation = glm::normalize(glm::quatLookAt(direction, vec3(0, 1, 0)));

	RotateTowards(targetRotation, glm::radians(15.0f) * Time::delta);

	if (!dirty) SetDirty();
}

vec3 GameObject3D::GetScale() const noexcept
{
	return scale;
}

void GameObject3D::SetScale(vec3 newScale) noexcept
{
	if (scale != newScale)
	{
		scale = newScale;

		if (!dirty) SetDirty();
	}
}
void GameObject3D::SetScale(float newScale) noexcept
{
	if (scale != vec3(newScale))
	{
		scale = vec3(newScale);

		if (!dirty) SetDirty();
	}
}
void GameObject3D::Scale(vec3 amountToScale) noexcept
{
	scale.x *= amountToScale.x;
	scale.y *= amountToScale.y;
	scale.z *= amountToScale.z;

	if (!dirty) SetDirty();
}
void GameObject3D::Scale(float amountToScale) noexcept
{
	scale.x *= amountToScale;
	scale.y *= amountToScale;
	scale.z *= amountToScale;

	if (!dirty) SetDirty();
}

void GameObject3D::SetPivot(vec3 newPivot) noexcept
{
	if (pivot != newPivot)
	{
		pivot = newPivot;

		if (!dirty) SetDirty();
	}
}
vec3 GameObject3D::GetPivot() const noexcept
{
	return pivot;
}

//void to_json(json& jsonObject, const GameObject3D& gameObject3D) noexcept
//{
//	to_json(jsonObject, *(GameObject*)&gameObject3D);
//
//	vector<float> position(3);
//	memcpy(position.data(), &gameObject3D.position[0], 3 * sizeof(float));;
//	jsonObject["Position"] = position;
//
//	vector<float> rotation(4);
//	memcpy(rotation.data(), &gameObject3D.rotation[0], 4 * sizeof(float));;
//	jsonObject["Rotation"] = rotation;
//
//	vector<float> scale(3);
//	memcpy(scale.data(), &gameObject3D.scale[0], 3 * sizeof(float));;
//	jsonObject["Scale"] = scale;
//
//	vector<float> pivot(3);
//	memcpy(pivot.data(), &gameObject3D.pivot[0], 3 * sizeof(float));;
//	jsonObject["Pivot"] = pivot;
//
//	vector<json> children;
//	for (int i = 0; i < gameObject3D.children.size(); i++)
//	{
//		json child = *gameObject3D.children[i];
//		children.push_back(child);
//	}
//	jsonObject["Children"] = children;
//}
//void from_json(const json& jsonObject, GameObject3D& gameObject3D) noexcept
//{
//	from_json(jsonObject, *(GameObject*)&gameObject3D);
//
//	//extra 3d things
//}
