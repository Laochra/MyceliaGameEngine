#include "InputEditor.h"

namespace InputEditor
{
	const char* binds[] = {
		 "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
		 "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0 (Keypad)", "1 (Keypad)", "2 (Keypad)", "3 (Keypad)", "4 (Keypad)", "5 (Keypad)", "6 (Keypad)", "7 (Keypad)", "8 (Keypad)", "9 (Keypad)",
		 "Space", ",", ".", ";", "-", "'", "`", "/", "\\", "[", "]",
		 "Mouse Left", "Mouse Right", "Mouse Middle", "Mouse 3", "Mouse 4", "Mouse 5", "Mouse 6", "Mouse 7",
		 "Shift (Left)", "Shift (Right)", "Control (Left)", "Control (Right)", "Alt (Left)", "Alt (Right)", "Super (Left)", "Super (Right)",
		 "Esc", "Tab", "Enter", "Enter (Keypad)", "Backspace", "Delete", "Insert", "Pause", "Print", "Menu",
		 "Up", "Down", "Left", "Right", "Page Up", "Page Down", "Home", "End",
		 ". (Keypad)", "+ (Keypad)", "- (Keypad)", "* (Keypad)", "/ (Keypad)", "=", "= (Keypad)",
		 "Caps Lock", "Num Lock", "Scroll Lock",
		  "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15", "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23", "f24", "f25",
	};

	void ConfigureInputBind(const char* label, InputBind* inputBind)
	{
		const char* currentBind = Input::GetNameFromCode(inputBind->GetBinds()[0]);

		if (ImGui::BeginCombo(label, currentBind, ImGuiComboFlags_HeightLarge))
		{
			for (int i = 0; i < sizeof(binds) / sizeof(binds[0]); i++)
			{
				bool isCurrent = binds[i] == currentBind;
				if (ImGui::Selectable(binds[i], isCurrent))
				{
					currentBind = binds[i];
					inputBind->ClearBinds();
					inputBind->Bind(Input::GetCodeFromName(binds[i]));
				}

				if (isCurrent) ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
	}
	void ConfigureInputAxis(const char* negativeLabel, const char* positiveLabel, InputAxis* inputAxis)
	{
		const char* currentNegativeBind = Input::GetNameFromCode(inputAxis->GetNegativeBinds()[0]);
		const char* currentPositiveBind = Input::GetNameFromCode(inputAxis->GetPositiveBinds()[0]);

		if (ImGui::BeginCombo(negativeLabel, currentNegativeBind, ImGuiComboFlags_HeightLarge))
		{
			for (int i = 0; i < sizeof(binds) / sizeof(binds[0]); i++)
			{
				bool isCurrent = binds[i] == currentNegativeBind;
				if (ImGui::Selectable(binds[i], isCurrent))
				{
					currentNegativeBind = binds[i];
					inputAxis->ClearNegativeBinds();
					inputAxis->BindNegative(Input::GetCodeFromName(binds[i]));
				}

				if (isCurrent) ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
		if (ImGui::BeginCombo(positiveLabel, currentPositiveBind, ImGuiComboFlags_HeightLarge))
		{
			for (int i = 0; i < sizeof(binds) / sizeof(binds[0]); i++)
			{
				bool isCurrent = binds[i] == currentPositiveBind;
				if (ImGui::Selectable(binds[i], isCurrent))
				{
					currentPositiveBind = binds[i];
					inputAxis->ClearPositiveBinds();
					inputAxis->BindPositive(Input::GetCodeFromName(binds[i]));
				}

				if (isCurrent) ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
	}
}