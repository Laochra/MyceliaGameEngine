#pragma once

#include <combaseapi.h>

namespace GuidGenerator
{
	inline unsigned long int NewGuid()
	{
		GUID newGuid;
		(void)CoCreateGuid(&newGuid);

		return newGuid.Data1;
	}
}