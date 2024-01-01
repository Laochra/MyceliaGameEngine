#include "InputEditor.h"

const int keyAmount = 126;
const char* keys[keyAmount] = {
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0 (Keypad)", "1 (Keypad)", "2 (Keypad)", "3 (Keypad)", "4 (Keypad)", "5 (Keypad)", "6 (Keypad)", "7 (Keypad)", "8 (Keypad)", "9 (Keypad)",
    "Space", ",", ".", ";", "- (Hyphen)", "'", "`", "/", "Back Slash", "[", "]",
    "Mouse Left", "Mouse Right", "Mouse Middle", "Mouse 3", "Mouse 4", "Mouse 5", "Mouse 6", "Mouse 7",
    "Shift (Left)", "Shift (Right)", "Control (Left)", "Control (Right)", "Alt (Left)", "Alt (Right)", "Super (Left)", "Super (Right)",
    "Escape", "Tab", "Enter", "Enter (Keypad)", "Backspace", "Delete", "Insert", "Pause", "Print", "Menu",
    "Up", "Down", "Left", "Right", "Page Up", "Page Down", "Home", "End",
    ". (Keypad)", "+ (Keypad)", "- (Keypad)", "* (Keypad)", "/ (Keypad)", "=", "= (Keypad)",
    "Caps Lock", "Num Lock", "Scroll Lock",
     "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15", "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23", "f24", "f25",
};

