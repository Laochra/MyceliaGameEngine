#include "AudioManager.h"

#include "soloud.h"
#include "soloud_wav.h"

SoLoud::Soloud AudioManager::audioEngine;
map<string, SoLoud::Wav> AudioManager::loadedAudioAssets;
map<string, SoLoud::handle> AudioManager::loopingTracks;

void AudioManager::Initialise() noexcept
{
	audioEngine.init();
}

void AudioManager::CleanUp() noexcept
{
	audioEngine.deinit();
}

void AudioManager::PlaySound(const char* const filepath, float volume) noexcept
{
	if (filepath == nullptr || filepath == "None") return;

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
	audioEngine.play(*audioAsset, volume);
}

void AudioManager::BeginLoop(const char* const filepath, float volume) noexcept
{
	if (filepath == nullptr || filepath == "None") return;

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
	SoLoud::handle track = audioEngine.play(*audioAsset, volume);
	audioEngine.setLooping(track, true);
	loopingTracks.insert(std::pair(string(filepath), track));
}
void AudioManager::EndLoop(const char* const filepath) noexcept
{
	if (filepath == nullptr || filepath == "None") return;

	audioEngine.stop(loopingTracks[filepath]);
}

void AudioManager::EndAll() noexcept
{
	audioEngine.stopAll();
}

void AudioManager::ClearLoadedAssets() noexcept
{
	loadedAudioAssets.clear();
}
