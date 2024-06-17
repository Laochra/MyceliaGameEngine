#include "ParticleEmitterGUI.h"

#include "GUI.h"

#include <format>

void ParticleEmitterGUI::Draw()
{
	DrawParticleEmitterGUI((ParticleEmitter*)target);
}

void ParticleEmitterGUI::DrawParticleEmitterGUI(ParticleEmitter* particleEmitter)
{
	if (particleEmitter == nullptr || *particleEmitter == GameObject::Destroyed) return;

	GameObject3DGUI::DrawGameObject3DGUI(particleEmitter);

	std::string idStr = GUI::GenerateID(ParticleEmitter::className, particleEmitter);
	const char* id = idStr.c_str();

	if (ImGui::CollapsingHeader(id, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ParticleSystem::Properties& properties = particleEmitter->GetParticleSystem()->properties;

		ImGui::BeginDisabled(particleEmitter->GetEmissionState() == ParticleSystem::Playing);
		{
			if (ImGui::Button("Play"))
			{
				if (properties.delay > 0.0f) particleEmitter->SetEmissionState(ParticleSystem::WaitingToStart);
				else particleEmitter->SetEmissionState(ParticleSystem::Playing);
			}
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
				if (properties.delay > 0.0f) particleEmitter->SetEmissionState(ParticleSystem::WaitingToStart);
				else particleEmitter->SetEmissionState(ParticleSystem::Playing);
			}

			if (particleEmitter->GetEmissionState() == ParticleSystem::Playing ||
				 particleEmitter->GetEmissionState() == ParticleSystem::WaitingToStart)
			{
				ImGui::SameLine();
				float elapsedTime = particleEmitter->GetParticleSystem()->GetElapsedTime() - properties.delay;
				string text = std::format("{:.2f}", elapsedTime) + " / " + std::format("{:.2f}", properties.duration) + "s";
				ImGui::Text(text.c_str());
			}
		}
		ImGui::EndDisabled();
		
		GUI::Spacing(3);

		ImGui::Indent();
		{
			typedef ParticleSystem::Shape Shape;

			if (ImGui::CollapsingHeader("System Settings", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Checkbox("Autoplay?", &properties.autoplay);
				ImGui::SameLine();
				ImGui::Checkbox("Loop?", &properties.loop);
				ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3);

				ImGui::DragFloat("Duration", &properties.duration, 0.1f, 0.0f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
				ImGui::DragFloat("Delay", &properties.delay, 0.01f, 0.0f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);

				GUI::Spacing(3);

				ImGui::DragInt("Max Count", (int*)&properties.maxCount, 1, 0, 10'000'000, NULL, ImGuiSliderFlags_AlwaysClamp);

				GUI::Spacing(3);

				ImGui::DragFloat("Gravity", &properties.gravity, 0.01f, -FLT_MAX, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
				ImGui::PopItemWidth();
			}
			if (ImGui::CollapsingHeader("Particle Settings", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3);
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

				ImGui::PushItemWidth(ImGui::CalcItemWidth() / 4.3f);
				ImGui::ColorEdit4("##ColourA", (float*)&properties.ColourA(), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_Float);
				ImGui::SameLine();
				ImGui::DragFloat("##IntensityA", &properties.ColourA().i, 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
				ImGui::SameLine();
				ImGui::ColorEdit4("##ColourB", (float*)&properties.ColourB(), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_Float);
				ImGui::SameLine();
				ImGui::DragFloat("##IntensityB", &properties.ColourB().i, 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
				ImGui::SameLine();
				ImGui::Text("HDR Colour Range");
				ImGui::PopItemWidth();
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
						if (i > 2) ImGui::BeginDisabled();
						if (ImGui::Selectable(shapeNames[i], properties.shape == (Shape)i))
						{
							properties.shape = (Shape)i;
						}
						if (i > 2) ImGui::EndDisabled();
					}

					ImGui::EndCombo();
				}

				ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3);
				switch (properties.shape)
				{
				case Shape::Sphere:
				{
					if (ImGui::DragFloat("##Inner Radius", &properties.innerRadius, 0.01f, 0.0f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
					{
						if (properties.innerRadius > properties.outerRadius) properties.outerRadius = properties.innerRadius;
					}
					ImGui::SameLine();
					if (properties.outerRadius < 0.01f) properties.outerRadius = 1.0f;
					if (ImGui::DragFloat("##Outer Radius", &properties.outerRadius, 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
					{
						if (properties.outerRadius < properties.innerRadius) properties.innerRadius = properties.outerRadius;
					}
					ImGui::SameLine();
					ImGui::Text("Inner/Outer Radius");

					properties.coneAngle = 0.0f;
					break;
				}
				case Shape::Cone:
				{
					if (ImGui::DragFloat("##Inner Radius", &properties.innerRadius, 0.01f, 0.0f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
					{
						if (properties.innerRadius > properties.outerRadius) properties.outerRadius = properties.innerRadius;
					}
					ImGui::SameLine();
					if (properties.outerRadius < 0.01f) properties.outerRadius = 1.0f;
					if (ImGui::DragFloat("##Outer Radius", &properties.outerRadius, 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
					{
						if (properties.outerRadius < properties.innerRadius) properties.innerRadius = properties.outerRadius;
					}
					ImGui::SameLine();
					ImGui::Text("Inner/Outer Radius");

					float coneAngle = glm::degrees(properties.coneAngle);
					if (ImGui::DragFloat("Angle", &coneAngle, 0.1f, 0.0f, 89.9f, NULL, ImGuiSliderFlags_AlwaysClamp))
					{
						properties.coneAngle = glm::radians(coneAngle);
					}
					break;
				}
				case Shape::Box:
				{
					if (properties.width < 0.01f) properties.width = 1.0f;
					ImGui::DragFloat("##Width", &properties.width, 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
					ImGui::SameLine();
					if (properties.height < 0.01f) properties.height = 1.0f;
					ImGui::DragFloat("##Height", &properties.height, 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
					ImGui::SameLine();
					if (properties.depth < 0.01f) properties.depth = 1.0f;
					ImGui::DragFloat("##Depth", &properties.depth, 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
					ImGui::SameLine();
					ImGui::Text("Size");
					break;
				}
				case Shape::Quad:
				{
					if (properties.width < 0.01f) properties.width = 1.0f;
					ImGui::DragFloat("##Width", &properties.width, 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
					ImGui::SameLine();
					if (properties.height < 0.01f) properties.height = 1.0f;
					ImGui::DragFloat("##Height", &properties.height, 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp);
					properties.depth = 0.0f;
					ImGui::SameLine();
					ImGui::Text("Size");
					break;
				}
				case Shape::Circle:
				{
					if (ImGui::DragFloat("##Inner Radius", &properties.innerRadius, 0.01f, 0.0f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
					{
						if (properties.innerRadius > properties.outerRadius) properties.outerRadius = properties.innerRadius;
					}
					ImGui::SameLine();
					if (properties.outerRadius < 0.01f) properties.outerRadius = 1.0f;
					if (ImGui::DragFloat("##Outer Radius", &properties.outerRadius, 0.01f, 0.01f, FLT_MAX, NULL, ImGuiSliderFlags_AlwaysClamp))
					{
						if (properties.outerRadius < properties.innerRadius) properties.innerRadius = properties.outerRadius;
					}
					ImGui::SameLine();
					ImGui::Text("Inner/Outer Radius");

					properties.coneAngle = 0.0f;
					break;
				}
				}

				ImGui::PopItemWidth();
			}

			if (ImGui::CollapsingHeader("Rendering Settings", ImGuiTreeNodeFlags_DefaultOpen))
			{
				string filepath(properties.spriteFilepath, properties.spriteFilepathLength);

				if (ImGui::BeginCombo("Sprite", filepath.c_str()))
				{
					if (ImGui::Selectable("DefaultColour", filepath == "DefaultColour"))
					{
						properties.spriteFilepathLength = 13;
						memcpy(properties.spriteFilepath, "DefaultColour", properties.spriteFilepathLength + 1);
					}
					if (ImGui::Selectable("Select from file"))
					{
						const char* const windowTitle = "Select Sprite";
						const uint defaultPathLength = 17;
						const char defaultPath[defaultPathLength] = "Assets\\Textures\\";
						const uint filterPatternCount = 3;
						const char* const filterPatterns[filterPatternCount] = { "*.png", "*.tga", "*.jpg" };

						const char* newFilepath = tinyfd_openFileDialog(windowTitle, defaultPath, filterPatternCount, filterPatterns, nullptr, false);

						if (newFilepath != nullptr)
						{
							const uint newFilepathLength = (uint)strlen(newFilepath);

							const uint startOffset = (uint)string(newFilepath).find("Assets\\");

							if (startOffset != string::npos)
							{
								properties.spriteFilepathLength = newFilepathLength - startOffset;
								memcpy(properties.spriteFilepath, newFilepath + startOffset, properties.spriteFilepathLength + 1);
							}
							else
							{
								Debug::LogWarning(LogID::WRN105,
									"Assets should only be loaded from inside the Assets folder. "
									"Assets outside this folder won't be included in builds. "
									locationinfo
								);
							}
						}
					}
					ImGui::EndCombo();
				}


				bool alphaClipping = properties.alphaClippingThreshold != 0.0f;
				if (ImGui::Checkbox("Alpha Clipping?", &alphaClipping))
				{
					properties.alphaClippingThreshold = alphaClipping ? 0.5f : 0.0f;
				}
				if (alphaClipping)
				{
					ImGui::SameLine();
					GUI::HSpacing(3);
					ImGui::SameLine();

					ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3);
					ImGui::DragFloat("Threshold", &properties.alphaClippingThreshold, 0.01f, 0.01f, 1.0f, NULL, ImGuiSliderFlags_AlwaysClamp);
					ImGui::PopItemWidth();
				}
			}
		}
		ImGui::Unindent();
	}
}
