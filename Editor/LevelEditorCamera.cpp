#include "LevelEditorCamera.h"

#include "TimeManager.h"

glm::mat4 LevelEditorCamera::GetViewMatrix()
{
	vec3 forward(0, 0, -1);
	vec3 up(0, 1, 0);

	vec3 globalPosition = GetGlobalPosition();

	mat4 view = glm::lookAt(globalPosition, globalPosition + forward, up);

	return view;
}

glm::mat4 LevelEditorCamera::GetProjectionMatrix(float w, float h)
{
	float aspect;
	if (h == 0.0f || w == 0.0f) aspect = 0.0f;
	else aspect = w / h;

	mat4 ortho = glm::ortho(
		aspect * -h / 2.0f + GetGlobalPosition().x,
		aspect *  h / 2.0f + GetGlobalPosition().x,
					-h / 2.0f + GetGlobalPosition().y,
					 h / 2.0f + GetGlobalPosition().y,
		nearClip,
		10.0f
	);

	return ortho;
}

void LevelEditorCamera::Initialise()
{
	Updater::UpdateAdd(this);
	gameObjectManager->Remove(this);
}