InputEditor::InputEditor() noexcept
{
    keyCodeToName = new std::map<KeyCode, const char*>();
    {
        // Mouse
        (*keyCodeToName)[MouseLeft] = "Mouse Left";
        (*keyCodeToName)[MouseRight] = "Mouse Right";
        (*keyCodeToName)[MouseMiddle] = "Mouse Middle";
        (*keyCodeToName)[Mouse3] = "Mouse 3";
        (*keyCodeToName)[Mouse4] = "Mouse 4";
        (*keyCodeToName)[Mouse5] = "Mouse 5";
        (*keyCodeToName)[Mouse6] = "Mouse 6";
        (*keyCodeToName)[Mouse7] = "Mouse 7";

        // Special
        (*keyCodeToName)[Esc] = "Escape";
        (*keyCodeToName)[Tab] = "Tab";
        (*keyCodeToName)[Enter] = "Enter";
        (*keyCodeToName)[keypadEnter] = "Enter (Keypad)";
        (*keyCodeToName)[Backspace] = "Backspace";
        (*keyCodeToName)[Delete] = "Delete";
        (*keyCodeToName)[Insert] = "Insert";
        (*keyCodeToName)[Pause] = "Pause";
        (*keyCodeToName)[Print] = "Print";
        (*keyCodeToName)[Menu] = "Menu";

        // Navigation
        (*keyCodeToName)[Up] = "Up";
        (*keyCodeToName)[Down] = "Down";
        (*keyCodeToName)[Left] = "Left";
        (*keyCodeToName)[Right] = "Right";
        (*keyCodeToName)[PageUp] = "Page Up";
        (*keyCodeToName)[PageDown] = "Page Down";
        (*keyCodeToName)[Home] = "Home";
        (*keyCodeToName)[End] = "End";

        // Punctuation
        (*keyCodeToName)[Space] = "Space";
        (*keyCodeToName)[Comma] = ",";
        (*keyCodeToName)[Period] = ".";
        (*keyCodeToName)[Semicolon] = ";";
        (*keyCodeToName)[Hyphen] = "- (Hyphen)";
        (*keyCodeToName)[Apostrophe] = "'";
        (*keyCodeToName)[Grave] = "`";
        (*keyCodeToName)[ForwardSlash] = "/";
        (*keyCodeToName)[BackSlash] = "Back Slash";
        (*keyCodeToName)[LBracket] = "[";
        (*keyCodeToName)[RBracket] = "]";

        // Maths
        (*keyCodeToName)[Decimal] = ". (Keypad)";
        (*keyCodeToName)[Add] = "+ (Keypad)";
        (*keyCodeToName)[Subtract] = "- (Keypad)";
        (*keyCodeToName)[Multiply] = "* (Keypad)";
        (*keyCodeToName)[Divide] = "/ (Keypad)";
        (*keyCodeToName)[Equals] = "=";
        (*keyCodeToName)[keypadEquals] = "= (Keypad)";

        // Locks
        (*keyCodeToName)[CapsLock] = "Caps Lock";
        (*keyCodeToName)[NumLock] = "Num Lock";
        (*keyCodeToName)[ScrollLock] = "Scroll Lock";

        // Modifiers
        (*keyCodeToName)[LShift] = "Shift (Left)";
        (*keyCodeToName)[RShift] = "Shift (Right)";
        (*keyCodeToName)[LControl] = "Control (Left)";
        (*keyCodeToName)[RControl] = "Control (Right)";
        (*keyCodeToName)[LAlt] = "Alt (Left)";
        (*keyCodeToName)[RAlt] = "Alt (Right)";
        (*keyCodeToName)[LSuper] = "Super (Left)";
        (*keyCodeToName)[RSuper] = "Super (Right)";

        // Function
        (*keyCodeToName)[f1] = "f1";
        (*keyCodeToName)[f2] = "f2";
        (*keyCodeToName)[f3] = "f3";
        (*keyCodeToName)[f4] = "f4";
        (*keyCodeToName)[f5] = "f5";
        (*keyCodeToName)[f6] = "f6";
        (*keyCodeToName)[f7] = "f7";
        (*keyCodeToName)[f8] = "f8";
        (*keyCodeToName)[f9] = "f9";
        (*keyCodeToName)[f10] = "f10";
        (*keyCodeToName)[f11] = "f11";
        (*keyCodeToName)[f12] = "f12";
        (*keyCodeToName)[f13] = "f13";
        (*keyCodeToName)[f14] = "f14";
        (*keyCodeToName)[f15] = "f15";
        (*keyCodeToName)[f16] = "f16";
        (*keyCodeToName)[f17] = "f17";
        (*keyCodeToName)[f18] = "f18";
        (*keyCodeToName)[f19] = "f19";
        (*keyCodeToName)[f20] = "f20";
        (*keyCodeToName)[f21] = "f21";
        (*keyCodeToName)[f22] = "f22";
        (*keyCodeToName)[f23] = "f23";
        (*keyCodeToName)[f24] = "f24";
        (*keyCodeToName)[f25] = "f25";

        // Numbers
        (*keyCodeToName)[num0] = "0";
        (*keyCodeToName)[num1] = "1";
        (*keyCodeToName)[num2] = "2";
        (*keyCodeToName)[num3] = "3";
        (*keyCodeToName)[num4] = "4";
        (*keyCodeToName)[num5] = "5";
        (*keyCodeToName)[num6] = "6";
        (*keyCodeToName)[num7] = "7";
        (*keyCodeToName)[num8] = "8";
        (*keyCodeToName)[num9] = "9";
        (*keyCodeToName)[keypad0] = "0 (Keypad)";
        (*keyCodeToName)[keypad1] = "1 (Keypad)";
        (*keyCodeToName)[keypad2] = "2 (Keypad)";
        (*keyCodeToName)[keypad3] = "3 (Keypad)";
        (*keyCodeToName)[keypad4] = "4 (Keypad)";
        (*keyCodeToName)[keypad5] = "5 (Keypad)";
        (*keyCodeToName)[keypad6] = "6 (Keypad)";
        (*keyCodeToName)[keypad7] = "7 (Keypad)";
        (*keyCodeToName)[keypad8] = "8 (Keypad)";
        (*keyCodeToName)[keypad9] = "9 (Keypad)";

        // Letters
        (*keyCodeToName)[A] = "A";
        (*keyCodeToName)[B] = "B";
        (*keyCodeToName)[C] = "C";
        (*keyCodeToName)[D] = "D";
        (*keyCodeToName)[E] = "E";
        (*keyCodeToName)[F] = "F";
        (*keyCodeToName)[G] = "G";
        (*keyCodeToName)[H] = "H";
        (*keyCodeToName)[I] = "I";
        (*keyCodeToName)[J] = "J";
        (*keyCodeToName)[K] = "K";
        (*keyCodeToName)[L] = "L";
        (*keyCodeToName)[M] = "M";
        (*keyCodeToName)[N] = "N";
        (*keyCodeToName)[O] = "O";
        (*keyCodeToName)[P] = "P";
        (*keyCodeToName)[Q] = "Q";
        (*keyCodeToName)[R] = "R";
        (*keyCodeToName)[S] = "S";
        (*keyCodeToName)[T] = "T";
        (*keyCodeToName)[U] = "U";
        (*keyCodeToName)[V] = "V";
        (*keyCodeToName)[W] = "W";
        (*keyCodeToName)[X] = "X";
        (*keyCodeToName)[Y] = "Y";
        (*keyCodeToName)[Z] = "Z";
    }
    keyNameToCode = new std::map<const char*, KeyCode>();
    {
        // Mouse
        (*keyNameToCode)["Mouse Left"] = MouseLeft;
        (*keyNameToCode)["Mouse Right"] = MouseRight;
        (*keyNameToCode)["Mouse Middle"] = MouseMiddle;
        (*keyNameToCode)["Mouse 3"] = Mouse3;
        (*keyNameToCode)["Mouse 4"] = Mouse4;
        (*keyNameToCode)["Mouse 5"] = Mouse5;
        (*keyNameToCode)["Mouse 6"] = Mouse6;
        (*keyNameToCode)["Mouse 7"] = Mouse7;

        // Special
        (*keyNameToCode)["Escape"] = Esc;
        (*keyNameToCode)["Tab"] = Tab;
        (*keyNameToCode)["Enter"] = Enter;
        (*keyNameToCode)["Enter (Keypad)"] = keypadEnter;
        (*keyNameToCode)["Backspace"] = Backspace;
        (*keyNameToCode)["Delete"] = Delete;
        (*keyNameToCode)["Insert"] = Insert;
        (*keyNameToCode)["Pause"] = Pause;
        (*keyNameToCode)["Print"] = Print;
        (*keyNameToCode)["Menu"] = Menu;

        // Navigation
        (*keyNameToCode)["Up"] = Up;
        (*keyNameToCode)["Down"] = Down;
        (*keyNameToCode)["Left"] = Left;
        (*keyNameToCode)["Right"] = Right;
        (*keyNameToCode)["Page Up"] = PageUp;
        (*keyNameToCode)["Page Down"] = PageDown;
        (*keyNameToCode)["Home"] = Home;
        (*keyNameToCode)["End"] = End;

        // Punctuation
        (*keyNameToCode)["Space"] = Space;
        (*keyNameToCode)[","] = Comma;
        (*keyNameToCode)["."] = Period;
        (*keyNameToCode)[";"] = Semicolon;
        (*keyNameToCode)["- (Hyphen)"] = Hyphen;
        (*keyNameToCode)["'"] = Apostrophe;
        (*keyNameToCode)["`"] = Grave;
        (*keyNameToCode)["/"] = ForwardSlash;
        (*keyNameToCode)["Back Slash"] = BackSlash;
        (*keyNameToCode)["["] = LBracket;
        (*keyNameToCode)["]"] = RBracket;

        // Maths
        (*keyNameToCode)[". (Keypad)"] = Decimal;
        (*keyNameToCode)["+ (Keypad)"] = Add;
        (*keyNameToCode)["- (Keypad)"] = Subtract;
        (*keyNameToCode)["* (Keypad)"] = Multiply;
        (*keyNameToCode)["/ (Keypad)"] = Divide;
        (*keyNameToCode)["="] = Equals;
        (*keyNameToCode)["= (Keypad)"] = keypadEquals;

        // Locks
        (*keyNameToCode)["Caps Lock"] = CapsLock;
        (*keyNameToCode)["Num Lock"] = NumLock;
        (*keyNameToCode)["Scroll Lock"] = ScrollLock;

        // Modifiers
        (*keyNameToCode)["Shift (Left)"] = LShift;
        (*keyNameToCode)["Shift (Right)"] = RShift;
        (*keyNameToCode)["Control (Left)"] = LControl;
        (*keyNameToCode)["Control (Right)"] = RControl;
        (*keyNameToCode)["Alt (Left)"] = LAlt;
        (*keyNameToCode)["Alt (Right)"] = RAlt;
        (*keyNameToCode)["Super (Left)"] = LSuper;
        (*keyNameToCode)["Super (Right)"] = RSuper;

        // Function
        (*keyNameToCode)["f1"] = f1;
        (*keyNameToCode)["f2"] = f2;
        (*keyNameToCode)["f3"] = f3;
        (*keyNameToCode)["f4"] = f4;
        (*keyNameToCode)["f5"] = f5;
        (*keyNameToCode)["f6"] = f6;
        (*keyNameToCode)["f7"] = f7;
        (*keyNameToCode)["f8"] = f8;
        (*keyNameToCode)["f9"] = f9;
        (*keyNameToCode)["f10"] = f10;
        (*keyNameToCode)["f11"] = f11;
        (*keyNameToCode)["f12"] = f12;
        (*keyNameToCode)["f13"] = f13;
        (*keyNameToCode)["f14"] = f14;
        (*keyNameToCode)["f15"] = f15;
        (*keyNameToCode)["f16"] = f16;
        (*keyNameToCode)["f17"] = f17;
        (*keyNameToCode)["f18"] = f18;
        (*keyNameToCode)["f19"] = f19;
        (*keyNameToCode)["f20"] = f20;
        (*keyNameToCode)["f21"] = f21;
        (*keyNameToCode)["f22"] = f22;
        (*keyNameToCode)["f23"] = f23;
        (*keyNameToCode)["f24"] = f24;
        (*keyNameToCode)["f25"] = f25;

        // Numbers
        (*keyNameToCode)["0"] = num0;
        (*keyNameToCode)["1"] = num1;
        (*keyNameToCode)["2"] = num2;
        (*keyNameToCode)["3"] = num3;
        (*keyNameToCode)["4"] = num4;
        (*keyNameToCode)["5"] = num5;
        (*keyNameToCode)["6"] = num6;
        (*keyNameToCode)["7"] = num7;
        (*keyNameToCode)["8"] = num8;
        (*keyNameToCode)["9"] = num9;
        (*keyNameToCode)["0 (Keypad)"] = keypad0;
        (*keyNameToCode)["1 (Keypad)"] = keypad1;
        (*keyNameToCode)["2 (Keypad)"] = keypad2;
        (*keyNameToCode)["3 (Keypad)"] = keypad3;
        (*keyNameToCode)["4 (Keypad)"] = keypad4;
        (*keyNameToCode)["5 (Keypad)"] = keypad5;
        (*keyNameToCode)["6 (Keypad)"] = keypad6;
        (*keyNameToCode)["7 (Keypad)"] = keypad7;
        (*keyNameToCode)["8 (Keypad)"] = keypad8;
        (*keyNameToCode)["9 (Keypad)"] = keypad9;

        // Letters
        (*keyNameToCode)["A"] = A;
        (*keyNameToCode)["B"] = B;
        (*keyNameToCode)["C"] = C;
        (*keyNameToCode)["D"] = D;
        (*keyNameToCode)["E"] = E;
        (*keyNameToCode)["F"] = F;
        (*keyNameToCode)["G"] = G;
        (*keyNameToCode)["H"] = H;
        (*keyNameToCode)["I"] = I;
        (*keyNameToCode)["J"] = J;
        (*keyNameToCode)["K"] = K;
        (*keyNameToCode)["L"] = L;
        (*keyNameToCode)["M"] = M;
        (*keyNameToCode)["N"] = N;
        (*keyNameToCode)["O"] = O;
        (*keyNameToCode)["P"] = P;
        (*keyNameToCode)["Q"] = Q;
        (*keyNameToCode)["R"] = R;
        (*keyNameToCode)["S"] = S;
        (*keyNameToCode)["T"] = T;
        (*keyNameToCode)["U"] = U;
        (*keyNameToCode)["V"] = V;
        (*keyNameToCode)["W"] = W;
        (*keyNameToCode)["X"] = X;
        (*keyNameToCode)["Y"] = Y;
        (*keyNameToCode)["Z"] = Z;
    }
}

