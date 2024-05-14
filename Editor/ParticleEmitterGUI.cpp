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
			typedef ParticleSystem::Shape Shape;

			if (ImGui::CollapsingHeader("System Settings", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Checkbox("Autoplay?", &properties.autoplay);
				ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3);

				ImGui::BeginDisabled();
				ImGui::DragFloat("Duration", &properties.duration, 0.1f, 0.0f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
				ImGui::DragFloat("Delay", &properties.delay, 0.01f, -FLT_MAX, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
				ImGui::EndDisabled();

				GUI::Spacing(3);

				ImGui::DragInt("Max Count", (int*)&properties.maxCount, 1, 0, 10'000'000, NULL, ImGuiSliderFlags_AlwaysClamp);

				GUI::Spacing(3);

				ImGui::BeginDisabled();
				ImGui::DragFloat("Gravity", &properties.gravity, 0.01f, -FLT_MAX, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
				ImGui::PopItemWidth();
				ImGui::EndDisabled();
			}
			if (ImGui::CollapsingHeader("Particle Settings", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3);
				ImGui::BeginDisabled();
				if (ImGui::DragFloat("##Size Min", &properties.sizeRange[0], 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
				{
					if (properties.sizeRange[0] > properties.sizeRange[1]) properties.sizeRange[1] = properties.sizeRange[0];
				}
				ImGui::SameLine();
				if (ImGui::DragFloat("##Size Max", &properties.sizeRange[1], 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
				{
					if (properties.sizeRange[1] < properties.sizeRange[0]) properties.sizeRange[0] = properties.sizeRange[1];
				}
				ImGui::SameLine();
				ImGui::Text("Size Range");
				ImGui::EndDisabled();

				if (ImGui::DragFloat("##Speed Min", &properties.speedRange[0], 0.01f, 0.0f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
				{
					if (properties.speedRange[0] > properties.speedRange[1]) properties.speedRange[1] = properties.speedRange[0];
				}
				ImGui::SameLine();
				if (ImGui::DragFloat("##Speed Max", &properties.speedRange[1], 0.01f, 0.0f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
				{
					if (properties.speedRange[1] < properties.speedRange[0]) properties.speedRange[0] = properties.speedRange[1];
				}
				ImGui::SameLine();
				ImGui::Text("Speed Range");

				ImGui::BeginDisabled();
				if (ImGui::DragFloat("##Lifetime Min", &properties.lifetimeRange[0], 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
				{
					if (properties.lifetimeRange[0] > properties.lifetimeRange[1]) properties.lifetimeRange[1] = properties.lifetimeRange[0];
				}
				ImGui::SameLine();
				if (ImGui::DragFloat("##Lifetime Max", &properties.lifetimeRange[1], 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
				{
					if (properties.lifetimeRange[1] < properties.lifetimeRange[0]) properties.lifetimeRange[0] = properties.lifetimeRange[1];
				}
				ImGui::SameLine();
				ImGui::Text("Lifetime Range");
				ImGui::PopItemWidth();
				ImGui::EndDisabled();
				ImGui::ColorEdit4("Colour", &properties.colour[0], ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
			}
			if (ImGui::CollapsingHeader("Emission Settings", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::BeginDisabled();
				ImGui::Checkbox("Emit over time?", &properties.emitOverTime);
				if (properties.emitOverTime)
				{
					ImGui::PushItemWidth(ImGui::CalcItemWidth() / 4);
					ImGui::SameLine();
					GUI::HSpacing(3);
					ImGui::SameLine();
					ImGui::DragInt("Particles Per Second", (int*)&properties.particlesPerSecond, 1, 1, INT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
					ImGui::PopItemWidth();
				}

				ImGui::EndDisabled();

				GUI::Spacing(3);

				const char* shapeNames[] { "Sphere", "Cone", "Box", "Quad", "Circle" };
				if (ImGui::BeginCombo("Shape", shapeNames[(int)properties.shape]))
				{
					for (int i = 0; i < 5; i++)
					{
						if (ImGui::Selectable(shapeNames[i], properties.shape == (Shape)i))
						{
							properties.shape = (Shape)i;
						}
					}

					ImGui::EndCombo();
				}

				ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3);
				switch (properties.shape)
				{
				case Shape::Sphere:
				{
					if (ImGui::DragFloat("##Inner Radius", &properties.shapeData[0], 0.01f, 0.0f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
					{
						if (properties.shapeData[0] > properties.shapeData[1]) properties.shapeData[1] = properties.shapeData[0];
					}
					ImGui::SameLine();
					if (properties.shapeData[1] < 0.01f) properties.shapeData[1] = 1.0f;
					if (ImGui::DragFloat("##Outer Radius", &properties.shapeData[1], 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
					{
						if (properties.shapeData[1] < properties.shapeData[0]) properties.shapeData[0] = properties.shapeData[1];
					}
					ImGui::SameLine();
					ImGui::Text("Inner/Outer Radius");

					properties.shapeData[2] = 0.0f;
					break;
				}
				case Shape::Cone:
				{
					if (ImGui::DragFloat("##Inner Radius", &properties.shapeData[0], 0.01f, 0.0f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
					{
						if (properties.shapeData[0] > properties.shapeData[1]) properties.shapeData[1] = properties.shapeData[0];
					}
					ImGui::SameLine();
					if (properties.shapeData[1] < 0.01f) properties.shapeData[1] = 1.0f;
					if (ImGui::DragFloat("##Outer Radius", &properties.shapeData[1], 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
					{
						if (properties.shapeData[1] < properties.shapeData[0]) properties.shapeData[0] = properties.shapeData[1];
					}
					ImGui::SameLine();
					ImGui::Text("Inner/Outer Radius");

					ImGui::DragFloat("Angle", &properties.shapeData[2], 0.1f, 0.0f, 89.9f, NULL, ImGuiSliderFlags_AlwaysClamp);
					break;
				}
				case Shape::Box:
				{
					if (properties.shapeData[0] < 0.01f) properties.shapeData[0] = 1.0f;
					ImGui::DragFloat("##Width", &properties.shapeData[0], 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
					ImGui::SameLine();
					if (properties.shapeData[1] < 0.01f) properties.shapeData[1] = 1.0f;
					ImGui::DragFloat("##Height", &properties.shapeData[1], 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
					ImGui::SameLine();
					if (properties.shapeData[2] < 0.01f) properties.shapeData[2] = 1.0f;
					ImGui::DragFloat("##Depth", &properties.shapeData[2], 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
					ImGui::SameLine();
					ImGui::Text("Size");
					break;
				}
				case Shape::Quad:
				{
					if (properties.shapeData[0] < 0.01f) properties.shapeData[0] = 1.0f;
					ImGui::DragFloat("##Width", &properties.shapeData[0], 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
					ImGui::SameLine();
					if (properties.shapeData[1] < 0.01f) properties.shapeData[1] = 1.0f;
					ImGui::DragFloat("##Height", &properties.shapeData[1], 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
					properties.shapeData[2] = 0.0f;
					ImGui::SameLine();
					ImGui::Text("Size");
					break;
				}
				case Shape::Circle:
				{
					if (ImGui::DragFloat("##Inner Radius", &properties.shapeData[0], 0.01f, 0.0f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
					{
						if (properties.shapeData[0] > properties.shapeData[1]) properties.shapeData[1] = properties.shapeData[0];
					}
					ImGui::SameLine();
					if (properties.shapeData[1] < 0.01f) properties.shapeData[1] = 1.0f;
					if (ImGui::DragFloat("##Outer Radius", &properties.shapeData[1], 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
					{
						if (properties.shapeData[1] < properties.shapeData[0]) properties.shapeData[0] = properties.shapeData[1];
					}
					ImGui::SameLine();
					ImGui::Text("Inner/Outer Radius");

					properties.shapeData[2] = 0.0f;
					break;
				}
				}

				ImGui::PopItemWidth();
			}
		}
		ImGui::Unindent();
	}
}
