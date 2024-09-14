#pragma once

namespace SoLoud
{
	class Soloud;
	class Wav;
	typedef unsigned int handle;
}

#include <map>
using std::map;
#include <vector>
using std::vector;
#include <string>
using std::string;

class AudioManager
{
public:
	static void Initialise() noexcept;
	static void CleanUp() noexcept;

	static void PlaySound(const char* const filepath, float volume) noexcept;
	static void BeginLoop(const char* const filepath, float volume) noexcept;
	static void EndLoop(const char* const filepath) noexcept;

	static void EndAll() noexcept;
	static void ClearLoadedAssets() noexcept;

private:
	static SoLoud::Soloud audioEngine;

	static map<string, SoLoud::Wav> loadedAudioAssets;
	static map<string, SoLoud::handle> loopingTracks;
};
