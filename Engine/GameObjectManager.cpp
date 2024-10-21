#include "GameObjectManager.h"

//#include "GameObject2D.h"
#include "GameObject3D.h"
#include "MeshRenderer.h"
#include "LightObject.h"
#include "Camera.h"

#include "GeneralMacros.h"

#include "Debug.h"

GameObjectManager* gameObjectManager;

GameObjectManager::~GameObjectManager() noexcept
{
	Clear();
}

void GameObjectManager::Add(GameObject* gameObject) noexcept
{
	gameObjects.push_back(gameObject);
}
void GameObjectManager::Remove(GameObject* gameObject) noexcept
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		if (*gameObjects[i] == *gameObject)
		{
			gameObjects.erase(gameObjects.begin() + i);
			break;
		}
	}
}
void GameObjectManager::Bury(GameObject* gameObject) noexcept
{
	graveyard.push_back(gameObject);
}
void GameObjectManager::Unbury(GameObject* gameObject) noexcept
{
	for (int i = 0; i < graveyard.size(); i++)
	{
		if (*graveyard[i] == *gameObject)
		{
			graveyard.erase(graveyard.begin() + i);
			break;
		}
	}
}
void GameObjectManager::Move(GameObject* gameObject, int newIndex) noexcept
{
	int i = GetIndexOf(gameObject);

	if (i == newIndex) return;

	char direction = (i < newIndex) ? 1 : -1;

	GameObject* current;
	while (i != newIndex)
	{
		if (i + direction < 0 || i + direction >= gameObjects.size()) return;

		current = gameObjects[i];
		gameObjects[i] = gameObjects[i + direction];
		gameObjects[i + direction] = current;

		i += direction;
	}
}
GameObject* GameObjectManager::Find(unsigned long long guid) const noexcept
{
	for (int c = 0; c < gameObjects.size(); c++)
	{
		if (gameObjects[c]->guid == guid) return gameObjects[c];

		if (dynamic_cast<GameObject3D*>(gameObjects[c]) != nullptr)
		{
			GameObject3D* potentialMatch = FindChild((GameObject3D*)gameObjects[c], guid);
			if (potentialMatch != nullptr) return potentialMatch;
		}
	}
	return nullptr;
}
GameObject* GameObjectManager::FindInGraveyard(unsigned long long guid) const noexcept
{
	for (int c = 0; c < graveyard.size(); c++)
	{
		if (graveyard[c]->guid == guid) return graveyard[c];

		if (dynamic_cast<GameObject3D*>(graveyard[c]) != nullptr)
		{
			GameObject3D* potentialMatch = FindChild((GameObject3D*)graveyard[c], guid);
			if (potentialMatch != nullptr) return potentialMatch;
		}
	}
	return nullptr;
}
GameObject3D* GameObjectManager::FindChild(const GameObject3D* gameObject, unsigned long long guid) const noexcept
{
	const std::vector<GameObject3D*>& children = *gameObject->GetChildren();

	for (int c = 0; c < children.size(); c++)
	{
		if (children[c]->guid == guid) return children[c];
		GameObject3D* potentialMatch = FindChild(children[c], guid);
		if (potentialMatch != nullptr) return potentialMatch;
	}
	return nullptr;
}
void GameObjectManager::Delete(GameObject* gameObject)
{
	if (gameObject == nullptr) return;

	if (gameObject != GameObject::Destroyed)
	{
		GameObject::Destroy(gameObject);
	}

	del(gameObject);
}

int GameObjectManager::GetIndexOf(GameObject* gameObject) const noexcept
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i] == gameObject)
		{
			return i;
		}
	}

	return -1;
}

void GameObjectManager::Clear() noexcept
{
	Updater::fixedUpdateList.clear();
	Updater::updateList.clear();
	Updater::drawList.clear();

	if (AppInfo::editorCamera != nullptr) Updater::updateList.push_back(AppInfo::editorCamera);
	if (AppInfo::gameCamera != nullptr) Updater::updateList.push_back(AppInfo::gameCamera);

	while (gameObjects.size() > 0)
	{
		GameObject::Destroy(gameObjects[0]);
	}
	while (graveyard.size() > 0)
	{
		Delete(graveyard[0]);
		graveyard.erase(graveyard.begin() + 0);
	}
}
void GameObjectManager::ClearGraveyard() noexcept
{
	while (graveyard.size() > 0)
	{
		Delete(graveyard[0]);
		graveyard.erase(graveyard.begin() + 0);
	}
}

void GameObjectManager::DrawGUIDs(uint& framebuffer, uint& texture, uint& depth, DrawFunc drawTransformIDsFunction) noexcept
{
	glViewport(0, 0, AppInfo::screenWidth, AppInfo::screenHeight);

	if (framebuffer == 0)
	{
		glGenFramebuffers(1, &framebuffer);
		glGenRenderbuffers(1, &depth);
		glGenTextures(1, &texture);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	if (AppInfo::screenSizeJustChanged)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, depth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, AppInfo::screenWidth, AppInfo::screenHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth);

		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32UI, AppInfo::screenWidth, AppInfo::screenHeight, 0, GL_RG_INTEGER, GL_UNSIGNED_INT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	}

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	Updater::CallDrawGUIDs();
	if (drawTransformIDsFunction != nullptr)
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		drawTransformIDsFunction();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void DrawGizmos(uint& gizmosTexture, GameObjectManager::DrawFunc drawTransformsFunction) noexcept
{
	if (gizmosTexture == 0)
	{
		glGenTextures(1, &gizmosTexture);
	}
	if (AppInfo::screenSizeJustChanged)
	{
		glBindTexture(GL_TEXTURE_2D, gizmosTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, AppInfo::screenWidth, AppInfo::screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gizmosTexture, 0);
	uint attachment[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachment);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	AppInfo::debug->lines.Draw();

	glClear(GL_DEPTH_BUFFER_BIT);

	if (drawTransformsFunction != nullptr)
	{
		drawTransformsFunction();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameObjectManager::DrawScene(
	uint& framebuffer,
	uint& texture,
	uint& depth, 
	uint& positionTexture, 
	uint& idTexture, 
	uint& gizmosTexture,
	DrawFunc drawTransformsFunction) noexcept
{
	if (framebuffer == 0)
	{
		glGenFramebuffers(1, &framebuffer);
		glGenRenderbuffers(1, &depth);
		glGenTextures(1, &texture);
		glGenTextures(1, &positionTexture);
		glGenTextures(1, &idTexture);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	if (AppInfo::screenSizeJustChanged)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, depth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, AppInfo::screenWidth, AppInfo::screenHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth);

		for (uint texId = 0; texId < 3; texId++)
		{
			switch (texId)
			{
			case 0: glBindTexture(GL_TEXTURE_2D, texture); break;
			case 1: glBindTexture(GL_TEXTURE_2D, positionTexture); break;
			case 2: glBindTexture(GL_TEXTURE_2D, idTexture); break;
			}
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, AppInfo::screenWidth, AppInfo::screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, positionTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, idTexture, 0);
	uint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, AppInfo::screenWidth, AppInfo::screenHeight);

	Updater::CallDraw();

	DrawGizmos(gizmosTexture, drawTransformsFunction);
}
