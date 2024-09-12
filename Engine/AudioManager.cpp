#include "AudioManager.h"

#include "soloud.h"
#include "soloud_wav.h"

SoLoud::Soloud AudioManager::audioEngine;
map<string, SoLoud::Wav> AudioManager::loadedAudioAssets;

void AudioManager::Initialise() noexcept
{
	audioEngine.init();
}

void AudioManager::CleanUp() noexcept
{
	audioEngine.deinit();
}

void AudioManager::PlaySound(const char* const filepath)
{
	SoLoud::Wav* audioAsset;
	if (!loadedAudioAssets.contains(filepath))
	{
		auto it = loadedAudioAssets.insert(std::pair((string)filepath, SoLoud::Wav()));
		it.first->second.load(filepath);
		audioAsset = &it.first->second;
	}
	else
	{
		audioAsset = &loadedAudioAssets[filepath];
	}
	audioEngine.play(*audioAsset);
}
