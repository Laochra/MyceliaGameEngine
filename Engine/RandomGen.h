#pragma once

typedef signed int int32;
typedef unsigned int uint32;

typedef signed long long int64;
typedef unsigned long long uint64;

namespace Random
{
	int32 Int32();
	int32 Int32(int32 min, int32 max);
	uint32 UInt32();
	uint32 UInt32(uint32 min, uint32 max);

	int64 Int64();
	int64 Int64(int64 min, int64 max);
	uint64 UInt64();
	uint64 UInt64(uint64 min, uint64 max);

	float Float();
	float Float(float min, float max);
}