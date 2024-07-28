#include "PostProcessingGUI.h"

#include "PostProcessing.h"

namespace PostProcessingGUI
{
	void PostProcessingGUI::Draw(const char* const name, bool& open) noexcept
	{
		ImGui::Begin(name, &open);

		ImGui::DragFloat("HDR Exposure", &PostProcess::Defaults::exposure, 0.1f, 0.1f, 1000.0f);

		ImGui::End();
	}
}
