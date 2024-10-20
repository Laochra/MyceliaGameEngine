#include "MycCoroutine.h"

void Coroutine::Update(float delta) noexcept
{
	for (int i = 0; i < (int)Coroutine::list.size(); i++)
	{
		if (Coroutine::list[i].function == nullptr) continue;

		Coroutine::FunctionInterface& function = *Coroutine::list[i].function;
		Coroutine::Package& package = Coroutine::list[i].package;

		if (package.yieldTimer > 0.0f)
		{
			package.yieldTimer -= delta;
		}
		else
		{
			package.yieldTimer = 0.0f;
			function(package);

			if (package.state == Coroutine::State::Finalised)
			{
				delete &function;
				Coroutine::list[i].function = nullptr;
				if (package.ownsData) delete &package.data;
				package.data = nullptr;
				package.ownsData = false;
			}
		}
	}
}
