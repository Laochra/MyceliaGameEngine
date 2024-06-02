#include "GuidGenerator.h"

#include <random>
#include <chrono>

namespace GuidGenerator
{
	static long long SecondsSinceEpoch()
	{
		using namespace std::chrono;

		const system_clock::time_point now = system_clock::now();
		const duration timeSinceEpoch = now.time_since_epoch();
		const seconds s = duration_cast<seconds>(timeSinceEpoch);

		return s.count();
	}

	using Random64 = std::mt19937_64;
	Random64 random(SecondsSinceEpoch());

	unsigned long long NewGuid()
	{
		return random();
	}
}
