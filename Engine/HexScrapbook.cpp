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

	bool shouldBeEnabled = newState != State::Closed;

	base->enabled = shouldBeEnabled;
	for (HabitatCollection& habitat : habitats)
	{
		habitat.habitat->enabled = shouldBeEnabled;
		habitat.tiles[0]->enabled = shouldBeEnabled;
		habitat.tiles[1]->enabled = shouldBeEnabled;
		habitat.tiles[2]->enabled = shouldBeEnabled;
	}
}

struct ScrapbookAnimationData
{
	const HexScrapbook::State& state = HexScrapbook::state;
	float& openness = HexScrapbook::openness;
	UISprite*& base = HexScrapbook::base;
	std::vector<HexScrapbook::HabitatCollection>& habitats = HexScrapbook::habitats;
	const float& speed = HexScrapbook::animationSpeed;
};
ScrapbookAnimationData scrapbookAnimationData;
class ScrapbookAnimation : public Coroutine::Function<ScrapbookAnimationData>
{
	void operator()(Coroutine::Package& package)
	{
		ScrapbookAnimationData& data = GetData(package);

		bool finalising = false;

		switch (data.state)
		{
		case HexScrapbook::State::Opening:
			data.openness += data.speed * Time::delta;
			finalising = data.openness >= 1.0f;
			if (finalising)
			{
				data.openness = 1.0f;
				HexScrapbook::SetState(HexScrapbook::State::Open);
			}
			break;
		case HexScrapbook::State::Closing:
			data.openness -= data.speed * Time::delta;
			finalising = data.openness <= 0.0f;
			if (finalising)
			{
				data.openness = 0.0f;
				HexScrapbook::SetState(HexScrapbook::State::Closed);
			}
			break;
		default:
			CoroutineFinalise();
		}

		float easedValue = Easing::QuadInOut(data.openness);
		float newY = std::lerp(-2.0f, 0.0f, easedValue);
		data.base->anchor.y = newY;
		for (auto& habitat : data.habitats)
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
void HexScrapbook::Open() noexcept
{
	switch (state)
	{
	case State::Closed:
		SetState(State::Opening);
		Coroutine::Start<ScrapbookAnimation>(&scrapbookAnimationData);
		return;
	case State::Opening:
		return;
	case State::Open:
		SetState(State::Open);
		return;
	case State::Closing:
		SetState(State::Opening);
		return;
	}
}
void HexScrapbook::Close() noexcept
{
	switch (state)
	{
	case State::Closed:
		SetState(State::Closed);
		return;
	case State::Opening:
		SetState(State::Closing);
		return;
	case State::Open:
		SetState(State::Closing);
		Coroutine::Start<ScrapbookAnimation>(&scrapbookAnimationData);
		return;
	case State::Closing:
		return;
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
