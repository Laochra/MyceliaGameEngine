#include "HexScrapbook.h"

#include "TimeManager.h"
#include "MycCoroutine.h"
#include "Easing.h"

UISprite* HexScrapbook::base;
std::string HexScrapbook::baseTexture;

std::vector<HexScrapbook::HabitatCollection> HexScrapbook::habitats;

float HexScrapbook::openness = 0.0f;
float HexScrapbook::animationSpeed = 2.0f;
HexScrapbook::State HexScrapbook::state = HexScrapbook::State::Closed;

void HexScrapbook::SetState(State newState) noexcept
{
	state = newState;

	bool shouldBeEnabled;
	switch (newState)
	{
	case State::Opening: shouldBeEnabled = true; break;
	case State::Closed: shouldBeEnabled = false; break;
	default: return;
	}

	base->enabled = shouldBeEnabled;
	for (HabitatCollection& habitat : habitats)
	{
		habitat.habitat->enabled = shouldBeEnabled;
		habitat.tiles[0]->enabled = shouldBeEnabled;
		habitat.tiles[1]->enabled = shouldBeEnabled;
		habitat.tiles[2]->enabled = shouldBeEnabled;
	}
}

class ScrapbookAnimation : public Coroutine::Function<char>
{
	void operator()(Coroutine::Package& package)
	{
		bool finalising = false;
		if (HexScrapbook::habitats[0].habitat->enabled)
		{
			finalising = false;
		}
		switch (HexScrapbook::state)
		{
		case HexScrapbook::State::Opening:
			HexScrapbook::openness += HexScrapbook::animationSpeed * Time::delta;
			finalising = HexScrapbook::openness >= 1.0f;
			if (finalising)
			{
				HexScrapbook::openness = 1.0f;
				HexScrapbook::SetState(HexScrapbook::State::Open);
			}
			break;
		case HexScrapbook::State::Closing:
			HexScrapbook::openness -= HexScrapbook::animationSpeed * Time::delta;
			finalising = HexScrapbook::openness <= 0.0f;
			if (finalising)
			{
				HexScrapbook::openness = 0.0f;
				HexScrapbook::SetState(HexScrapbook::State::Closed);
			}
			break;
		default:
			CoroutineFinalise();
		}

		float easedValue = Easing::QuadInOut(HexScrapbook::openness);
		float newY = std::lerp(-2.0f, 0.0f, easedValue);
		HexScrapbook::base->anchor.y = newY;
		for (auto& habitat : HexScrapbook::habitats)
		{
			habitat.habitat->anchor.y = newY;
			for (UISprite* tile : habitat.tiles)
			{
				tile->anchor.y = newY;
			}
		}

		if (finalising) { CoroutineFinalise(); }
		else { CoroutineYield(); }
	}
};
const Coroutine::Pair* HexScrapbook::Open() noexcept
{
	switch (state)
	{
	case State::Closed:
		SetState(State::Opening);
		return Coroutine::Start<ScrapbookAnimation>((char*)nullptr, false);
	case State::Opening:
		return nullptr;
	case State::Open:
		SetState(State::Open);
		return nullptr;
	case State::Closing:
		SetState(State::Opening);
		return nullptr;
	default:
		return nullptr;
	}
}
const Coroutine::Pair* HexScrapbook::Close() noexcept
{
	switch (state)
	{
	case State::Closed:
		SetState(State::Closed);
		return nullptr;
	case State::Opening:
		SetState(State::Closing);
		return nullptr;
	case State::Open:
		SetState(State::Closing);
		return Coroutine::Start<ScrapbookAnimation>((char*)nullptr, false);
	case State::Closing:
		return nullptr;
	default:
		return nullptr;
	}
}

void HexScrapbook::Reset() noexcept
{
	state = State::Closed;
	openness = 0.0f;

	float newY = 0.0f;
	base->anchor.y = newY;
	for (auto& habitat : habitats)
	{
		habitat.habitat->anchor.y = newY;
		for (UISprite* tile : habitat.tiles)
		{
			tile->anchor.y = newY;
		}
	}
}

HexScrapbook::State HexScrapbook::GetState() noexcept
{
	return state;
}

struct EnabledStatusCache
{
	bool base;
	std::vector<std::array<bool, 4>> habitatsCache;
};

bool baseEnabledCache;
std::vector<std::tuple<bool, bool, bool, bool>> habitatsEnabledCache;
void HexScrapbook::CacheEnabledStatus() noexcept
{
	baseEnabledCache = base->enabled;

	habitatsEnabledCache.clear();
	for (const HabitatCollection& habitat : habitats)
	{
		habitatsEnabledCache.push_back( {
			habitat.habitat->enabled,
			habitat.tiles[0]->enabled,
			habitat.tiles[1]->enabled,
			habitat.tiles[2]->enabled
		} );
	}
}
void HexScrapbook::RestoreEnabledStatus() noexcept
{
	base->enabled = baseEnabledCache;
	for (int i = 0; i < (int)habitats.size(); i++)
	{
		std::tie(
			habitats[i].habitat->enabled,
			habitats[i].tiles[0]->enabled,
			habitats[i].tiles[1]->enabled,
			habitats[i].tiles[2]->enabled
		) = habitatsEnabledCache[i];
	}
}

void HexScrapbook::ConcealSprites() noexcept
{
	for (HabitatCollection& habitat : habitats)
	{
		habitat.habitat->Load(habitat.habitatTextures[0].c_str());
		habitat.tiles[0]->Load(habitat.tileTextures[0].c_str());
		habitat.tiles[1]->Load(habitat.tileTextures[2].c_str());
		habitat.tiles[2]->Load(habitat.tileTextures[4].c_str());
	}
	HexScrapbook::habitats[0].habitat->enabled = HexScrapbook::habitats[0].habitat->enabled;
}

void HexScrapbook::RevealSprites() noexcept
{
	for (HabitatCollection& habitat : habitats)
	{
		habitat.habitat->Load(habitat.habitatTextures[1].c_str());
		habitat.tiles[0]->Load(habitat.tileTextures[1].c_str());
		habitat.tiles[1]->Load(habitat.tileTextures[3].c_str());
		habitat.tiles[2]->Load(habitat.tileTextures[5].c_str());
	}
}

void HexScrapbook::RevealHabitat(int index) noexcept
{
	HabitatCollection& habitat = habitats[index];

	habitat.habitat->Load(habitat.habitatTextures[1].c_str());
	habitat.tiles[0]->Load(habitat.tileTextures[1].c_str());
	habitat.tiles[1]->Load(habitat.tileTextures[3].c_str());
	habitat.tiles[2]->Load(habitat.tileTextures[5].c_str());
}
void HexScrapbook::RevealHabitatTile(int habitatIndex, int tileIndex) noexcept
{
	HabitatCollection& habitat = habitats[habitatIndex];

	habitat.tiles[tileIndex]->Load(habitat.tileTextures[tileIndex * 2 + 1].c_str());
}
