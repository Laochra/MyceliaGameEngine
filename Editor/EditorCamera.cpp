#include "EditorCamera.h"
#include "EditorCameraConfig.h"

#include "TimeManager.h"

#include "AppInfo.h"

void EditorCamera::SerialiseTo(json& jsonObj) const {}
void EditorCamera::DeserialiseFrom(const json& jsonObj, GuidGeneration guidOptions) {}
void EditorCamera::UpdateFrom(const json& jsonObj, GuidGeneration guidOptions) {}

glm::mat4 EditorCamera::GetViewMatrix()
{
	float thetaR = glm::radians(xRotation);
	float phiR = glm::radians(yRotation);

	vec3 forward(cos(phiR) * cos(thetaR), sin(phiR), cos(phiR) * sin(thetaR));
	vec3 up(0, 1, 0);

	vec3 globalPosition = GetGlobalPosition();
	return glm::lookAt(globalPosition, globalPosition + forward, up);
}

void EditorCamera::Initialise()
{
	Updater::UpdateAdd(this);
	gameObjectManager->Remove(this);

	if (!EditorCameraConfig::Load(this))
	{
		fov = glm::radians(80.0f);

		xInput.Bind(InputCode::A, InputCode::D);
		yInput.Bind(InputCode::LControl, InputCode::Space);
		zInput.Bind(InputCode::S, InputCode::W);

		freeCamera.Bind(InputCode::MouseRight);
		quickMode.Bind(InputCode::LShift);

		EditorCameraConfig::Save(this);
	}
}

void EditorCamera::Update()
{
	float thetaR = glm::radians(xRotation);
	float phiR = glm::radians(yRotation);

	vec3 forward = glm::normalize(vec3(cos(phiR) * cos(thetaR), 0, cos(phiR) * sin(thetaR)));
	vec3 right(-sin(thetaR), 0, cos(thetaR));
	vec3 up(0, 1, 0);

	if (AppInfo::CompareState(AppState::Editor) && freeCamera)
	{
		// Keyboard Movement
		vec3 xzMovement = ((float)xInput * right) + ((float)zInput * forward);

		if (xzMovement != vec3(0, 0, 0))
		{
			xzMovement = glm::normalize(xzMovement);

			Translate(glm::normalize(xzMovement) * (quickMode ? quickFlySpeed : flySpeed) * Time::delta);
		}

		if (yInput != 0.0f)
		{
			Translate((float)yInput * up * (quickMode ? quickFlySpeed : flySpeed) * Time::delta);
		}

		// Mouse Look
		xRotation += AppInfo::input->cursorMovement.x * turnSpeed * Time::delta;
		yRotation += -AppInfo::input->cursorMovement.y * turnSpeed * Time::delta;
		if (yRotation > 85) yRotation = 85;
		if (yRotation < -85) yRotation = -85;
	}

	SetRotation(quat(glm::vec3(glm::radians(xRotation), glm::radians(yRotation), glm::radians(0.0f))));
}