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

glm::mat4 Camera::GetViewMatrix()
{
	return glm::inverse(GetMatrix());
}

glm::mat4 Camera::GetProjectionMatrix(float w, float h)
{
	float aspect;
	if (h == 0.0f || w == 0.0f) aspect = 0.0f;
	else aspect = w / h;

   return glm::perspective(fov, aspect, nearClip, farClip);
}

glm::mat4 Camera::GetProjectionMatrix()
{
	return GetProjectionMatrix(AppInfo::screenWidth, AppInfo::screenHeight);
}

glm::mat4 Camera::GetPVMatrix()
{
	return GetProjectionMatrix((float)AppInfo::screenWidth, (float)AppInfo::screenHeight) * GetViewMatrix();;
}

void Camera::Initialise()
{
	Updater::UpdateAdd(this);

	fov = glm::radians(80.0f);
}

void Camera::DrawDebug()
{
	AppInfo::debug->lines.AddFrustum(GetPVMatrix());
}
