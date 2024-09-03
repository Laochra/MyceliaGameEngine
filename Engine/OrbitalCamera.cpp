#include "OrbitalCamera.h"

#include "TimeManager.h"
#include "Input.h"

#include "AppInfo.h"

glm::mat4 OrbitalCamera::GetViewMatrix()
{
    vec3 up = vec3(0, 1, 0);
    return glm::lookAt(GetGlobalPosition(), centre, up);
}

void OrbitalCamera::Update()
{
	vec3 forward = glm::normalize(vec3(GetGlobalPosition().x, 0, GetGlobalPosition().z) - vec3(centre.x, 0, centre.z));
	vec3 up = vec3(0, 1, 0);
	vec3 right = glm::cross(forward, up);
	
	InputAxis xInput(InputCode::A, InputCode::D);
	InputAxis yInput(InputCode::LShift, InputCode::Space);
	InputAxis zInput(InputCode::S, InputCode::W);

	// Keyboard Movement
	if (xInput != 0.0f)
	{
		Translate(-(float)xInput * right * 2.0f * Time::delta);
		centre += -(float)xInput * right * 2.0f * Time::delta;
	}
	if (yInput != 0.0f)
	{
		Translate((float)yInput * up * Time::delta);
		centre += (float)yInput * up * Time::delta;
	}
	if (zInput != 0.0f)
	{
		Translate(-(float)zInput * forward * 2.0f * Time::delta);
		centre += -(float)zInput * forward * 2.0f * Time::delta;
	}

	InputBind cursorToggle(InputCode::MouseRight);

	// Mouse Orbitting
	if (cursorToggle)
	{
		Translate(AppInfo::input->cursorMovement.x * right * 2.0f * Time::delta);

		vec3 zoom = AppInfo::input->cursorMovement.y * forward * 2.0f * Time::delta;

		vec3 position = GetPosition();
		bool minZoomOverflow = glm::distance(vec3(position.x + zoom.x, 0, position.z + zoom.z), vec3(centre.x, 0, centre.z)) < minZoom;
		bool maxZoomOverflow = glm::distance(vec3(position.x, 0, position.z), vec3(centre.x, 0, centre.z)) > maxZoom;

		if ((!minZoomOverflow || AppInfo::input->cursorMovement.y > 0) && (!maxZoomOverflow || AppInfo::input->cursorMovement.y < 0))
		{
			Translate(zoom);
			Translate(AppInfo::input->cursorMovement.y * up * 0.75f * Time::delta);
		}
	}
}