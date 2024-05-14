#include "RandomGen.h"

namespace Random
{
	typedef std::default_random_engine Engine;
	Engine engine(Time::time);
}

float Random::Float(float min, float max)
{
	typedef std::uniform_real_distribution<float> FloatDist;
	return FloatDist(min, max)(engine);
}