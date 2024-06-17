#include "GUI.h"

#include "GLIncludes.h"
#include <JsonIncludes.h>

#include "Inspector.h"
#include "ShaderProgramGUI.h"
#include "MaterialGUI.h"

#include "GameObject.h"

#include <sstream>

#include "Debug.h"

string GUI::currentStyle = "Orchid";
map<string, string> GUI::styleMap = {
	{ "Forest", "Engine\\EditorStyles\\Forest.style" },
	{ "Igneous", "Engine\\EditorStyles\\Igneous.style" },
	{ "Marshmellow", "Engine\\EditorStyles\\Marshmellow.style" },
	{ "Obsidian", "Engine\\EditorStyles\\Obsidian.style" },
	{ "Orchid", "Engine\\EditorStyles\\Orchid.style" },
	{ "Seagrass", "Engine\\EditorStyles\\Seagrass.style" },
	{ "Kidcore", "Engine\\EditorStyles\\Kidcore.style" },
};

GUI::EditorColours GUI::colours;


std::string GUI::GenerateID(GameObject* gameObject)
{
	string str = gameObject->GetName();
	str += "##" + std::to_string(gameObject->GetGUID());

	return str;
}
std::string GUI::GenerateID(const char* name, GameObject* gameObject)
{
	string str = name;
	str += "##" + std::to_string(gameObject->GetGUID());

	return str;
}
std::string GUI::InvisibleName(const char* uniqueName)
{
	string str = "##";
	str += uniqueName;

	return str;
}

void GUI::Spacing(uint numberOfSpacings)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) return;

	for (uint i = 0; i < numberOfSpacings; i++)
	{
		ImGui::ItemSize(ImVec2(0, 0));
	}
}
void GUI::HSpacing(uint numberOfSpacings)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) return;

	for (uint i = 0; i < numberOfSpacings; i++)
	{
		ImGui::ItemSize(ImVec2(0, 0));
		if (i + 1 < numberOfSpacings)
		{
			ImGui::SameLine();
		}
	}
}

void GUI::SaveStyle()
{

#define SerialiseColour(style, colour, group, name, var) vector<float> var = { colour.r, colour.g, colour.b, colour.a }; style[group][name] = var
	
	json style;

	SerialiseColour(style, colours.text, "Text", "Regular", a1);
	SerialiseColour(style, colours.textDisabled, "Text", "Disabled", a2);
	SerialiseColour(style, colours.textSelection, "Text", "Selected", a3);

	SerialiseColour(style, colours.background, "Backgrounds", "Main", b1);
	SerialiseColour(style, colours.backgroundActive, "Backgrounds", "Active", b2);
	SerialiseColour(style, colours.accentBackground, "Backgrounds", "Accent", b3);

	SerialiseColour(style, colours.inputBoxes, "Input Fields", "Regular", c1);
	SerialiseColour(style, colours.inputBoxesHover, "Input Fields", "Hovered", c2);
	SerialiseColour(style, colours.inputBoxesClick, "Input Fields", "Clicked", c3);
	SerialiseColour(style, colours.toggledInput, "Input Fields", "Toggled", c4);

	SerialiseColour(style, colours.interactable, "Buttons & Resize", "Regular", d1);
	SerialiseColour(style, colours.interactableHover, "Buttons & Resize", "Hovered", d2);
	SerialiseColour(style, colours.interactableClick, "Buttons & Resize", "Clicked", d3);

	SerialiseColour(style, colours.header, "Foldable Headers", "Regular", e1);
	SerialiseColour(style, colours.headerHover, "Foldable Headers", "Hovered", e2);
	SerialiseColour(style, colours.headerClick, "Foldable Headers", "Clicked", e3);

	SerialiseColour(style, colours.tab, "Tabs", "Regular", f1);
	SerialiseColour(style, colours.tabHover, "Tabs", "Hovered", f2);
	SerialiseColour(style, colours.tabActive, "Tabs", "Active", f3);
	SerialiseColour(style, colours.tabUnfocused, "Tabs", "Unfocused", f4);
	SerialiseColour(style, colours.tabUnfocusedActive, "Tabs", "Unfocused & Active", f5);

	SerialiseColour(style, colours.scrollHandle, "Scrollbar Handles", "Regular", g1);
	SerialiseColour(style, colours.scrollHandleHover, "Scrollbar Handles", "Hovered", g2);
	SerialiseColour(style, colours.scrollHandleClick, "Scrollbar Handles", "Clicked", g3);

	SerialiseColour(style, colours.seperatorBar, "Borders & Bars", "Seperator Bar", h1);
	SerialiseColour(style, colours.windowBorder, "Borders & Bars", "Window Border", h2);

	SerialiseColour(style, colours.tableBorderMain, "Tables", "Main Border", i1);
	SerialiseColour(style, colours.tableBorderInner, "Tables", "Inner Border", i2);
	SerialiseColour(style, colours.tableRowFill, "Tables", "Row Fill", i3);
	SerialiseColour(style, colours.tableAltRowFill, "Tables", "Alt Row Fill", i4);

	SerialiseColour(style, colours.docking, "Docking", "Preview", j1);
	SerialiseColour(style, colours.emptyDockingFill, "Docking", "Empty Fill", j2);


#undef SerialiseColour

	ofstream output("UserSettings\\EditorStyle.style");
	output << std::setw(2) << style << '\n';
}

