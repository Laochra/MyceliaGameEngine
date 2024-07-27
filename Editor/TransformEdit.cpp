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

	struct TranslateData
	{
		vec3 start = { 0.0f, 0.0f, 0.0f };
		vec3 relativeStart = { 0.0f, 0.0f, 0.0f };
	};
	TranslateData translate;

	struct RotateData
	{
		quat start = quat();
		quat relativeStart = quat();

		vec4 initialDirection = { 0.0f, 0.0f, 0.0f, 0.0f };
		vec4 currentDirection = { 0.0f, 0.0f, 0.0f, 0.0f };
	};
	RotateData rotate;

	Mesh* coneMesh;
	Mesh* cubeMesh;
	Mesh* ringMesh;

	void Update() noexcept
	{
		if (mode == Mode::Select) return;

		GameObject3D* target = dynamic_cast<GameObject3D*>(inspector->GetTarget());
		if (target == nullptr) return;

		if (selectedHandle != Handle::None && ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
		{
			selectedHandle = Handle::None;
			switch (mode)
			{
			case Mode::Translate: // Temporarily set back the position so we can track the change with EditHistory
				vec3 newPosition = target->GetPosition();
				target->SetPosition(translate.relativeStart);

				GUI::editHistory.Begin(target);
				target->SetPosition(newPosition);
				GUI::editHistory.End();

				translate = TranslateData();
				break;
			case Mode::Rotate: // Temporarily set back the rotation so we can track the change with EditHistory
				quat newRotation = target->GetRotationQuat();
				target->SetRotation(rotate.relativeStart);

				GUI::editHistory.Begin(target);
				target->SetRotation(newRotation);
				GUI::editHistory.End();

				rotate = RotateData();
				break;
			}
		}


		vec3 relativeAxis;
		mat4 axisModelMatrix;
		switch (selectedHandle)
		{
		default:
			return;
		case Handle::X:
			relativeAxis = vec3(1, 0, 0);
			axisModelMatrix = glm::rotate(glm::identity<mat4>(), glm::radians(-90.0f), vec3(0, 1, 0));
			break;
		case Handle::Y:
			relativeAxis = vec3(0, 1, 0);
			axisModelMatrix = glm::rotate(glm::identity<mat4>(), glm::radians(90.0f), vec3(1, 0, 0));
			break;
		case Handle::Z:
			relativeAxis = vec3(0, 0, 1);
			axisModelMatrix = glm::rotate(glm::identity<mat4>(), glm::radians(180.0f), vec3(0, 1, 0));
			break;
		}
		Colour colour(relativeAxis.x * 0.5f + 0.5f, relativeAxis.y * 0.5f + 0.5f, relativeAxis.z * 0.5f + 0.5f);
		if (space == Space::Local)
		{
			switch (mode)
			{
			case Mode::Translate:
				axisModelMatrix = (mat4)target->GetGlobalRotationMatrix() * axisModelMatrix;
				relativeAxis = glm::normalize(vec3((mat4)target->GetRotationMatrix() * vec4(relativeAxis, 1)));
				break;
			case Mode::Rotate:
				mat4 globalTranslation = glm::translate(glm::identity<mat4>(), target->GetGlobalPivot());
				mat4 globalRotation = glm::toMat4(rotate.start) * axisModelMatrix;
				axisModelMatrix = globalTranslation * globalRotation;
				break;
			}
		}
		else
		{
			switch (mode)
			{
			case Mode::Translate:
				if (target->GetParent() != nullptr)
				{
					relativeAxis = glm::normalize(vec3(glm::inverse(target->GetParent()->GetGlobalRotationMatrix()) * vec4(relativeAxis, 1)));
				}
				break;
			case Mode::Rotate:
				axisModelMatrix = glm::translate(glm::identity<mat4>(), target->GetGlobalPivot()) * axisModelMatrix;
				relativeAxis = glm::normalize(vec3(glm::inverse(rotate.start) * vec4(relativeAxis, 1)));
				break;
			}
		}

		switch (mode)
		{
		case Mode::Translate:
		{
			vec2 displacement = SceneGUI::normalisedMousePos - normalisedMouseStart;

			vec3 cameraSpaceDisplacement =
				vec3(glm::inverse(Camera::main->GetProjectionMatrix((float)screenWidth, (float)screenHeight)) *
					vec4(displacement.x, displacement.y, 0.0f, 0.0f));

			vec3 cameraSpaceAxis =
				vec3(Camera::main->GetViewMatrix() *
					vec4(-(vec3)axisModelMatrix[2], 0.0f));

			float amountToMove = glm::dot(cameraSpaceAxis, cameraSpaceDisplacement);
			float distanceFactor = glm::length(translate.start - Camera::main->GetGlobalPosition());

			AppInfo::debug->lines.Add(
				translate.start,
				translate.start + -(vec3)axisModelMatrix[2] * distanceFactor * amountToMove,
				colour
			);

			if (target->GetParent() == nullptr)
			{
				target->SetPosition((translate.relativeStart + relativeAxis * distanceFactor * amountToMove));
			}
			else
			{
				target->SetPosition((translate.relativeStart + relativeAxis * distanceFactor * amountToMove * (1.0f / target->GetParent()->GetGlobalScale())));
			}
			break;
		}
		case Mode::Rotate:
		{
			vec4 ray;
			vec4 rayOrigin;

			// Convert from clip space to view space (including perspective divide)
			mat4 clipToView = glm::inverse(Camera::main->GetProjectionMatrix((float)screenWidth, (float)screenHeight));
			rayOrigin = clipToView * vec4(SceneGUI::normalisedMousePos, 0.0f, 1.0f);
			rayOrigin /= rayOrigin.w;

			// Convert from view space to world space and create rays
			mat4 viewToWorld = glm::inverse(Camera::main->GetViewMatrix());
			rayOrigin = viewToWorld * rayOrigin;
			ray = vec4(glm::normalize(Camera::main->GetGlobalPosition() - (vec3)rayOrigin), 0.0f);

			// Convert from view space to plane space
			mat4 worldToPlane = glm::inverse(axisModelMatrix);
			rayOrigin = worldToPlane * rayOrigin;
			ray = vec4(glm::normalize(vec3(worldToPlane * ray)), 0.0f);

			// Find the intersecting point on the plane
			vec3 currentCollisionPoint(0, 0, 0);
			bool broughtBackOntoPlane = false;
			if (ray.z < 0 != rayOrigin.z < 0)
			{
				broughtBackOntoPlane = true;
				// If the ray happens to be facing away from the plane
				// this will make it just *barely* face towards it.
				ray.z = ray.z < 0 ? 0.001f : -0.001f;
				ray = vec4(glm::normalize(vec3(ray)), 0.0f);
			}
			float currentMagnitude = rayOrigin.z / ray.z;
			currentCollisionPoint.x = -currentMagnitude * ray.x + rayOrigin.x;
			currentCollisionPoint.y = -currentMagnitude * ray.y + rayOrigin.y;

			// Get the direction from the centre of the object to the point of collision
			vec3 worldSpaceCollisionPoint = vec3(axisModelMatrix * vec4(currentCollisionPoint, 1.0f));
			rotate.currentDirection = vec4(glm::normalize(worldSpaceCollisionPoint - target->GetGlobalPivot()), 0.0f);
			if (rotate.initialDirection == vec4(0.0f)) rotate.initialDirection = rotate.currentDirection;

			float amountToRotate = acos(std::clamp(glm::dot((vec3)rotate.initialDirection, (vec3)rotate.currentDirection), -1.0f, 1.0f));
			float sign = glm::sign(glm::dot(-(vec3)axisModelMatrix[2], glm::cross((vec3)rotate.initialDirection, (vec3)rotate.currentDirection)));
			target->SetRotation(glm::rotate(rotate.relativeStart, amountToRotate * sign, vec3(vec4(relativeAxis, 1.0f))));


			// Render lines to visualise directions
			// TODO: Do this with TransformEdit's Draw function so they are visible through objects
			float distanceFactor = glm::length(target->GetGlobalPivot() - Camera::main->GetGlobalPosition());
			float scale = distanceFactor * 0.25f;
			
			AppInfo::debug->lines.Add(target->GetGlobalPivot(), target->GetGlobalPivot() + vec3(rotate.initialDirection) * scale, colour);
			AppInfo::debug->lines.Add(target->GetGlobalPivot(), target->GetGlobalPivot() + vec3(rotate.currentDirection) * scale, colour);
			
			break;
		}
		}
	}

	void DrawIDs() noexcept
	{
		glLineWidth(10.0f);

		if (mode == Mode::Select) return;
		if (selectedHandle != Handle::None) return;
		GameObject3D* target = dynamic_cast<GameObject3D*>(inspector->GetTarget());
		if (target == nullptr) return;

		if (coneMesh == nullptr) coneMesh = meshManager->GetMesh("ProceduralCone");
		if (ringMesh == nullptr)
		{
			ringMesh = new Mesh;
			ringMesh->InitialiseRing(32);
		}

		ShaderProgram* guidProgram = shaderManager->GetProgram("DrawGUID");
		guidProgram->Bind();

		mat4 modelTranslationMatrix = glm::translate(glm::identity<mat4>(), target->GetGlobalPivot());
		mat4 unscaledModelMatrix = modelTranslationMatrix * (mat4)target->GetGlobalRotationMatrix();

		float distanceFactor = 0.25f * glm::length(target->GetGlobalPivot() - Camera::main->GetGlobalPosition());
		
		switch (mode)
		{
		case Mode::Translate:
		{
			vec3 scale = vec3(distanceFactor * 0.2f);

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
			// Z Arrow
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
			break;
		}
		case Mode::Rotate:
		{
			vec3 scale = vec3(distanceFactor);

			// X Ring
			{
				mat4 ringOffset = glm::rotate(glm::identity<mat4>(), glm::radians(-90.0f), vec3(0, 1, 0));
				ringOffset = glm::scale(ringOffset, scale);
				guidProgram->BindUniform("ProjectionViewModel",
					Camera::main->GetPVMatrix() *
					(space == Space::Local ? unscaledModelMatrix : modelTranslationMatrix) *
					ringOffset);

				ulong id = (ulong)Handle::X;
				guidProgram->BindUniform("GUID", *(glm::uvec2*)&id);

				ringMesh->Draw();
			}
			// Y Ring
			{
				mat4 ringOffset = glm::rotate(glm::identity<mat4>(), glm::radians(90.0f), vec3(1, 0, 0));
				ringOffset = glm::scale(ringOffset, scale);
				guidProgram->BindUniform("ProjectionViewModel",
					Camera::main->GetPVMatrix() *
					(space == Space::Local ? unscaledModelMatrix : modelTranslationMatrix) *
					ringOffset);

				ulong id = (ulong)Handle::Y;
				guidProgram->BindUniform("GUID", *(glm::uvec2*)&id);

				ringMesh->Draw();
			}
			// Z Ring
			{
				mat4 ringOffset = glm::scale(glm::identity<mat4>(), scale);
				guidProgram->BindUniform("ProjectionViewModel",
					Camera::main->GetPVMatrix() *
					(space == Space::Local ? unscaledModelMatrix : modelTranslationMatrix) *
					ringOffset);

				ulong id = (ulong)Handle::Z;
				guidProgram->BindUniform("GUID", *(glm::uvec2*)&id);

				ringMesh->Draw();
			}
			break;
		}
		}
	}
	void Draw() noexcept
	{
		glLineWidth(5.0f);

		GameObject3D* target = dynamic_cast<GameObject3D*>(inspector->GetTarget());
		if (target == nullptr) return;

		if (coneMesh == nullptr) coneMesh = meshManager->GetMesh("ProceduralCone");
		if (cubeMesh == nullptr) cubeMesh = meshManager->GetMesh("ProceduralCube");
		if (ringMesh == nullptr)
		{
			ringMesh = new Mesh;
			ringMesh->InitialiseRing(32);
		}

		ShaderProgram* unlit = shaderManager->GetProgram("Unlit");
		unlit->Bind();

		mat4 modelTranslationMatrix = glm::translate(glm::identity<mat4>(), target->GetGlobalPivot());
		mat4 unscaledModelMatrix = modelTranslationMatrix * (mat4)target->GetGlobalRotationMatrix();

		float distanceFactor = 0.25f * glm::length(target->GetGlobalPivot() - Camera::main->GetGlobalPosition());
		
		switch (mode)
		{
		case Mode::Translate:
		{
			vec3 scale(distanceFactor * 0.2f);

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
			break;
		}
		case Mode::Rotate:
		{
			vec3 scale(distanceFactor);

			if (selectedHandle == Handle::X || selectedHandle == Handle::None)
			{
				mat4 ringOffset = glm::rotate(glm::identity<mat4>(), glm::radians(-90.0f), vec3(0, 1, 0));
				ringOffset = glm::scale(ringOffset, scale);
				unlit->BindUniform("ProjectionViewModel",
					Camera::main->GetPVMatrix() *
					(space == Space::Local ? unscaledModelMatrix : modelTranslationMatrix) *
					ringOffset);

				textureManager->GetTexture("DefaultColour", Texture::NonLinear)->Bind(0);
				unlit->BindUniform("ColourMap", 0);

				unlit->BindUniform("ColourTint", vec3(1, 0, 0));

				ringMesh->Draw();
			}
			if (selectedHandle == Handle::Y || selectedHandle == Handle::None)
			{
				mat4 ringOffset = glm::rotate(glm::identity<mat4>(), glm::radians(90.0f), vec3(1, 0, 0));
				ringOffset = glm::scale(ringOffset, scale);
				unlit->BindUniform("ProjectionViewModel",
					Camera::main->GetPVMatrix() *
					(space == Space::Local ? unscaledModelMatrix : modelTranslationMatrix) *
					ringOffset);

				textureManager->GetTexture("DefaultColour", Texture::NonLinear)->Bind(0);
				unlit->BindUniform("ColourMap", 0);

				unlit->BindUniform("ColourTint", vec3(0, 1, 0));

				ringMesh->Draw();
			}
			if (selectedHandle == Handle::Z || selectedHandle == Handle::None)
			{
				mat4 ringOffset = glm::scale(glm::identity<mat4>(), scale);
				unlit->BindUniform("ProjectionViewModel",
					Camera::main->GetPVMatrix() *
					(space == Space::Local ? unscaledModelMatrix : modelTranslationMatrix) *
					ringOffset);

				textureManager->GetTexture("DefaultColour", Texture::NonLinear)->Bind(0);
				unlit->BindUniform("ColourMap", 0);

				unlit->BindUniform("ColourTint", vec3(0, 0, 1));

				ringMesh->Draw();
			}
			break;
		}
		}
		
		// Centre
		{
			vec3 scale(distanceFactor * 0.1f);

			unlit->BindUniform("ProjectionViewModel",
				Camera::main->GetPVMatrix() *
				glm::scale(modelTranslationMatrix, scale)
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

		switch (mode)
		{
		case Mode::Translate:
		{
			translate.start = target->GetGlobalPivot();
			if (target->GetParent() == nullptr)
			{
				translate.relativeStart = translate.start;
			}
			else
			{
				translate.relativeStart = target->GetPosition();
			}
			break;
		}
		case Mode::Rotate:
		{
			rotate.start = target->GetGlobalRotationQuat();
			if (target->GetParent() == nullptr)
			{
				rotate.relativeStart = rotate.start;
			}
			else
			{
				rotate.relativeStart = target->GetRotationQuat();
			}
			break;
		}
		}
	}
	void CancelTransform() noexcept
	{
		if (selectedHandle == Handle::None) return;
		GameObject3D* target = dynamic_cast<GameObject3D*>(inspector->GetTarget());
		if (target != nullptr)
		{
			switch (mode)
			{
			case Mode::Translate:
			{
				target->SetPosition(translate.relativeStart);
				break;
			}
			case Mode::Rotate:
			{
				target->SetRotation(rotate.relativeStart);
				break;
			}
			}
		}

		selectedHandle = Handle::None;
		translate = TranslateData();
		rotate = RotateData();
	}
}
