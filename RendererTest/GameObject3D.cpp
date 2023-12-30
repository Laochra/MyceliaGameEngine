#include "GameObject3D.h"

mat4 GameObject3D::GetMatrix()
{
    if (parent == nullptr) return GetLocalMatrix();
    else return GetLocalMatrix() * parent->GetMatrix();
}
mat4 GameObject3D::GetLocalMatrix()
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

vec3 GameObject3D::GetRotationEuler()
{
	return glm::eulerAngles(rotation);
}
void GameObject3D::SetRotationEuler(float radians, vec3 axis)
{
	rotation = glm::angleAxis(radians, axis);
}

void GameObject3D::Translate(vec3 amountToTranslate)
{
	position += amountToTranslate;
}

void GameObject3D::Rotate(float radians, vec3 axis)
{
	rotation = glm::normalize(glm::rotate(rotation, radians, axis));
}
void GameObject3D::RotateTowards(quat targetRotation, float maxRadians)
{
	if (maxRadians == 0) return;

	float dot = abs(glm::dot(rotation, targetRotation));
	
	if (dot > 1) return;

	float difference = acos(dot);

	float radians = glm::min(difference, maxRadians);
	if (radians == 0.0f) return;
	
	rotation = glm::normalize(glm::slerp(rotation, targetRotation, glm::min(radians / difference, 1.0f)));
}
void GameObject3D::LookAt(vec3 positionToLookAt)
{
	vec3 direction = glm::normalize(positionToLookAt + pivot - position);
	rotation = glm::normalize(glm::quatLookAt(direction, vec3(0, 1, 0 )));
}
void GameObject3D::LookTowards(vec3 positionToLookTowards, float maxRadians)
{
	vec3 direction = glm::normalize(positionToLookTowards + pivot - position);
	quat targetRotation = glm::normalize(glm::quatLookAt(direction, vec3(0, 1, 0)));

	RotateTowards(targetRotation, glm::radians(15.0f) * Time::delta);
}

void GameObject3D::Scale(vec3 amountToScale)
{
	scale.x *= amountToScale.x;
	scale.y *= amountToScale.y;
	scale.z *= amountToScale.z;
}
void GameObject3D::Scale(float amountToScale)
{
	scale.x *= amountToScale;
	scale.y *= amountToScale;
	scale.z *= amountToScale;
}