void GUI::LoadStyle(string styleName)
{
	currentStyle = styleName;

	string stylePath;
	if (styleName == "Custom") stylePath = "UserSettings\\EditorStyle.style";
	else stylePath = styleMap[styleName];

	ifstream input(stylePath.c_str());

	if (!input.good())
	{
		Debug::LogWarning(LogID::WRN101, styleName, " style could not be found at filepath ", stylePath, ". Reverted to defaults.", locationinfo);
		return;
	}

	json style;

	try { input >> style; }
	catch (parse_error)
	{
		Debug::LogWarning(LogID::WRN102, styleName, " (", stylePath, ")", locationinfo);
		return;
	}

#define DeserialiseColour(style, colour, group, name, var) vector<float> var = style[group][name]; memcpy(&colour, var.data(), 4 * sizeof(float))

	DeserialiseColour(style, colours.text, "Text", "Regular", a1);
	DeserialiseColour(style, colours.textDisabled, "Text", "Disabled", a2);
	DeserialiseColour(style, colours.textSelection, "Text", "Selected", a3);
	
	DeserialiseColour(style, colours.background, "Backgrounds", "Main", b1);
	DeserialiseColour(style, colours.backgroundActive, "Backgrounds", "Active", b2);
	DeserialiseColour(style, colours.accentBackground, "Backgrounds", "Accent", b3);
	
	DeserialiseColour(style, colours.inputBoxes, "Input Fields", "Regular", c1);
	DeserialiseColour(style, colours.inputBoxesHover, "Input Fields", "Hovered", c2);
	DeserialiseColour(style, colours.inputBoxesClick, "Input Fields", "Clicked", c3);
	DeserialiseColour(style, colours.toggledInput, "Input Fields", "Toggled", c4);
	
	DeserialiseColour(style, colours.interactable, "Buttons & Resize", "Regular", d1);
	DeserialiseColour(style, colours.interactableHover, "Buttons & Resize", "Hovered", d2);
	DeserialiseColour(style, colours.interactableClick, "Buttons & Resize", "Clicked", d3);
	
	DeserialiseColour(style, colours.header, "Foldable Headers", "Regular", e1);
	DeserialiseColour(style, colours.headerHover, "Foldable Headers", "Hovered", e2);
	DeserialiseColour(style, colours.headerClick, "Foldable Headers", "Clicked", e3);
	
	DeserialiseColour(style, colours.tab, "Tabs", "Regular", f1);
	DeserialiseColour(style, colours.tabHover, "Tabs", "Hovered", f2);
	DeserialiseColour(style, colours.tabActive, "Tabs", "Active", f3);
	DeserialiseColour(style, colours.tabUnfocused, "Tabs", "Unfocused", f4);
	DeserialiseColour(style, colours.tabUnfocusedActive, "Tabs", "Unfocused & Active", f5);
	
	DeserialiseColour(style, colours.scrollHandle, "Scrollbar Handles", "Regular", g1);
	DeserialiseColour(style, colours.scrollHandleHover, "Scrollbar Handles", "Hovered", g2);
	DeserialiseColour(style, colours.scrollHandleClick, "Scrollbar Handles", "Clicked", g3);
	
	DeserialiseColour(style, colours.seperatorBar, "Borders & Bars", "Seperator Bar", h1);
	DeserialiseColour(style, colours.windowBorder, "Borders & Bars", "Window Border", h2);
	
	DeserialiseColour(style, colours.tableBorderMain, "Tables", "Main Border", i1);
	DeserialiseColour(style, colours.tableBorderInner, "Tables", "Inner Border", i2);
	DeserialiseColour(style, colours.tableRowFill, "Tables", "Row Fill", i3);
	DeserialiseColour(style, colours.tableAltRowFill, "Tables", "Alt Row Fill", i4);
	
	DeserialiseColour(style, colours.docking, "Docking", "Preview", j1);
	DeserialiseColour(style, colours.emptyDockingFill, "Docking", "Empty Fill", j2);

#undef DeserialiseColour

	UpdateStyle();
}

