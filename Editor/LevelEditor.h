#pragma once

#include "GUI.h"
#include "LineDrawer.h"
#include "Mesh.h"
#include "LevelEditorCamera.h"

namespace LevelEditor
{
	inline bool initialised = false;

	inline unsigned int frameBuffer;
	inline unsigned int colourBuffer;

	inline Camera* camera;
	inline LineDrawer lineDrawer;

	inline GameObject3D* levelRoot;

	struct LevelTile
	{
		bool isOpen = false;
		bool isNavigable = false;
	};

	inline const int tileCount = 64;
	inline const int tileCountSqrt = 8;
	#define TILE_WIDTH (0.125f * screenWidth)
	#define TILE_HEIGHT (0.125f * screenHeight)
	inline LevelTile tiles[tileCount];

	inline Colour clearColour = Colour(0.15f, 0.15f, 0.15f);

	enum class EditMode { Tile, Path };
	EditMode editMode = EditMode::Tile;

	inline void Draw() noexcept;
	inline void DrawLines() noexcept;
}


void LevelEditor::Draw() noexcept
{
	if (!initialised)
	{
		camera = GameObject3D::Instantiate<LevelEditorCamera>(vec3(0, 0, 1));
		lineDrawer;
	}

	unsigned int previousFrameBuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&previousFrameBuffer);
	unsigned int previousDepthSetting;
	glGetIntegerv(GL_DEPTH_TEST, (GLint*)&previousDepthSetting);
	unsigned int previousCullSetting;
	glGetIntegerv(GL_CULL_FACE, (GLint*)&previousCullSetting);

	Camera* previousCamera = mainCamera;
	mainCamera = camera;

	int previousScreenWidth  = screenWidth;
	int previousScreenHeight = screenHeight;

	screenWidth = (int)ImGui::GetWindowWidth();
	float titleBarHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
	screenHeight = (int)(ImGui::GetWindowHeight() - titleBarHeight);

	if (screenWidth == 0 || screenHeight == 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, previousFrameBuffer);
		mainCamera = previousCamera;
		screenWidth = previousScreenWidth;
		screenHeight = previousScreenHeight;
		return;
	}

	glViewport(0, 0, screenWidth, screenHeight);

	if (!initialised)
	{
		glGenFramebuffers(1, &frameBuffer);
		glGenTextures(1, &colourBuffer);
		initialised = true;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glBindTexture(GL_TEXTURE_2D, colourBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourBuffer, 0);

	glClearColor(clearColour.r, clearColour.g, clearColour.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	Mesh screenQuad;
	screenQuad.InitialiseQuad();

	DrawLines();

	screenQuad.Draw();


	uintptr_t image = (uintptr_t)colourBuffer; // Casting to a uintptr_t is required to stop a warning with converting 32bit uint to 64bit void*. ImGui::Image works regardless.
	ImGui::Image((void*)image, ImVec2((float)screenWidth, (float)screenHeight), ImVec2(0, 1), ImVec2(1, 0));

	mainCamera = previousCamera;
	screenWidth = previousScreenWidth;
	screenHeight = previousScreenHeight;
	
	glBindFramebuffer(GL_FRAMEBUFFER, previousFrameBuffer);
	if (previousFrameBuffer != 0) glViewport(0, 0, screenWidth, screenHeight);
	if (previousDepthSetting == true) glEnable(GL_DEPTH_TEST);
	if (previousCullSetting == true) glEnable(GL_CULL_FACE);
}

void LevelEditor::DrawLines() noexcept
{
	int halfCountSqrt = tileCountSqrt / 2;

	for (int y = 0; y <= tileCountSqrt; y++)
	{
		float yPos = y - halfCountSqrt;

		for (int x = 0; x <= tileCountSqrt; x++)
		{
			float xPos = x - halfCountSqrt;

			Colour hColour(0.4f, 0.4f, 0.4f);
			Colour vColour(0.4f, 0.4f, 0.4f);

			bool thisIsOpen = tiles[(x - 0) + (y + 0) * tileCountSqrt].isOpen;
			bool upIsOpen	 = tiles[(x - 0) + (y + 1) * tileCountSqrt].isOpen;
			bool leftIsOpen = tiles[(x - 1) + (y + 0) * tileCountSqrt].isOpen;

			if (thisIsOpen || upIsOpen)
			{
				if (thisIsOpen && upIsOpen) hColour = clearColour;
				else hColour = Colour(0.9f, 0.9f, 0.9f);
			}

			if (thisIsOpen || leftIsOpen)
			{
				if (thisIsOpen && leftIsOpen) vColour = clearColour;
				else vColour = Colour(0.9f, 0.9f, 0.9f);
			}

			if (xPos != halfCountSqrt && yPos != -halfCountSqrt)
			{
				lineDrawer.Add(
					vec3(TILE_WIDTH * xPos, TILE_HEIGHT * yPos, 0),
					vec3(TILE_WIDTH * xPos, TILE_HEIGHT * (yPos - 1), 0),
					vColour
				);
				if (yPos != halfCountSqrt)
				{
					lineDrawer.Add(
						vec3(TILE_WIDTH * xPos, TILE_HEIGHT * yPos, 0),
						vec3(TILE_WIDTH * (xPos + 1), TILE_HEIGHT * yPos, 0),
						hColour
					);
				}
			}
		}
	}

	if (ImGui::IsWindowHovered())
	{
		double cursorPos[2];
		glfwGetCursorPos(glfwGetCurrentContext(), &cursorPos[0], &cursorPos[1]);
		ImVec2 windowPos = ImGui::GetCurrentWindow()->Pos;
		ImVec2 relativeCursorPos = ImVec2(cursorPos[0] - windowPos.x, -cursorPos[1] - -windowPos.y);

		int hoveredTileX = std::floor(relativeCursorPos.x / TILE_WIDTH) - halfCountSqrt;
		int hoveredTileY = std::ceil(relativeCursorPos.y / TILE_HEIGHT) + halfCountSqrt;

		lineDrawer.Add(
			vec3((hoveredTileX + 0) * TILE_WIDTH, (hoveredTileY - 0) * TILE_HEIGHT, 0),
			vec3((hoveredTileX + 1) * TILE_WIDTH, (hoveredTileY - 0) * TILE_HEIGHT, 0),
			Colour(0.7f, 0.2f, 0.9f)
		);
		lineDrawer.Add(
			vec3((hoveredTileX + 1) * TILE_WIDTH, (hoveredTileY - 0) * TILE_HEIGHT, 0),
			vec3((hoveredTileX + 1) * TILE_WIDTH, (hoveredTileY - 1) * TILE_HEIGHT, 0),
			Colour(0.7f, 0.2f, 0.9f)
		);
		lineDrawer.Add(
			vec3((hoveredTileX + 1) * TILE_WIDTH, (hoveredTileY - 1) * TILE_HEIGHT, 0),
			vec3((hoveredTileX + 0) * TILE_WIDTH, (hoveredTileY - 1) * TILE_HEIGHT, 0),
			Colour(0.7f, 0.2f, 0.9f)
		);
		lineDrawer.Add(
			vec3((hoveredTileX + 0) * TILE_WIDTH, (hoveredTileY - 1) * TILE_HEIGHT, 0),
			vec3((hoveredTileX + 0) * TILE_WIDTH, (hoveredTileY - 0) * TILE_HEIGHT, 0),
			Colour(0.7f, 0.2f, 0.9f)
		);

		if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Enter))
		{
			vector<GameObject3D*>* levelChildren = levelRoot->GetChildren();
			while (levelChildren->size() > 0)
			{
				GameObject3D::Destroy((*levelChildren)[0]);
			}
			levelChildren->clear();

			for (int y = 1; y <= tileCountSqrt; y++)
			{
				float yPos = y - halfCountSqrt;

				for (int x = 0; x < tileCountSqrt; x++)
				{
					float xPos = x - halfCountSqrt;

					bool thisIsOpen  = tiles[( x ) + ( y ) * tileCountSqrt].isOpen;

					bool downIsOpen  = y != 1						&& tiles[( x ) + (y-1) * tileCountSqrt].isOpen;
					bool upIsOpen    = y != tileCountSqrt		&& tiles[( x ) + (y+1) * tileCountSqrt].isOpen;
					bool leftIsOpen  = x != 0						&& tiles[(x-1) + ( y ) * tileCountSqrt].isOpen;
					bool rightIsOpen = x != tileCountSqrt - 1 && tiles[(x+1) + ( y ) * tileCountSqrt].isOpen;

					if (thisIsOpen)
					{
						MeshRenderer* floor = GameObject3D::Instantiate<MeshRenderer>(vec3(xPos, 0.0f, -yPos), glm::identity<quat>(), vec3(1.00f, 1.00f, 1.00f), vec3(-0.5f, 0.0f, -0.5f), levelRoot);
						floor->SetName(("Floor " + std::to_string(xPos) + ", " + std::to_string(yPos)).c_str());
						floor->SetMesh("ProceduralQuad"); floor->SetMaterial("Assets\\Materials\\Wood Planks.mat");

						if (!downIsOpen)
						{
							MeshRenderer* wall = GameObject3D::Instantiate<MeshRenderer>(vec3(xPos, 0.0f, -yPos + 1), glm::identity<quat>(), vec3(1.00f, 1.00f, 1.00f), vec3(0.5f, 0.0f, -0.5f), levelRoot);
							wall->Rotate(glm::radians(-90.0f), vec3(0, 0, 1));
							wall->Rotate(glm::radians(-90.0f), vec3(1, 0, 0));
							wall->SetName(("Wall " + std::to_string(xPos) + ", " + std::to_string(yPos)).c_str());
							wall->SetMesh("ProceduralQuad"); wall->SetMaterial("Assets\\Materials\\Wallpaper.mat");
						}
						if (!upIsOpen)
						{
							MeshRenderer* wall = GameObject3D::Instantiate<MeshRenderer>(vec3(xPos, 0.0f, -yPos), glm::identity<quat>(), vec3(1.00f, 1.00f, 1.00f), vec3(-0.5f, 0.0f, -0.5f), levelRoot);
							wall->Rotate(glm::radians(90.0f), vec3(0, 0, 1));
							wall->Rotate(glm::radians(90.0f), vec3(1, 0, 0));
							wall->SetName(("Wall " + std::to_string(xPos) + ", " + std::to_string(yPos)).c_str());
							wall->SetMesh("ProceduralQuad"); wall->SetMaterial("Assets\\Materials\\Wallpaper.mat");
						}
						if (!leftIsOpen)
						{
							MeshRenderer* wall = GameObject3D::Instantiate<MeshRenderer>(vec3(xPos, 0.0f, -yPos), glm::identity<quat>(), vec3(1.00f, 1.00f, 1.00f), vec3(0.5f, 0.0f, -0.5f), levelRoot);
							wall->Rotate(glm::radians(-90.0f), vec3(0, 0, 1));
							wall->SetName(("Wall " + std::to_string(xPos) + ", " + std::to_string(yPos)).c_str());
							wall->SetMesh("ProceduralQuad"); wall->SetMaterial("Assets\\Materials\\Wallpaper.mat");
						}
						if (!rightIsOpen)
						{
							MeshRenderer* wall = GameObject3D::Instantiate<MeshRenderer>(vec3(xPos + 1, 0.0f, -yPos), glm::identity<quat>(), vec3(1.00f, 1.00f, 1.00f), vec3(-0.5f, 0.0f, -0.5f), levelRoot);
							wall->Rotate(glm::radians(90.0f), vec3(0, 0, 1));
							wall->SetName(("Wall " + std::to_string(xPos) + ", " + std::to_string(yPos)).c_str());
							wall->SetMesh("ProceduralQuad"); wall->SetMaterial("Assets\\Materials\\Wallpaper.mat");
						}
					}
				}
			}
		}

		switch (editMode)
		{
		case EditMode::Tile:
		{
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				tiles[(hoveredTileX + halfCountSqrt) + (hoveredTileY + halfCountSqrt) * tileCountSqrt].isOpen = true;
			}
			else if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
			{
				tiles[(hoveredTileX + halfCountSqrt) + (hoveredTileY + halfCountSqrt) * tileCountSqrt].isOpen = false;
			}
			break;
		}
		case EditMode::Path:
		{
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				tiles[(hoveredTileX + halfCountSqrt) + (hoveredTileY + halfCountSqrt) * tileCountSqrt].isNavigable = true;
			}
			else if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
			{
				tiles[(hoveredTileX + halfCountSqrt) + (hoveredTileY + halfCountSqrt) * tileCountSqrt].isNavigable = false;
			}
			break;
		}
		}
		
	}

	lineDrawer.Draw();
}
