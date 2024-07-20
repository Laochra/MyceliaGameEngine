#include "Camera.h"

int screenWidth = 0;
int screenHeight = 0;
bool screenSizeJustChanged = true;

Camera* Camera::main = nullptr;

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
}
void Camera::UpdateFrom(const json& jsonObj, GuidGeneration guidOptions)
{
	GameObject3D::UpdateFrom(jsonObj, guidOptions);

	fov = jsonObj["FOV"];
	nearClip = jsonObj["NearClip"];
	farClip = jsonObj["FarClip"];
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::mat4();
}

glm::mat4 Camera::GetProjectionMatrix(float w, float h)
{
	float aspect;
	if (h == 0.0f || w == 0.0f) aspect = 0.0f;
	else aspect = w / h;

   return glm::perspective(fov, aspect, nearClip, farClip);
}

glm::mat4 Camera::GetPVMatrix()
{
	return GetProjectionMatrix((float)screenWidth, (float)screenHeight) * GetViewMatrix();;
}

void Camera::Initialise()
{
	Updater::UpdateAdd(this);

	fov = glm::radians(80.0f);
}