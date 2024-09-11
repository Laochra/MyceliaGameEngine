#pragma once

namespace SoLoud
{
	class Soloud;
	class Wav;
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

	static void PlaySound(const char* const filepath);

private:
	static SoLoud::Soloud audioEngine;

	static map<string, SoLoud::Wav> loadedAudioAssets;
};
