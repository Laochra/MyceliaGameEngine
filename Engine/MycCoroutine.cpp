#include "MycCoroutine.h"

void Coroutine::Update(float delta) noexcept
{
	for (int i = 0; i < (int)Coroutine::active.size();)
	{
		Coroutine::FunctionInterface& coroutine = *Coroutine::active[i].first;
		Coroutine::Package& package = Coroutine::active[i].second;

		if (package.yieldTimer > 0.0f)
		{
			package.yieldTimer -= delta;
		}
		else
		{
			package.yieldTimer = 0.0f;
			coroutine(package);
			if (package.state == Coroutine::State::Finalised)
			{
				delete& coroutine;
				Coroutine::active.erase(Coroutine::active.begin() + i);
				continue;
			}
		}

		++i;
	}
}
