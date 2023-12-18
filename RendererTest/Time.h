#pragma once

namespace Time
{
	extern float time;
	extern float delta;
	extern float fixedStep;

	inline void Tick(float newTime)
	{
		delta = newTime - time;
		time = newTime;
	}
}