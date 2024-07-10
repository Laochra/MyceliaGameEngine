#include "TransformEdit.h"

#include "Inspector.h"
#include "SceneGUI.h"

#include "GameObject3D.h"
#include "Camera.h"

#include "MeshManager.h"
#include "ShaderManager.h"
#include "TextureManager.h"

#include "Debug.h"

typedef unsigned long long ulong;

namespace TransformEdit
{
	Space space = Space::Global;
	Mode mode = Mode::Translate;
	Handle selectedHandle = Handle::None;

	vec2 normalisedMouseStart = { 0.0f, 0.0f };

	vec3 targetStartPos = { 0.0f, 0.0f, 0.0f };
	vec3 targetStartPosRelative = { 0.0f, 0.0f, 0.0f };

	Mesh* coneMesh;
	Mesh* cubeMesh;

	void Update() noexcept
	{
		if (selectedHandle != Handle::None)
		{
			if (ImGui::IsKeyReleased(ImGuiKey_MouseLeft)) selectedHandle = Handle::None;
		}
		if (mode == Mode::Select) return;

		GameObject3D* target = dynamic_cast<GameObject3D*>(inspector->GetTarget());
		if (target == nullptr) return;


		vec3 axis;
		switch (selectedHandle)
		{
		default: return;
		case Handle::X: axis = { 1, 0, 0 }; break;
		case Handle::Y: axis = { 0, 1, 0 }; break;
		case Handle::Z: axis = { 0, 0, 1 }; break;
		}
		Colour colour(axis.x * 0.5f + 0.5f, axis.y * 0.5f + 0.5f, axis.z * 0.5f + 0.5f);
		if (space == Space::Local)
		{
			axis = glm::normalize(vec3((mat4)target->GetGlobalRotationMatrix() * vec4(axis, 1)));
		}

		vec2 displacement = SceneGUI::normalisedMousePos - normalisedMouseStart;

		vec3 cameraSpaceDisplacement =
			vec3(glm::inverse(Camera::main->GetProjectionMatrix(screenWidth, screenHeight)) *
			vec4(displacement.x, displacement.y, 0.0f, 0.0f));

		vec3 cameraSpaceAxis =
			vec3(Camera::main->GetViewMatrix() *
			vec4(axis, 0.0f));

		float amountToMove = glm::dot(cameraSpaceAxis, cameraSpaceDisplacement);
		float distanceFactor = glm::length(targetStartPos - Camera::main->GetGlobalPosition());

		debug->lines.Add(
			targetStartPos,
			targetStartPos + axis * distanceFactor * amountToMove,
			colour
		);

		if (target->GetParent() == nullptr)
		{
			target->SetPosition((targetStartPosRelative + axis * distanceFactor * amountToMove));
		}
		else
		{
			target->SetPosition((targetStartPosRelative + axis * distanceFactor * amountToMove * (1.0f / target->GetParent()->GetGlobalScale())));
		}
	}

