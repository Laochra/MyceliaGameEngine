#include "Camera.h"
#include "AppInfo.h"

#include "Debug.h"


void Camera::SerialiseTo(json& jsonObj) const
{
	GameObject3D::SerialiseTo(jsonObj);

	jsonObj["FOV"] = fov;
	jsonObj["NearClip"] = nearClip;
	jsonObj["FarClip"] = farClip;
}
void Camera::DeserialiseFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	GameObject3D::DeserialiseFrom(jsonObj, guidOptions);

	fov = jsonObj["FOV"];
	nearClip = jsonObj["NearClip"];
	farClip = jsonObj["FarClip"];

	if (AppInfo::gameCamera == nullptr) AppInfo::gameCamera = this;
}
bool Camera::UpdateFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	if (!GameObject3D::UpdateFrom(jsonObj, guidOptions)) return false;

	fov = jsonObj["FOV"];
	nearClip = jsonObj["NearClip"];
	farClip = jsonObj["FarClip"];

	return true;
}

glm::mat4 Camera::UpdateProjectionMatrix()
{
	return UpdateProjectionMatrix((float)AppInfo::screenWidth, (float)AppInfo::screenHeight);
}
glm::mat4 Camera::UpdateProjectionMatrix(float w, float h)
{
	float aspect;
	if (h == 0.0f || w == 0.0f) aspect = 0.0f;
	else aspect = w / h;

	projectionMatrix = glm::perspective(fov, aspect, nearClip, farClip);
   return projectionMatrix;
}
glm::mat4 Camera::UpdatePVMatrix()
{
	pvMatrix = GetProjectionMatrix() * GetViewMatrix();
	return pvMatrix;
}
glm::mat4 Camera::UpdateViewMatrix()
{
	viewMatrix = glm::inverse(GetMatrix());
	return viewMatrix;
}

const glm::mat4& Camera::GetProjectionMatrix() const noexcept
{
	return projectionMatrix;
}
const glm::mat4& Camera::GetPVMatrix() const noexcept
{
	return pvMatrix;
}
const glm::mat4& Camera::GetViewMatrix() const noexcept
{
	return viewMatrix;
}

void Camera::Initialise()
{
	GameObject3D::Initialise();

	fov = glm::radians(80.0f);
}

void Camera::Update()
{
	UpdateProjectionMatrix((float)AppInfo::screenWidth, (float)AppInfo::screenHeight);
	UpdateViewMatrix();
	UpdatePVMatrix();
}

void Camera::DrawDebug()
{
	AppInfo::debug->lines.AddFrustum(GetPVMatrix());
}
