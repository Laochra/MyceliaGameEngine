#include "InputEditor.h"

namespace InputEditor
{
    const int keyAmount = 126;
    const char* keys[keyAmount] = {
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

    const std::map<KeyCode, const char*> keyCodeToName = {
        // Mouse
        { MouseLeft, "Mouse Left" },
        { MouseRight, "Mouse Right" },
        { MouseMiddle, "Mouse Middle" },
        { Mouse3, "Mouse 3" },
        { Mouse4, "Mouse 4" },
        { Mouse5, "Mouse 5" },
        { Mouse6, "Mouse 6" },
        { Mouse7, "Mouse 7" },

        // Special
        { Esc, "Esc" },
        { Tab, "Tab" },
        { Enter, "Enter" },
        { keypadEnter, "Enter (Keypad)" },
        { Backspace, "Backspace" },
        { Delete, "Delete" },
        { Insert, "Insert" },
        { Pause, "Pause" },
        { Print, "Print" },
        { Menu, "Menu" },

        // Navigation
        { Up, "Up" },
        { Down, "Down" },
        { Left, "Left" },
        { Right, "Right" },
        { PageUp, "Page Up" },
        { PageDown, "Page Down" },
        { Home, "Home" },
        { End, "End" },

        // Punctuation
        { Space, "Space" },
        { Comma, "," },
        { Period, "." },
        { Semicolon, ";" },
        { Hyphen, "-" },
        { Apostrophe, "'" },
        { Grave, "`" },
        { ForwardSlash, "/" },
        { BackSlash, "\\" },
        { LBracket, "[" },
        { RBracket, "]" },

        // Maths
        { Decimal, ". (Keypad)" },
        { Add, "+ (Keypad)" },
        { Subtract, "- (Keypad)" },
        { Multiply, "* (Keypad)" },
        { Divide, "/ (Keypad)" },
        { Equals, "=" },
        { keypadEquals, "= (Keypad)" },

        // Locks
        { CapsLock, "Caps Lock" },
        { NumLock, "Num Lock" },
        { ScrollLock, "Scroll Lock" },

        // Modifiers
        { LShift, "Shift (Left)" },
        { RShift, "Shift (Right)" },
        { LControl, "Control (Left)" },
        { RControl, "Control (Right)" },
        { LAlt, "Alt (Left)" },
        { RAlt, "Alt (Right)" },
        { LSuper, "Super (Left)" },
        { RSuper, "Super (Right)" },

        // Function
        { f1, "f1" },
        { f2, "f2" },
        { f3, "f3" },
        { f4, "f4" },
        { f5, "f5" },
        { f6, "f6" },
        { f7, "f7" },
        { f8, "f8" },
        { f9, "f9" },
        { f10, "f10" },
        { f11, "f11" },
        { f12, "f12" },
        { f13, "f13" },
        { f14, "f14" },
        { f15, "f15" },
        { f16, "f16" },
        { f17, "f17" },
        { f18, "f18" },
        { f19, "f19" },
        { f20, "f20" },
        { f21, "f21" },
        { f22, "f22" },
        { f23, "f23" },
        { f24, "f24" },
        { f25, "f25" },

        // Numbers
        { num0, "0" },
        { num1, "1" },
        { num2, "2" },
        { num3, "3" },
        { num4, "4" },
        { num5, "5" },
        { num6, "6" },
        { num7, "7" },
        { num8, "8" },
        { num9, "9" },
        { keypad0, "0 (Keypad)" },
        { keypad1, "1 (Keypad)" },
        { keypad2, "2 (Keypad)" },
        { keypad3, "3 (Keypad)" },
        { keypad4, "4 (Keypad)" },
        { keypad5, "5 (Keypad)" },
        { keypad6, "6 (Keypad)" },
        { keypad7, "7 (Keypad)" },
        { keypad8, "8 (Keypad)" },
        { keypad9, "9 (Keypad)" },

        // Letters
        { A, "A" },
        { B, "B" },
        { C, "C" },
        { D, "D" },
        { E, "E" },
        { F, "F" },
        { G, "G" },
        { H, "H" },
        { I, "I" },
        { J, "J" },
        { K, "K" },
        { L, "L" },
        { M, "M" },
        { N, "N" },
        { O, "O" },
        { P, "P" },
        { Q, "Q" },
        { R, "R" },
        { S, "S" },
        { T, "T" },
        { U, "U" },
        { V, "V" },
        { W, "W" },
        { X, "X" },
        { Y, "Y" },
        { Z, "Z" }
    };
    const std::map<const char*, KeyCode> keyNameToCode = {
        // Mouse
        { "Mouse Left", MouseLeft },
        { "Mouse Right", MouseRight },
        { "Mouse Middle", MouseMiddle },
        { "Mouse 3", Mouse3 },
        { "Mouse 4", Mouse4 },
        { "Mouse 5", Mouse5 },
        { "Mouse 6", Mouse6 },
        { "Mouse 7", Mouse7 },

        // Special
        { "Esc", Esc },
        { "Tab", Tab },
        { "Enter", Enter },
        { "Enter (Keypad)", keypadEnter },
        { "Backspace", Backspace },
        { "Delete", Delete },
        { "Insert", Insert },
        { "Pause", Pause },
        { "Print", Print },
        { "Menu", Menu },

        // Navigation
        { "Up", Up },
        { "Down", Down },
        { "Left", Left },
        { "Right", Right },
        { "Page Up", PageUp },
        { "Page Down", PageDown },
        { "Home", Home },
        { "End", End },

        // Punctuation
        { "Space", Space },
        { ",", Comma },
        { ".", Period },
        { ";", Semicolon },
        { "-", Hyphen },
        { "'", Apostrophe },
        { "`", Grave },
        { "/", ForwardSlash },
        { "\\", BackSlash },
        { "[", LBracket },
        { "]", RBracket },

        // Maths
        { ". (Keypad)", Decimal },
        { "+ (Keypad)", Add },
        { "- (Keypad)", Subtract },
        { "* (Keypad)", Multiply },
        { "/ (Keypad)", Divide },
        { "=", Equals },
        { "= (Keypad)", keypadEquals },

        // Locks
        { "Caps Lock", CapsLock },
        { "Num Lock", NumLock },
        { "Scroll Lock", ScrollLock },

        // Modifiers
        { "Shift (Left)", LShift },
        { "Shift (Right)", RShift },
        { "Control (Left)", LControl },
        { "Control (Right)", RControl },
        { "Alt (Left)", LAlt },
        { "Alt (Right)", RAlt },
        { "Super (Left)", LSuper },
        { "Super (Right)", RSuper },

        // Function
        { "f1", f1 },
        { "f2", f2 },
        { "f3", f3 },
        { "f4", f4 },
        { "f5", f5 },
        { "f6", f6 },
        { "f7", f7 },
        { "f8", f8 },
        { "f9", f9 },
        { "f10", f10 },
        { "f11", f11 },
        { "f12", f12 },
        { "f13", f13 },
        { "f14", f14 },
        { "f15", f15 },
        { "f16", f16 },
        { "f17", f17 },
        { "f18", f18 },
        { "f19", f19 },
        { "f20", f20 },
        { "f21", f21 },
        { "f22", f22 },
        { "f23", f23 },
        { "f24", f24 },
        { "f25", f25 },

        // Numbers
        { "0", num0 },
        { "1", num1 },
        { "2", num2 },
        { "3", num3 },
        { "4", num4 },
        { "5", num5 },
        { "6", num6 },
        { "7", num7 },
        { "8", num8 },
        { "9", num9 },
        { "0 (Keypad)", keypad0 },
        { "1 (Keypad)", keypad1 },
        { "2 (Keypad)", keypad2 },
        { "3 (Keypad)", keypad3 },
        { "4 (Keypad)", keypad4 },
        { "5 (Keypad)", keypad5 },
        { "6 (Keypad)", keypad6 },
        { "7 (Keypad)", keypad7 },
        { "8 (Keypad)", keypad8 },
        { "9 (Keypad)", keypad9 },

        // Letters
        { "A", A },
        { "B", B },
        { "C", C },
        { "D", D },
        { "E", E },
        { "F", F },
        { "G", G },
        { "H", H },
        { "I", I },
        { "J", J },
        { "K", K },
        { "L", L },
        { "M", M },
        { "N", N },
        { "O", O },
        { "P", P },
        { "Q", Q },
        { "R", R },
        { "S", S },
        { "T", T },
        { "U", U },
        { "V", V },
        { "W", W },
        { "X", X },
        { "Y", Y },
        { "Z", Z }
    };

    void ConfigureKeybind(const char* label, Keybind* keybind)
    {
        const char* currentKey = keyCodeToName.find(keybind->GetBinds()[0])->second;

        if (ImGui::BeginCombo(label, currentKey, ImGuiComboFlags_HeightLarge))
        {
            for (int i = 0; i < keyAmount; i++)
            {
                bool isCurrent = keys[i] == currentKey;
                if (ImGui::Selectable(keys[i], isCurrent))
                {
                    currentKey = keys[i];
                    keybind->ClearBinds();
                    keybind->Bind(keyNameToCode.find(keys[i])->second);
                }

                if (isCurrent) ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }
    }
    void ConfigureKeyAxis(const char* negativeLabel, const char* positiveLabel, KeyAxis* keyAxis)
    {
        const char* currentNegativeKey = keyCodeToName.find(keyAxis->GetNegativeBinds()[0])->second;
        const char* currentPositiveKey = keyCodeToName.find(keyAxis->GetPositiveBinds()[0])->second;

        if (ImGui::BeginCombo(negativeLabel, currentNegativeKey, ImGuiComboFlags_HeightLarge))
        {
            for (int i = 0; i < keyAmount; i++)
            {
                bool isCurrent = keys[i] == currentNegativeKey;
                if (ImGui::Selectable(keys[i], isCurrent))
                {
                    currentNegativeKey = keys[i];
                    keyAxis->ClearNegativeBinds();
                    keyAxis->BindNegative(keyNameToCode.find(keys[i])->second);
                }

                if (isCurrent) ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }
        if (ImGui::BeginCombo(positiveLabel, currentPositiveKey, ImGuiComboFlags_HeightLarge))
        {
            for (int i = 0; i < keyAmount; i++)
            {
                bool isCurrent = keys[i] == currentPositiveKey;
                if (ImGui::Selectable(keys[i], isCurrent))
                {
                    currentPositiveKey = keys[i];
                    keyAxis->ClearPositiveBinds();
                    keyAxis->BindPositive(keyNameToCode.find(keys[i])->second);
                }

                if (isCurrent) ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }
    }
}