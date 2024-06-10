#include "GuidGenerator.h"

#include "RandomGen.h"

namespace GuidGenerator
{
	unsigned long long NewGuid()
	{
		return Random::UInt64();
	}
}
