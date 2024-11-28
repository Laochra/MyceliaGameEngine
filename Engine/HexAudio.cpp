#include "HexAudio.h"

#include "AudioManager.h"

#include "HabitatData.h"

string HexAudio::music = "None";
float HexAudio::musicVolume = 1.0f;

vector<string> HexAudio::ambience;
float HexAudio::ambienceVolume = 1.0f;

map<string, string> HexAudio::spiritSounds;
float HexAudio::spiritSFXVolume = 1.0f;

using SoundEffect = HexAudio::SoundEffect;
map<SoundEffect, string> HexAudio::soundEffectNames {
	{ SoundEffect::PlaceTree, "PlaceTree" }, { SoundEffect::PlaceFlower, "PlaceFlower" }, { SoundEffect::PlaceWater, "PlaceWater" }, { SoundEffect::PlaceLand, "PlaceLand" },
	{ SoundEffect::FormHabitat, "FormHabitat" },
	{ SoundEffect::VariantUnlock, "VariantUnlock" }, { SoundEffect::FogMove, "FogMove" },
	{ SoundEffect::RadialOpen, "RadialOpen" }, { SoundEffect::RadialClose, "RadialClose" }, { SoundEffect::RadialSelect, "RadialSelect" }, { SoundEffect::RadialHover, "RadialHover" }, { SoundEffect::RadialCyclePage, "RadialCyclePage" },
	{ SoundEffect::ScrapbookOpen, "ScrapbookOpen" }, { SoundEffect::ScrapbookClose, "ScrapbookClose" }, { SoundEffect::ScrapbookStickerPlace, "ScrapbookStickerPlace" },
	{ SoundEffect::MenuSelect, "MenuSelect" },
};
string HexAudio::soundEffects[SoundEffect_Count] { "None", "None", "None", "None", "None", "None", "None", "None", "None", "None", "None", "None", "None", "None", "None", "None" };
float HexAudio::sfxVolume = 1.0f;

void HexAudio::BeginMusic() noexcept
{
	AudioManager::BeginLoop(music.c_str(), musicVolume);
}
void HexAudio::EndMusic() noexcept
{
	AudioManager::EndLoop(music.c_str());
}

void HexAudio::BeginAmbience() noexcept
{
	for (const string& track : ambience)
	{
		AudioManager::BeginLoop(track.c_str(), ambienceVolume);
	}
}
void HexAudio::EndAmbience() noexcept
{
	for (const string& track : ambience)
	{
		AudioManager::EndLoop(track.c_str());
	}
}

void HexAudio::PlaySpiritSFX(string spiritHabitatName) noexcept
{
	AudioManager::PlaySound(spiritSounds[spiritHabitatName].c_str(), spiritSFXVolume);
}

void HexAudio::PlayMiscSFX(SoundEffect soundEffect) noexcept
{
	AudioManager::PlaySound(soundEffects[soundEffect].c_str(), sfxVolume);
}

void HexAudio::EndAll() noexcept
{
	AudioManager::EndAll();
}

void HexAudio::SaveTo(json& jsonObj) noexcept
{
	jsonObj["Music"] = music;
	jsonObj["MusicVolume"] = musicVolume;

	jsonObj["Ambience"] = ambience;
	jsonObj["AmbienceVolume"] = ambienceVolume;

	json spiritSoundsJSON;
	for (map<string, string>::iterator it = spiritSounds.begin(); it != spiritSounds.end(); it++)
	{
		spiritSoundsJSON[it->first] = it->second;
	}
	jsonObj["SpiritSounds"] = spiritSoundsJSON;
	jsonObj["SpiritSoundsVolume"] = spiritSFXVolume;

	json soundEffectsJSON;
	for (int s = 0; s < SoundEffect_Count; s++)
	{
		soundEffectsJSON[soundEffectNames[SoundEffect(s)]] = soundEffects[s];
	}
	jsonObj["SoundEffects"] = soundEffectsJSON;
	jsonObj["SoundEffectsVolume"] = sfxVolume;
}

void HexAudio::LoadFrom(const json& jsonObj) noexcept
{
	json::const_iterator musicJSONIt = jsonObj.find("Music");
	if (musicJSONIt != jsonObj.end())
	{
		music = *musicJSONIt;
		musicVolume = jsonObj["MusicVolume"];
	}

	json::const_iterator ambienceJSONIt = jsonObj.find("Ambience");
	if (ambienceJSONIt != jsonObj.end())
	{
		ambience = *ambienceJSONIt;
		ambienceVolume = jsonObj["AmbienceVolume"];
	}

	json::const_iterator spiritSoundsJSONIt = jsonObj.find("SpiritSounds");
	if (spiritSoundsJSONIt != jsonObj.end())
	{
		const json& spiritSoundsJSON = *spiritSoundsJSONIt;
		for (HabitatData habitatData : HabitatData::habitatsData)
		{
			spiritSounds.insert(std::pair<string, string>(habitatData.name, spiritSoundsJSON[habitatData.name]));
		}
		spiritSFXVolume = jsonObj["SpiritSoundsVolume"];
	}

	json::const_iterator soundEffectsJSONIt = jsonObj.find("SoundEffects");
	if (soundEffectsJSONIt != jsonObj.end())
	{
		for (int s = 0; s < SoundEffect_Count; s++)
		{
			string& name = soundEffectNames[SoundEffect(s)];
			if (soundEffectsJSONIt->contains(name))
			{
				soundEffects[s] = (*soundEffectsJSONIt)[name];
			}
		}
		sfxVolume = jsonObj["SoundEffectsVolume"];
	}
}
