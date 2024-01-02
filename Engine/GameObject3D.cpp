#include "GameObject3D.h"

mat4 GameObject3D::GetMatrix() const noexcept
{
    if (parent == nullptr) return GetLocalMatrix();
    else return GetLocalMatrix() * parent->GetMatrix();
}
mat4 GameObject3D::GetLocalMatrix() const noexcept
{
	mat4 centrer = mat4
	(
		1,			0,			0,			0,
		0,			1,			0,			0,
		0,			0,			1,			0,
		-pivot.x,	-pivot.y,	-pivot.z,	1
	);

	mat4 scaler = mat4
	(
		scale.x,	0,			0,			0,
		0,			scale.y,	0,			0,
		0,			0,			scale.z,	0,
		0,			0,			0,			1
	);
	mat4 rotator = glm::mat4_cast(rotation);
	mat4 translater = mat4
	(
		1,			0,			0,			0,
		0,			1,			0,			0,
		0,			0,			1,			0,
		position.x,	position.y,	position.z, 1
	);

	return translater * rotator * scaler * centrer;
}

vec3 GameObject3D::GetGlobalPosition() const noexcept
{
	if (parent == nullptr) return position - pivot;
	return parent->GetGlobalPosition() + position - pivot;
}

void GameObject3D::Translate(vec3 amountToTranslate) noexcept
{
	position += amountToTranslate;
}

quat GameObject3D::GetGlobalRotation() const noexcept
{
	if (parent == nullptr) return glm::normalize(rotation);
	return glm::normalize(parent->GetGlobalRotation() * rotation);
}

void GameObject3D::Rotate(float radians, vec3 axis) noexcept
{
	rotation = glm::normalize(glm::rotate(rotation, radians, axis));
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
}
void GameObject3D::LookAt(vec3 positionToLookAt) noexcept
{
	vec3 direction = glm::normalize(positionToLookAt + pivot - position);
	rotation = glm::normalize(glm::quatLookAt(direction, vec3(0, 1, 0 )));
}
void GameObject3D::LookTowards(vec3 positionToLookTowards, float maxRadians) noexcept
{
	vec3 direction = glm::normalize(positionToLookTowards + pivot - position);
	quat targetRotation = glm::normalize(glm::quatLookAt(direction, vec3(0, 1, 0)));

	RotateTowards(targetRotation, glm::radians(15.0f) * Time::delta);
}

vec3 GameObject3D::GetGlobalScale() const noexcept
{
	if (parent == nullptr) return scale;
	return parent->GetGlobalScale() * scale;
}

void GameObject3D::Scale(vec3 amountToScale) noexcept
{
	scale.x *= amountToScale.x;
	scale.y *= amountToScale.y;
	scale.z *= amountToScale.z;
}
void GameObject3D::Scale(float amountToScale) noexcept
{
	scale.x *= amountToScale;
	scale.y *= amountToScale;
	scale.z *= amountToScale;
}