#include "ParticleEmitterGUI.h"

#include "GUI.h"

void ParticleEmitterGUI::Draw()
{
	DrawParticleEmitterGUI((ParticleEmitter*)target);
}

void ParticleEmitterGUI::DrawParticleEmitterGUI(ParticleEmitter* particleEmitter)
{
	if (particleEmitter == nullptr || *particleEmitter == GameObject::Destroyed) return;

	GameObject3DGUI::DrawGameObject3DGUI(particleEmitter);

	std::string idStr = GUI::GenerateID("ParticleEmitter", particleEmitter);
	const char* id = idStr.c_str();

	if (ImGui::CollapsingHeader(id, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::BeginDisabled(particleEmitter->GetEmissionState() == ParticleSystem::Playing);
		if (ImGui::Button("Play"))
		{
			particleEmitter->SetEmissionState(ParticleSystem::Playing);
		}
		ImGui::EndDisabled();

		ImGui::BeginDisabled(particleEmitter->GetEmissionState() == ParticleSystem::Stopped);
		{
			ImGui::BeginDisabled(particleEmitter->GetEmissionState() == ParticleSystem::Paused);
			{
				ImGui::SameLine();
				if (ImGui::Button("Pause"))
				{
					particleEmitter->SetEmissionState(ParticleSystem::Paused);
				}
			}
			ImGui::EndDisabled();

			ImGui::SameLine();
			if (ImGui::Button("Stop"))
			{
				particleEmitter->SetEmissionState(ParticleSystem::Stopped);
			}

			ImGui::SameLine();
			if (ImGui::Button("Restart"))
			{
				particleEmitter->SetEmissionState(ParticleSystem::Stopped);
				particleEmitter->SetEmissionState(ParticleSystem::Playing);
			}
		}
		ImGui::EndDisabled();
		
		GUI::Spacing(3);

		ImGui::Indent();
		{
			ParticleSystem::Properties& properties = particleEmitter->GetParticleSystem()->properties;

			if (ImGui::CollapsingHeader("System Settings", ImGuiTreeNodeFlags_DefaultOpen))
			{
				static bool autoplay = false;
				ImGui::Checkbox("Autoplay?", &autoplay);
				ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3);

				static float duration = 0.0f;
				ImGui::DragFloat("Duration", &duration, 0.1f, 0.0f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
				ImGui::SameLine();
				GUI::HSpacing(3);
				ImGui::SameLine();
				static float delay = 0.0f;
				ImGui::DragFloat("Delay", &delay, 0.01f, -FLT_MAX, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);

				GUI::Spacing(3);

				if (ImGui::DragInt("Max Count", (int*)&properties.maxCount, 1, 0, 10'000'000, NULL, ImGuiSliderFlags_AlwaysClamp))
				{
					properties.startingCount = properties.maxCount;
				}

				GUI::Spacing(3);

				static float gravity = 0.0f;
				ImGui::DragFloat("Gravity", &gravity, 0.01f, -FLT_MAX, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
				ImGui::PopItemWidth();
			}
			if (ImGui::CollapsingHeader("Particle Settings", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3);
				static float size = 0.0f;
				ImGui::DragFloat("Size", &size, 0.1f, 0.0f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
				static float lifetime = 0.0f;
				ImGui::DragFloat("Lifetime", &lifetime, 0.1f, 0.0f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
				ImGui::PopItemWidth();
				static float colour[] = { 0.0f, 0.0f, 0.0f, 1.0f };
				ImGui::ColorEdit4("Colour", &colour[0], ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
			}
			if (ImGui::CollapsingHeader("Emission Settings", ImGuiTreeNodeFlags_DefaultOpen))
			{
				static bool emitOverTime = false;
				ImGui::Checkbox("Emit over time?", &emitOverTime);
				if (emitOverTime)
				{
					ImGui::PushItemWidth(ImGui::CalcItemWidth() / 4);
					ImGui::SameLine();
					GUI::HSpacing(3);
					ImGui::SameLine();
					static int particlesPerSecond = 1;
					ImGui::DragInt("Particles Per Second", &particlesPerSecond, 1, 1, INT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
					ImGui::PopItemWidth();
				}

				GUI::Spacing(3);

				enum class Shape { Sphere, Cone, Box, Quad, Circle };
				const char* shapeNames[] { "Sphere", "Cone", "Box", "Quad", "Circle" };
				static Shape shape = Shape::Sphere;
				if (ImGui::BeginCombo("Shape", shapeNames[(int)shape]))
				{
					for (int i = 0; i < 5; i++)
					{
						if (ImGui::Selectable(shapeNames[i], shape == (Shape)i))
						{
							shape = (Shape)i;
						}
					}

					ImGui::EndCombo();
				}

				ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3);
				static float shapeData[3]{};
				switch (shape)
				{
				case Shape::Sphere:
				{
					if (ImGui::DragFloat("##Inner Radius", &shapeData[0], 0.01f, 0.0f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
					{
						if (shapeData[0] > shapeData[1]) shapeData[1] = shapeData[0];
					}
					ImGui::SameLine();
					if (shapeData[1] < 0.01f) shapeData[1] = 0.01f;
					if (ImGui::DragFloat("##Outer Radius", &shapeData[1], 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
					{
						if (shapeData[1] < shapeData[0]) shapeData[0] = shapeData[1];
					}
					ImGui::SameLine();
					ImGui::Text("Inner/Outer Radius");

					shapeData[2] = 0.0f;
					break;
				}
				case Shape::Cone:
				{
					if (ImGui::DragFloat("##Inner Radius", &shapeData[0], 0.01f, 0.0f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
					{
						if (shapeData[0] > shapeData[1]) shapeData[1] = shapeData[0];
					}
					ImGui::SameLine();
					if (shapeData[1] < 0.01f) shapeData[1] = 0.01f;
					if (ImGui::DragFloat("##Outer Radius", &shapeData[1], 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
					{
						if (shapeData[1] < shapeData[0]) shapeData[0] = shapeData[1];
					}
					ImGui::SameLine();
					ImGui::Text("Inner/Outer Radius");

					ImGui::DragFloat("Angle", &shapeData[2], 0.1f, 0.0f, 89.9f, NULL, ImGuiSliderFlags_AlwaysClamp);
					break;
				}
				case Shape::Box:
				{
					if (shapeData[0] < 0.01f) shapeData[0] = 0.01f;
					ImGui::DragFloat("##Width", &shapeData[0], 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
					ImGui::SameLine();
					if (shapeData[1] < 0.01f) shapeData[1] = 0.01f;
					ImGui::DragFloat("##Height", &shapeData[1], 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
					ImGui::SameLine();
					if (shapeData[2] < 0.01f) shapeData[2] = 0.01f;
					ImGui::DragFloat("##Depth", &shapeData[2], 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
					ImGui::SameLine();
					ImGui::Text("Size");
					break;
				}
				case Shape::Quad:
				{
					if (shapeData[0] < 0.01f) shapeData[0] = 0.01f;
					ImGui::DragFloat("##Width", &shapeData[0], 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
					ImGui::SameLine();
					if (shapeData[1] < 0.01f) shapeData[1] = 0.01f;
					ImGui::DragFloat("##Height", &shapeData[1], 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
					shapeData[2] = 0.0f;
					ImGui::SameLine();
					ImGui::Text("Size");
					break;
				}
				case Shape::Circle:
				{
					if (ImGui::DragFloat("##Inner Radius", &shapeData[0], 0.01f, 0.0f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
					{
						if (shapeData[0] > shapeData[1]) shapeData[1] = shapeData[0];
					}
					ImGui::SameLine();
					if (shapeData[1] < 0.01f) shapeData[1] = 0.01f;
					if (ImGui::DragFloat("##Outer Radius", &shapeData[1], 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
					{
						if (shapeData[1] < shapeData[0]) shapeData[0] = shapeData[1];
					}
					ImGui::SameLine();
					ImGui::Text("Inner/Outer Radius");

					shapeData[2] = 0.0f;
					break;
				}
				}

				ImGui::PopItemWidth();
			}
		}
		ImGui::Unindent();
	}
}
