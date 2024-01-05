#pragma once

#include "EditorCamera.h"
#include <fstream>
#include "json.hpp"
using json = nlohmann::json;

namespace EditorCameraConfig
{
	inline bool Load(EditorCamera* camera)
	{
		std::ifstream input("UserSettings/EditorCameraConfig.json");

		if (!input.good()) return false;

		json config;
		input >> config;

		if (!config.contains("Display")) config["Display"] = json();
		{
			json display = config["Display"];

			if (display.contains("FOV")) camera->fov = display["FOV"];
			else config["Display"]["FOV"] = camera->fov;

			if (!config["Display"].contains("Clipping Planes")) config["Display"]["Clipping Planes"] = json();
			{
				json clippingPlanes = config["Display"]["Clipping Planes"];

				if (clippingPlanes.contains("Near")) camera->nearClip = clippingPlanes["Near"];
				else config["Display"]["Clipping Planes"]["Near"] = camera->nearClip;

				if (clippingPlanes.contains("Far")) camera->farClip = clippingPlanes["Far"];
				else config["Display"]["Clipping Planes"]["Far"] = camera->farClip;
			}
		}

		if (!config.contains("Movement")) config["Movement"] = json();
		{
			json movement = config["Movement"];

			if (movement.contains("Fly Speed")) camera->flySpeed = movement["Fly Speed"];
			else config["Movement"]["Fly Speed"] = camera->flySpeed;

			if (movement.contains("Quick Fly Speed")) camera->quickFlySpeed = movement["Quick Fly Speed"];
			else config["Movement"]["Quick Fly Speed"] = camera->quickFlySpeed;
		}

		if (!config.contains("Controls")) config["Controls"] = json();
		{
			json controls = config["Controls"];

			if (controls.contains("Backward"))
			{
				std::vector<KeyCode> backwardKeyCodes = controls["Backward"];
				camera->zInput.ClearNegativeBinds();
				camera->zInput.BindNegative(backwardKeyCodes);
			}
			else config["Controls"]["Backward"] = camera->zInput.GetNegativeBinds();

			if (controls.contains("Forward"))
			{
				std::vector<KeyCode> forwardKeyCodes = controls["Forward"];
				camera->zInput.ClearPositiveBinds();
				camera->zInput.BindPositive(forwardKeyCodes);
			}
			else config["Controls"]["Forward"] = camera->zInput.GetPositiveBinds();

			if (controls.contains("Left"))
			{
				std::vector<KeyCode> leftKeyCodes = controls["Left"];
				camera->xInput.ClearNegativeBinds();
				camera->xInput.BindNegative(leftKeyCodes);
			}
			else config["Controls"]["Left"] = camera->xInput.GetNegativeBinds();

			if (controls.contains("Right"))
			{
				std::vector<KeyCode> rightKeyCodes = controls["Right"];
				camera->xInput.ClearPositiveBinds();
				camera->xInput.BindPositive(rightKeyCodes);
			}
			else config["Controls"]["Right"] = camera->xInput.GetPositiveBinds();

			if (controls.contains("Down"))
			{
				std::vector<KeyCode> downKeyCodes = controls["Down"];
				camera->yInput.ClearNegativeBinds();
				camera->yInput.BindNegative(downKeyCodes);
			}
			else config["Controls"]["Down"] = camera->xInput.GetNegativeBinds();

			if (controls.contains("Up"))
			{
				std::vector<KeyCode> upKeyCodes = controls["Up"];
				camera->yInput.ClearPositiveBinds();
				camera->yInput.BindPositive(upKeyCodes);
			}
			else config["Controls"]["Up"] = camera->xInput.GetPositiveBinds();

			if (controls.contains("Quick Mode"))
			{
				std::vector<KeyCode> quickModeKeyCodes = controls["Quick Mode"];
				camera->quickMode.ClearBinds();
				camera->quickMode.Bind(quickModeKeyCodes);
			}
			else config["Controls"]["Quick Mode"] = camera->quickMode.GetBinds();

			if (controls.contains("Free Camera"))
			{
				std::vector<KeyCode> freeCameraKeyCodes = controls["Free Camera"];
				camera->freeCamera.ClearBinds();
				camera->freeCamera.Bind(freeCameraKeyCodes);
			}
			else config["Controls"]["Free Camera"] = camera->freeCamera.GetBinds();
		}

		std::ofstream output("UserSettings/EditorCameraConfig.json");
		output << std::setw(4) << config << std::endl;
	}

	inline void Save(EditorCamera* camera)
	{
		json newConfig;

		newConfig["Display"]["FOV"] = camera->fov;
		newConfig["Display"]["Clipping Planes"]["Near"] = camera->nearClip;
		newConfig["Display"]["Clipping Planes"]["Far"] = camera->farClip;

		newConfig["Movement"]["Fly Speed"] = camera->flySpeed;
		newConfig["Movement"]["Quick Fly Speed"] = camera->quickFlySpeed;

		newConfig["Controls"]["Backward"] = camera->zInput.GetNegativeBinds();
		newConfig["Controls"]["Forward"] = camera->zInput.GetPositiveBinds();
		newConfig["Controls"]["Left"] = camera->xInput.GetNegativeBinds();
		newConfig["Controls"]["Right"] = camera->xInput.GetPositiveBinds();
		newConfig["Controls"]["Down"] = camera->yInput.GetNegativeBinds();
		newConfig["Controls"]["Up"] = camera->yInput.GetPositiveBinds();
		newConfig["Controls"]["Quick Mode"] = camera->quickMode.GetBinds();
		newConfig["Controls"]["Free Camera"] = camera->freeCamera.GetBinds();

		std::ofstream output("UserSettings/EditorCameraConfig.json");
		output << std::setw(4) << newConfig << std::endl;
	}
}