#pragma once

#include "ImGuiIncludes.h"

#include <string>
using std::string;
#include <vector>
using std::vector;
using std::array;
#include <map>
using std::map;
using std::pair;
#include <initializer_list>
typedef std::initializer_list<string> StringParams;

class GameObject;

namespace GUI
{
	typedef unsigned int uint;

	extern string GenerateID(GameObject* gameObject);
	extern string GenerateID(const char* name, GameObject* gameObject);
	extern string InvisibleName(const char* uniqueName);

	extern void Spacing(uint numberOfSpacings);

	extern string currentStyle;
	extern map<string, string> styleMap;
	extern void SaveStyle();
	extern void LoadStyle(string styleName);
	extern void UpdateStyle();
	struct Colour
	{
		float r, g, b, a;

		Colour() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
		Colour(float whiteness) : r(whiteness), g(whiteness), b(whiteness), a(1.0f) {}
		Colour(float whiteness, float alpha) : r(whiteness), g(whiteness), b(whiteness), a(alpha) {}
		Colour(float red, float green, float blue) : r(red), g(green), b(blue), a(1.0f) {}
		Colour(float red, float green, float blue, float alpha) : r(red), g(green), b(blue), a(alpha) {}

		operator float*() { return (float*)this; }
	};
	struct EditorColours
	{
		Colour text						= Colour(1.0f,  1.0f,  1.0f);		// Text
		Colour textDisabled			= Colour(0.5f,  0.5f,  0.5f);		// TextDisabled
		Colour textSelection			= Colour(0.5f,  0.3f,  0.55f);	// TextSelectedBg

		Colour background				= Colour(0.1f,  0.1f,  0.15f);	// WindowBg, ChildBg, PopupBg, TitleBg, TitleBgCollapsed, ResizeGrip
		Colour backgroundActive		= Colour(0.1f,  0.05f, 0.15f);	// TitleBgActive
		Colour accentBackground		= Colour(0.05f, 0.0f,  0.1f);		// MenuBarBg, ScrollbarBg

		Colour inputBoxes				= Colour(0.3f,  0.3f,  0.4f);		// FrameBg
		Colour inputBoxesHover		= Colour(0.35f, 0.35f, 0.5f);		// FrameBgHovered
		Colour inputBoxesClick		= Colour(0.4f,  0.35f, 0.55f);	// FrameBgActive
		Colour toggledInput			= Colour(0.85f, 0.65f, 1.0f);		// CheckMark, DragDropTarget

		Colour interactable			= Colour(0.5f,  0.3f,  0.55f);	// Button
		Colour interactableHover	= Colour(0.6f,  0.35f, 0.65f);	// ButtonHovered, SeparatorHovered, ResizeGripHovered
		Colour interactableClick	= Colour(0.65f, 0.4f,  0.7f);		// ButtonActive, ResizeGripActive

		Colour header					= Colour(0.5f,  0.3f,  0.55f);	// Header, TableHeaderBg
		Colour headerHover			= Colour(0.6f,  0.35f, 0.65f);	// HeaderHovered
		Colour headerClick			= Colour(0.65f, 0.4f,  0.7f);		// HeaderActive

		Colour tab						= Colour(0.5f,  0.3f,  0.55f);	// Tab
		Colour tabHover				= Colour(0.6f,  0.35f, 0.65f);	// TabHovered
		Colour tabActive				= Colour(0.65f, 0.4f,  0.7f);		// TabActive
		Colour tabUnfocused			= Colour(0.35f, 0.25f, 0.4f);		// TabUnfocused
		Colour tabUnfocusedActive	= Colour(0.45f, 0.3f,  0.5f);		// TabUnfocusedActive

		Colour scrollHandle			= Colour(0.15f, 0.15f, 0.2f);		// ScrollbarGrab
		Colour scrollHandleHover	= Colour(0.25f, 0.2f,  0.3f);		// ScrollbarGrabHovered
		Colour scrollHandleClick	= Colour(0.5f,  0.3f,  0.55f);	// ScrollbarGrabActive

		Colour seperatorBar			= Colour(0.25f, 0.25f, 0.35f);	// Seperator
		Colour windowBorder			= Colour(0.05f, 0.0f,  0.1f);		// Border

		Colour tableBorderMain		= Colour(0.85f, 0.8f, 1.0f, 1.0f);	// TableBorderStrong
		Colour tableBorderInner		= Colour(0.85f, 0.8f, 1.0f, 0.25f);	// TableBorderLight
		Colour tableRowFill			= Colour(0.75f, 0.7f, 1.0f, 0.25f);	// TableRowBg
		Colour tableAltRowFill		= Colour(0.75f, 0.5f, 1.0f, 0.1f);	// TableRowBgAlt

		Colour docking					= Colour(0.5f,  0.3f,  0.55f);	// DockingPreview
		Colour emptyDockingFill		= Colour(0.05f, 0.05f, 0.1f);		// DockingEmptyBg
	};

	extern EditorColours colours;
}