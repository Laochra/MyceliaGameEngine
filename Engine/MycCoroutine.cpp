#include "MycCoroutine.h"

void Coroutine::Update(float delta) noexcept
{
	for (auto it = Coroutine::active.begin(); it != Coroutine::active.end();)
	{
		Coroutine::FunctionInterface& coroutine = *it->first;
		Coroutine::Package& package = it->second;

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
				delete &coroutine;
				it = Coroutine::active.erase(it);
				continue;
			}
		}

		++it;
	}
}
