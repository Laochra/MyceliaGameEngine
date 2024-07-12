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
		if (selectedHandle != Handle::None && ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
		{
			selectedHandle = Handle::None;
			switch (mode)
			{
			case Mode::Translate: translate = TranslateData(); break;
			case Mode::Rotate: rotate = RotateData(); break;
			}
		}
		if (mode == Mode::Select) return;

		GameObject3D* target = dynamic_cast<GameObject3D*>(inspector->GetTarget());
		if (target == nullptr) return;


		vec3 axis, relativeRotateAxis;
		mat4 axisRelativeModelMatrix;
		switch (selectedHandle)
		{
		default: return;
		case Handle::X:
			axis = vec3(1, 0, 0);
			axisRelativeModelMatrix = glm::translate(glm::identity<mat4>(), target->GetGlobalPivot());
			axisRelativeModelMatrix = glm::rotate(axisRelativeModelMatrix, glm::radians(-90.0f), vec3(0, 1, 0));
			break;
		case Handle::Y:
			axis = vec3(0, 1, 0);
			axisRelativeModelMatrix = glm::translate(glm::identity<mat4>(), target->GetGlobalPivot());
			axisRelativeModelMatrix = glm::rotate(axisRelativeModelMatrix, glm::radians(90.0f), vec3(1, 0, 0));
			break;
		case Handle::Z:
			axis = vec3(0, 0, 1);
			axisRelativeModelMatrix = glm::translate(glm::identity<mat4>(), target->GetGlobalPivot());
			break;
		}
		Colour colour(axis.x * 0.5f + 0.5f, axis.y * 0.5f + 0.5f, axis.z * 0.5f + 0.5f);
		if (space == Space::Local)
		{
			switch (mode)
			{
			case Mode::Translate:
				axis = glm::normalize(vec3((mat4)target->GetGlobalRotationMatrix() * vec4(axis, 1)));
				axisRelativeModelMatrix = (mat4)target->GetGlobalRotationMatrix() * axisRelativeModelMatrix;
				break;
			case Mode::Rotate:
				relativeRotateAxis = axis;
				axis = glm::normalize(vec3(rotate.start * vec4(axis, 1)));
				axisRelativeModelMatrix = glm::toMat4(rotate.start) * axisRelativeModelMatrix;
				break;
			}
		}
		else
		{
			switch (mode)
			{
			case Mode::Rotate:
				relativeRotateAxis = glm::normalize(vec3(glm::inverse(rotate.start) * vec4(axis, 1)));
				break;
			}
		}

		switch (mode)
		{
		case Mode::Translate:
		{
			vec2 displacement = SceneGUI::normalisedMousePos - normalisedMouseStart;

			vec3 cameraSpaceDisplacement =
				vec3(glm::inverse(Camera::main->GetProjectionMatrix(screenWidth, screenHeight)) *
					vec4(displacement.x, displacement.y, 0.0f, 0.0f));

			vec3 cameraSpaceAxis =
				vec3(Camera::main->GetViewMatrix() *
					vec4(axis, 0.0f));

			float amountToMove = glm::dot(cameraSpaceAxis, cameraSpaceDisplacement);
			float distanceFactor = glm::length(translate.start - Camera::main->GetGlobalPosition());

			debug->lines.Add(
				translate.start,
				translate.start + axis * distanceFactor * amountToMove,
				colour
			);

			if (target->GetParent() == nullptr)
			{
				target->SetPosition((translate.relativeStart + axis * distanceFactor * amountToMove));
			}
			else
			{
				target->SetPosition((translate.relativeStart + axis * distanceFactor * amountToMove * (1.0f / target->GetParent()->GetGlobalScale())));
			}
			break;
		}
		case Mode::Rotate:
		{
			vec4 ray;
			vec4 rayOrigin;

			// Convert from clip space to view space (including perspective divide)
			mat4 clipToView = glm::inverse(Camera::main->GetProjectionMatrix(screenWidth, screenHeight));
			rayOrigin = clipToView * vec4(SceneGUI::normalisedMousePos, 0.0f, 1.0f);
			rayOrigin /= rayOrigin.w;

			// Convert from view space to world space and create rays
			mat4 viewToWorld = glm::inverse(Camera::main->GetViewMatrix());
			rayOrigin = viewToWorld * rayOrigin;
			ray = vec4(glm::normalize(Camera::main->GetGlobalPosition() - (vec3)rayOrigin), 0.0f);

			// Convert from view space to plane space
			mat4 worldToPlane = glm::inverse(axisRelativeModelMatrix);
			rayOrigin = worldToPlane * rayOrigin;
			ray = vec4(glm::normalize(vec3(worldToPlane * ray)), 0.0f);

			// Find the intersecting point on the plane
			vec3 currentCollisionPoint(0, 0, 0);
			if (ray.z < 0 != rayOrigin.z < 0)
			{
				// If the ray happens to be facing away from the plane
				// this will make it just *barely* face towards it.
				ray.z = ray.z < 0 ? 0.001f : -0.001f;
				ray = vec4(glm::normalize(vec3(ray)), 0.0f);
			}
			float currentMagnitude = rayOrigin.z / ray.z;
			currentCollisionPoint.x = -currentMagnitude * ray.x + rayOrigin.x;
			currentCollisionPoint.y = -currentMagnitude * ray.y + rayOrigin.y;

			// Get the direction from the centre of the object to the point of collision
			vec4 currentDirection = vec4(glm::normalize(currentCollisionPoint), 0.0f);
			rotate.currentDirection = vec4(glm::normalize(vec3(axisRelativeModelMatrix * currentDirection)), 0.0f);
			if (rotate.initialDirection == vec4(0.0f)) rotate.initialDirection = rotate.currentDirection;

			float amountToRotate = acos(std::clamp(glm::dot(rotate.initialDirection, rotate.currentDirection), -1.0f, 1.0f));
			float sign = glm::sign(glm::dot(axis, glm::cross((vec3)rotate.initialDirection, (vec3)rotate.currentDirection)));
			target->SetRotation(glm::rotate(rotate.relativeStart, amountToRotate * sign, vec3(vec4(relativeRotateAxis, 1.0f))));


			// Render lines to visualise directions
			// TODO: Do this with TransformEdit's Draw function so they are visible through objects
			float distanceFactor = glm::length(translate.start - Camera::main->GetGlobalPosition());
			float scale = distanceFactor * 0.25f;
			
			debug->lines.Add(target->GetGlobalPivot(), target->GetGlobalPivot() + vec3(rotate.initialDirection) * scale, colour);
			debug->lines.Add(target->GetGlobalPivot(), target->GetGlobalPivot() + vec3(rotate.currentDirection) * scale, colour);
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
		mat4 unscaledModelMatrix = modelTranslationMatrix * (mat4)target->GetRotationMatrix();

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

		if (coneMesh == nullptr) coneMesh == meshManager->GetMesh("ProceduralCone");
		if (cubeMesh == nullptr) cubeMesh = meshManager->GetMesh("ProceduralCube");
		if (ringMesh == nullptr)
		{
			ringMesh = new Mesh;
			ringMesh->InitialiseRing(32);
		}

		ShaderProgram* unlit = shaderManager->GetProgram("Unlit");
		unlit->Bind();

		mat4 modelTranslationMatrix = glm::translate(glm::identity<mat4>(), target->GetGlobalPivot());
		mat4 unscaledModelMatrix = modelTranslationMatrix * (mat4)target->GetRotationMatrix();

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
