#pragma once

#include <random>
using random64 = std::mt19937_64;

namespace GuidGenerator
{
	inline random64 random;

	inline unsigned long long int NewGuid()
	{
		return random();
	}
}