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
		if (ImGui::CollapsingHeader("Properties", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3);

			ParticleSystem::Properties& properties = particleEmitter->GetParticleSystem()->properties;
			if (ImGui::DragInt("Max Count", (int*)&properties.maxCount, 1, 0, 10'000'000, NULL, ImGuiSliderFlags_AlwaysClamp))
			{
				if (properties.startingCount > properties.maxCount) properties.startingCount = properties.maxCount;
			}

			ImGui::DragInt("Starting Count", (int*)&properties.startingCount, 1, 0, properties.maxCount, NULL, ImGuiSliderFlags_AlwaysClamp);

			ImGui::PopItemWidth();
		}
		ImGui::Unindent();
	}
}
