#include "GameObject3D.h"

GameObject3D* GameObject3D::GetParent() const noexcept
{
	return parent;
}

void GameObject3D::SetParent(GameObject3D* parentInit) noexcept
{
	if (parentInit == parent) return;

	if (parentInit == nullptr)
	{
		gameObjectManager->Add(this);
		parent->RemoveChild(this);
	}
	else
	{
		if (parent == nullptr)
		{
			gameObjectManager->Remove(this);
		}
		else
		{
			parent->RemoveChild(this);
		}
		parentInit->AddChild(this);
	}

	parent = parentInit;
}
const vector<GameObject3D*>* GameObject3D::GetChildren() const noexcept
{
	return &children;
}
void GameObject3D::AddChild(GameObject3D* child) noexcept
{
	children.push_back(child);
}
void GameObject3D::RemoveChild(GameObject3D* child) noexcept
{
	for (int i = 0; i < children.size(); i++)
	{
		if (*children[i] == *child)
		{
			children.erase(children.begin() + i);
			break;
		}
	}
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

	if (parent == nullptr) globalMatrix = localMatrix;
	else globalMatrix = localMatrix * parent->globalMatrix;

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

		if (!dirty) dirty = true;
	}
}
void GameObject3D::Translate(vec3 amountToTranslate) noexcept
{
	position += amountToTranslate;

	if (!dirty) dirty = true;
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

		if (!dirty) dirty = true;
	}
}
void GameObject3D::Rotate(float radians, vec3 axis) noexcept
{
	rotation = glm::normalize(glm::rotate(rotation, radians, axis));

	if (!dirty) dirty = true;
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

	if (!dirty) dirty = true;
}
void GameObject3D::LookAt(vec3 positionToLookAt) noexcept
{
	vec3 direction = glm::normalize(positionToLookAt + pivot - position);
	rotation = glm::normalize(glm::quatLookAt(direction, vec3(0, 1, 0 )));

	if (!dirty) dirty = true;
}
void GameObject3D::LookTowards(vec3 positionToLookTowards, float maxRadians) noexcept
{
	vec3 direction = glm::normalize(positionToLookTowards + pivot - position);
	quat targetRotation = glm::normalize(glm::quatLookAt(direction, vec3(0, 1, 0)));

	RotateTowards(targetRotation, glm::radians(15.0f) * Time::delta);

	if (!dirty) dirty = true;
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

		if (!dirty) dirty = true;
	}
}
void GameObject3D::SetScale(float newScale) noexcept
{
	if (scale != vec3(newScale))
	{
		scale = vec3(newScale);

		if (!dirty) dirty = true;
	}
}
void GameObject3D::Scale(vec3 amountToScale) noexcept
{
	scale.x *= amountToScale.x;
	scale.y *= amountToScale.y;
	scale.z *= amountToScale.z;

	if (!dirty) dirty = true;
}
void GameObject3D::Scale(float amountToScale) noexcept
{
	scale.x *= amountToScale;
	scale.y *= amountToScale;
	scale.z *= amountToScale;

	if (!dirty) dirty = true;
}

void GameObject3D::SetPivot(vec3 newPivot) noexcept
{
	if (pivot != newPivot)
	{
		pivot = newPivot;

		if (!dirty) dirty = true;
	}
}
vec3 GameObject3D::GetPivot() const noexcept
{
	return pivot;
}