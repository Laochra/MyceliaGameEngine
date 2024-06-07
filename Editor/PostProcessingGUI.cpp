#include "PostProcessingGUI.h"

namespace PostProcessingGUI
{
	float exposure = 3.0f;

	void PostProcessingGUI::Draw(const char* const name, bool& open) noexcept
	{
		ImGui::Begin(name, &open);

		ImGui::DragFloat("HDR Exposure", &exposure, 0.1f, 0.1f, 1000.0f);

		ImGui::End();
	}
}
