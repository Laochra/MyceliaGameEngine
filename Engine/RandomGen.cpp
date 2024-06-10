#include "RandomGen.h"

#include <random>
#include <chrono>

namespace Random
{
	static long long SecondsSinceEpoch()
	{
		using namespace std::chrono;

		const system_clock::time_point now = system_clock::now();
		const duration timeSinceEpoch = now.time_since_epoch();
		const seconds s = duration_cast<seconds>(timeSinceEpoch);

		return s.count();
	}

	typedef std::mt19937 Engine32;
	Engine32 engine32(SecondsSinceEpoch());
	typedef std::mt19937_64 Engine64;
	Engine64 engine64(SecondsSinceEpoch());
}


int32 Random::Int32()
{
	return engine32();
}
int32 Random::Int32(int32 min, int32 max)
{
	typedef std::uniform_int_distribution<int32> Int32Dist;
	return Int32Dist(min, max)(engine32);
}
uint32 Random::UInt32()
{
	return engine32();
}
uint32 Random::UInt32(uint32 min, uint32 max)
{
	typedef std::uniform_int_distribution<uint32> UInt32Dist;
	return UInt32Dist(min, max)(engine32);
}

int64 Random::Int64()
{
	return engine64();
}
int64 Random::Int64(int64 min, int64 max)
{
	typedef std::uniform_int_distribution<int64> Int64Dist;
	return Int64Dist(min, max)(engine64);
}
uint64 Random::UInt64()
{
	return engine64();
}
uint64 Random::UInt64(uint64 min, uint64 max)
{
	typedef std::uniform_int_distribution<uint64> UInt64Dist;
	return UInt64Dist(min, max)(engine64);
}

float Random::Float()
{
	return engine32();
}
float Random::Float(float min, float max)
{
	typedef std::uniform_real_distribution<float> FloatDist;
	return FloatDist(min, max)(engine32);
}