void GUI::UpdateStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	
	// Font
	ImFont* odudoMono = ImGui::GetIO().Fonts->AddFontFromFileTTF("Assets\\Fonts\\OdudoMono-Regular.otf", 16);

	// Text
	style->Colors[ImGuiCol_Text]						= *(ImVec4*)&colours.text;
	style->Colors[ImGuiCol_TextDisabled]			= *(ImVec4*)&colours.textDisabled;
	style->Colors[ImGuiCol_TextSelectedBg]			= *(ImVec4*)&colours.textSelection;

	// Backgrounds
	style->Colors[ImGuiCol_WindowBg]					= *(ImVec4*)&colours.background;
	style->Colors[ImGuiCol_ChildBg]					= *(ImVec4*)&colours.background;
	style->Colors[ImGuiCol_PopupBg]					= *(ImVec4*)&colours.background;
	style->Colors[ImGuiCol_TitleBg]					= *(ImVec4*)&colours.background;
	style->Colors[ImGuiCol_TitleBgCollapsed]		= *(ImVec4*)&colours.background;
	style->Colors[ImGuiCol_ResizeGrip]				= *(ImVec4*)&colours.background;
	style->Colors[ImGuiCol_TitleBgActive]			= *(ImVec4*)&colours.backgroundActive;
	style->Colors[ImGuiCol_MenuBarBg]				= *(ImVec4*)&colours.accentBackground;
	style->Colors[ImGuiCol_ScrollbarBg]				= *(ImVec4*)&colours.accentBackground;

	// Input Fields
	style->Colors[ImGuiCol_FrameBg]					= *(ImVec4*)&colours.inputBoxes;
	style->Colors[ImGuiCol_FrameBgHovered]			= *(ImVec4*)&colours.inputBoxesHover;
	style->Colors[ImGuiCol_FrameBgActive]			= *(ImVec4*)&colours.inputBoxesClick;
	style->Colors[ImGuiCol_CheckMark]				= *(ImVec4*)&colours.toggledInput;
	style->Colors[ImGuiCol_DragDropTarget]			= *(ImVec4*)&colours.toggledInput;

	// Buttons & Resize
	style->Colors[ImGuiCol_Button]					= *(ImVec4*)&colours.interactable;
	style->Colors[ImGuiCol_ButtonHovered]			= *(ImVec4*)&colours.interactableHover;
	style->Colors[ImGuiCol_ResizeGripHovered]		= *(ImVec4*)&colours.interactableHover;
	style->Colors[ImGuiCol_SeparatorHovered]		= *(ImVec4*)&colours.interactableHover;
	style->Colors[ImGuiCol_ButtonActive]			= *(ImVec4*)&colours.interactableClick;
	style->Colors[ImGuiCol_ResizeGripActive]		= *(ImVec4*)&colours.interactableClick;

	// Foldable Headers
	style->Colors[ImGuiCol_Header]					= *(ImVec4*)&colours.header;
	style->Colors[ImGuiCol_TableHeaderBg]			= *(ImVec4*)&colours.header;
	style->Colors[ImGuiCol_HeaderHovered]			= *(ImVec4*)&colours.headerHover;
	style->Colors[ImGuiCol_HeaderActive]			= *(ImVec4*)&colours.headerClick;

	// Tabs
	style->Colors[ImGuiCol_Tab]						= *(ImVec4*)&colours.tab;
	style->Colors[ImGuiCol_TabHovered]				= *(ImVec4*)&colours.tabHover;
	style->Colors[ImGuiCol_TabActive]				= *(ImVec4*)&colours.tabActive;
	style->Colors[ImGuiCol_TabUnfocused]			= *(ImVec4*)&colours.tabUnfocused;
	style->Colors[ImGuiCol_TabUnfocusedActive]	= *(ImVec4*)&colours.tabUnfocusedActive;

	// Scrollbar Handles
	style->Colors[ImGuiCol_ScrollbarGrab]			= *(ImVec4*)&colours.scrollHandle;
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = *(ImVec4*)&colours.scrollHandleHover;
	style->Colors[ImGuiCol_ScrollbarGrabActive]	= *(ImVec4*)&colours.scrollHandleClick;

	// Borders & Bars
	style->Colors[ImGuiCol_Separator]				= *(ImVec4*)&colours.seperatorBar;
	style->Colors[ImGuiCol_Border]					= *(ImVec4*)&colours.windowBorder;

	// Tables
	style->Colors[ImGuiCol_TableBorderStrong]		= *(ImVec4*)&colours.tableBorderMain;
	style->Colors[ImGuiCol_TableBorderLight]		= *(ImVec4*)&colours.tableBorderInner;
	style->Colors[ImGuiCol_TableRowBg]				= *(ImVec4*)&colours.tableRowFill;
	style->Colors[ImGuiCol_TableRowBgAlt]			= *(ImVec4*)&colours.tableAltRowFill;

	// Docking
	style->Colors[ImGuiCol_DockingPreview]			= *(ImVec4*)&colours.docking;
	style->Colors[ImGuiCol_DockingEmptyBg]			= *(ImVec4*)&colours.emptyDockingFill;
	

	// To-Add
	//style->Colors[ImGuiCol_PlotLines];
	//style->Colors[ImGuiCol_PlotLinesHovered];
	//style->Colors[ImGuiCol_PlotHistogram];
	//style->Colors[ImGuiCol_PlotHistogramHovered];
	//style->Colors[ImGuiCol_NavHighlight];
	//style->Colors[ImGuiCol_NavWindowingHighlight];
	//style->Colors[ImGuiCol_NavWindowingDimBg];
	//style->Colors[ImGuiCol_ModalWindowDimBg];


	// No clue what these ones do:
	//style->Colors[ImGuiCol_SeparatorActive];
	//style->Colors[ImGuiCol_SliderGrab];
	//style->Colors[ImGuiCol_SliderGrabActive];
	//style->Colors[ImGuiCol_BorderShadow]


}
