#pragma once

#include "JsonIncludes.h"

#include <string>
using std::string;
#include <vector>
using std::vector;
#include <map>
using std::map;

class HexAudio
{
public:
	static string music;
	static float musicVolume;

	static vector<string> ambience;
	static float ambienceVolume;

	static map<string, string> spiritSounds;
	static float spiritSFXVolume;

	enum SoundEffect
	{
		PlaceTree,
		PlaceFlower,
		PlaceWater,
		PlaceLand,

		FormHabitat,

		VariantUnlock,
		FogMove,

		RadialOpen,
		RadialClose,
		RadialSelect,
		RadialHover,
		RadialCyclePage,

		ScrapbookOpen,
		ScrapbookClose,
		ScrapbookStickerPlace,

		MenuSelect,

		SoundEffect_First = PlaceTree,
		SoundEffect_Last = MenuSelect,
		SoundEffect_Count = SoundEffect_Last + 1
	};
	static string soundEffects[SoundEffect_Count];
	static map<SoundEffect, string> soundEffectNames;
	static float sfxVolume;

	static void BeginMusic() noexcept;
	static void EndMusic() noexcept;
	
	static void BeginAmbience() noexcept;
	static void EndAmbience() noexcept;

	static void PlaySpiritSFX(string spiritHabitatName) noexcept;

	static void PlayMiscSFX(SoundEffect soundEffect) noexcept;

	static void EndAll() noexcept;


	static void SaveTo(json& jsonObj) noexcept;
	static void LoadFrom(const json& jsonObj) noexcept;
};