	void DrawIDs() noexcept
	{
		if (mode == Mode::Select) return;
		if (selectedHandle != Handle::None) return;
		GameObject3D* target = dynamic_cast<GameObject3D*>(inspector->GetTarget());
		if (target == nullptr) return;

		if (coneMesh == nullptr) coneMesh = meshManager->GetMesh("ProceduralCone");

		ShaderProgram* guidProgram = shaderManager->GetProgram("DrawGUID");
		guidProgram->Bind();

		mat4 modelTranslationMatrix = glm::translate(glm::identity<mat4>(), target->GetGlobalPivot());
		mat4 unscaledModelMatrix = modelTranslationMatrix * (mat4)target->GetRotationMatrix();

		float distanceFactor = 0.25f * glm::length(target->GetGlobalPivot() - Camera::main->GetGlobalPosition());
		vec3 scale = vec3(distanceFactor * 0.2f);

		if (mode == Mode::Translate)
		{
			// X Arrow
			{
				mat4 coneOffset = glm::translate(glm::identity<mat4>(), vec3(1, 0, 0) * distanceFactor);
				coneOffset = glm::rotate(coneOffset, glm::radians(-90.0f), vec3(0, 1, 0));
				coneOffset = glm::scale(coneOffset, scale);
				guidProgram->BindUniform("ProjectionViewModel",
					Camera::main->GetPVMatrix() *
					(space == Space::Local ? unscaledModelMatrix : modelTranslationMatrix) *
					coneOffset);

				ulong id = (ulong)Handle::X;
				guidProgram->BindUniform("GUID", *(glm::uvec2*)&id);

				coneMesh->Draw();
			}
			// Y Arrow
			{
				mat4 coneOffset = glm::translate(glm::identity<mat4>(), vec3(0, 1, 0) * distanceFactor);
				coneOffset = glm::rotate(coneOffset, glm::radians(90.0f), vec3(1, 0, 0));
				coneOffset = glm::scale(coneOffset, scale);
				guidProgram->BindUniform("ProjectionViewModel",
					Camera::main->GetPVMatrix() *
					(space == Space::Local ? unscaledModelMatrix : modelTranslationMatrix) *
					coneOffset);

				ulong id = (ulong)Handle::Y;
				guidProgram->BindUniform("GUID", *(glm::uvec2*)&id);

				coneMesh->Draw();
			}
			// Y Arrow
			{
				mat4 coneOffset = glm::translate(glm::identity<mat4>(), vec3(0, 0, 1) * distanceFactor);
				coneOffset = glm::rotate(coneOffset, glm::radians(180.0f), vec3(0, 1, 0));
				coneOffset = glm::scale(coneOffset, scale);
				guidProgram->BindUniform("ProjectionViewModel",
					Camera::main->GetPVMatrix() *
					(space == Space::Local ? unscaledModelMatrix : modelTranslationMatrix) *
					coneOffset);

				ulong id = (ulong)Handle::Z;
				guidProgram->BindUniform("GUID", *(glm::uvec2*)&id);

				coneMesh->Draw();
			}
		}
	}
	void Draw() noexcept
	{
		GameObject3D* target = dynamic_cast<GameObject3D*>(inspector->GetTarget());
		if (target == nullptr) return;

		if (coneMesh == nullptr) coneMesh == meshManager->GetMesh("ProceduralCone");
		if (cubeMesh == nullptr) cubeMesh = meshManager->GetMesh("ProceduralCube");

		ShaderProgram* unlit = shaderManager->GetProgram("Unlit");
		unlit->Bind();

		mat4 modelTranslationMatrix = glm::translate(glm::identity<mat4>(), target->GetGlobalPivot());
		mat4 unscaledModelMatrix = modelTranslationMatrix * (mat4)target->GetRotationMatrix();

		float distanceFactor = 0.25f * glm::length(target->GetGlobalPivot() - Camera::main->GetGlobalPosition());
		vec3 scale = vec3(distanceFactor * 0.2f);

		if (mode == Mode::Translate)
		{
			if (selectedHandle == Handle::X || selectedHandle == Handle::None)
			{
				mat4 coneOffset = glm::translate(glm::identity<mat4>(), vec3(1, 0, 0) * distanceFactor);
				coneOffset = glm::rotate(coneOffset, glm::radians(-90.0f), vec3(0, 1, 0));
				coneOffset = glm::scale(coneOffset, scale);
				unlit->BindUniform("ProjectionViewModel",
					Camera::main->GetPVMatrix() *
					(space == Space::Local ? unscaledModelMatrix : modelTranslationMatrix) *
					coneOffset);

				textureManager->GetTexture("DefaultColour", Texture::NonLinear)->Bind(0);
				unlit->BindUniform("ColourMap", 0);

				unlit->BindUniform("ColourTint", vec3(1, 0, 0));

				coneMesh->Draw();
			}
			if (selectedHandle == Handle::Y || selectedHandle == Handle::None)
			{
				mat4 coneOffset = glm::translate(glm::identity<mat4>(), vec3(0, 1, 0) * distanceFactor);
				coneOffset = glm::rotate(coneOffset, glm::radians(90.0f), vec3(1, 0, 0));
				coneOffset = glm::scale(coneOffset, scale);
				unlit->BindUniform("ProjectionViewModel",
					Camera::main->GetPVMatrix() *
					(space == Space::Local ? unscaledModelMatrix : modelTranslationMatrix) *
					coneOffset);

				textureManager->GetTexture("DefaultColour", Texture::NonLinear)->Bind(0);
				unlit->BindUniform("ColourMap", 0);

				unlit->BindUniform("ColourTint", vec3(0, 1, 0));

				coneMesh->Draw();
			}
			if (selectedHandle == Handle::Z || selectedHandle == Handle::None)
			{
				mat4 coneOffset = glm::translate(glm::identity<mat4>(), vec3(0, 0, 1) * distanceFactor);
				coneOffset = glm::rotate(coneOffset, glm::radians(180.0f), vec3(0, 1, 0));
				coneOffset = glm::scale(coneOffset, scale);
				unlit->BindUniform("ProjectionViewModel",
					Camera::main->GetPVMatrix() *
					(space == Space::Local ? unscaledModelMatrix : modelTranslationMatrix) *
					coneOffset);

				textureManager->GetTexture("DefaultColour", Texture::NonLinear)->Bind(0);
				unlit->BindUniform("ColourMap", 0);

				unlit->BindUniform("ColourTint", vec3(0, 0, 1));

				coneMesh->Draw();
			}
		}

		// Centre
		{
			unlit->BindUniform("ProjectionViewModel",
				Camera::main->GetPVMatrix() *
				glm::scale(modelTranslationMatrix, scale * 0.5f)
			);

			textureManager->GetTexture("DefaultColour", Texture::NonLinear)->Bind(0);
			unlit->BindUniform("ColourMap", 0);

			unlit->BindUniform("ColourTint", vec3(1, 1, 1));

			cubeMesh->Draw();
		}
	}

	void BeginTransform(Handle handle, vec2 normalisedMousePos) noexcept
	{
		GameObject3D* target = dynamic_cast<GameObject3D*>(inspector->GetTarget());
		if (target == nullptr) return;

		selectedHandle = handle;
		normalisedMouseStart = normalisedMousePos;

		if (mode == Mode::Translate)
		{
			targetStartPos = target->GetGlobalPivot();
			if (target->GetParent() == nullptr)
			{
				targetStartPosRelative = targetStartPos;
			}
			else
			{
				targetStartPosRelative = target->GetPosition();
			}
		}
	}
	void CancelTransform() noexcept
	{
		if (selectedHandle == Handle::None) return;
		GameObject3D* target = dynamic_cast<GameObject3D*>(inspector->GetTarget());
		if (target == nullptr) return;

		if (mode == Mode::Translate)
		{
			target->SetPosition(targetStartPosRelative);
		}

		selectedHandle = Handle::None;
	}
}