void InputEditor::ConfigureKeybind(const char* label, Keybind* keybind)
{
    const char* currentKey = keyCodeToName->find(keybind->GetBinds()[0])->second;

    if (ImGui::BeginCombo(label, currentKey))
    {
        for (int i = 0; i < keyAmount; i++)
        {
            bool isCurrent = keys[i] == currentKey;
            if (ImGui::Selectable(keys[i], isCurrent))
            {
                currentKey = keys[i];
                keybind->ClearBinds();
                keybind->Bind(keyNameToCode->find(keys[i])->second);
            }

            if (isCurrent) ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }
}

void InputEditor::ConfigureKeyAxis(const char* negativeLabel, const char* positiveLabel, KeyAxis* keyAxis)
{
    const char* currentNegativeKey = keyCodeToName->find(keyAxis->GetNegativeBinds()[0])->second;
    const char* currentPositiveKey = keyCodeToName->find(keyAxis->GetPositiveBinds()[0])->second;

    if (ImGui::BeginCombo(negativeLabel, currentNegativeKey))
    {
        for (int i = 0; i < keyAmount; i++)
        {
            bool isCurrent = keys[i] == currentNegativeKey;
            if (ImGui::Selectable(keys[i], isCurrent))
            {
                currentNegativeKey = keys[i];
                keyAxis->ClearNegativeBinds();
                keyAxis->BindNegative(keyNameToCode->find(keys[i])->second);
            }

            if (isCurrent) ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }
    if (ImGui::BeginCombo(positiveLabel, currentPositiveKey))
    {
        for (int i = 0; i < keyAmount; i++)
        {
            bool isCurrent = keys[i] == currentPositiveKey;
            if (ImGui::Selectable(keys[i], isCurrent))
            {
                currentPositiveKey = keys[i];
                keyAxis->ClearPositiveBinds();
                keyAxis->BindPositive(keyNameToCode->find(keys[i])->second);
            }

            if (isCurrent) ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }
}