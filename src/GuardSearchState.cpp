#include <SDL3/SDL.h>

#include "GuardSearchState.h"

#include "Guard.h"
#include "GuardMoveToLastTileStrategy.h"
#include "GuardFollowDirectionStrategy.h"
#include "GuardRandomSearchStrategy.h"
#include "Configuration.h"
#include "GuardStateManager.h"
#include "GuardChaseState.h"
#include "GuardSearchStrategyProgressTracker.h"
#include "Miscs.h"
#include "MemoryTracker.h"
#include "Unused.h"

GuardSearchState::GuardSearchState(int lastSeenX, int lastSeenY, Direction lastSeenDir, float aggressionLevel)
{
	int additionalTiles = 0;
	if (aggressionLevel > 0.0f && aggressionLevel < 0.5f)
	{
		additionalTiles = 3;
	}
	else
	{
		additionalTiles = 5;
	}

	const GameConfiguration& config = Configuration::Get();
	const int searchTiles = config.guardSearch.followTiles;
	const float searchTime = config.guardSearch.randomSearchTime;
	const float seearchTimeAggressionMultiplier = config.guardSearch.aggressionRandomTimeMultiplier;

	strategies.push_back(ENG_NEW(GuardMoveToLastTileStrategy, lastSeenX, lastSeenY));
	strategies.push_back(ENG_NEW(GuardFollowDirectionStrategy, lastSeenDir, searchTiles + additionalTiles));
	strategies.push_back(ENG_NEW(GuardRandomSearchStrategy, searchTime + aggressionLevel * seearchTimeAggressionMultiplier));
}

GuardSearchState::GuardSearchState(const SDL_Point& lastSeenPoint, Direction lastSeenDir, float aggressionLevel) :
	GuardSearchState(lastSeenPoint.x, lastSeenPoint.y, lastSeenDir, aggressionLevel) {}

GuardSearchState::GuardSearchState(GuardSearchState&& other) noexcept :
	GuardState(std::move(other)),
	currentIndex(other.currentIndex),
	strategies(std::move(other.strategies)) {}

GuardSearchState::~GuardSearchState()
{
	for (GuardStrategy*& strategy : strategies)
	{
		if (strategy)
		{
			ENG_DELETE(strategy);
			strategy = nullptr;
		}
	}
	strategies.clear();
}

void GuardSearchState::Enter(Guard& guard)
{
	int extraTiles = 0;
	float extraAngle = 0.0f;
	if (guard.GetAggressionLevel() > 0.0f && guard.GetAggressionLevel() < 0.5f) 
	{
		extraTiles = 1;
		extraAngle = 10.0f;
	}
	else 
	{
		extraTiles = 2;
		extraAngle = 20.0f;
	}

	const GameConfiguration& config = Configuration::Get();
	const VisionRangeModifierConfiguration& searchMod = config.guard.vision.search;

	VisionRange visionRange = config.guard.MakeVisionRange(searchMod);
	visionRange += extraTiles;
	visionRange += extraAngle;

	guard.SetVisionRange(visionRange);
}

void GuardSearchState::Update(double deltaTime, Guard& guard)
{
	if (guard.CanSeePlayer())
	{
		GuardStateManager::SetState(guard, &GuardChaseState::GetInstance());
		return;
	}

	if (currentIndex >= strategies.size())
	{
		GuardStateManager::ChangeToDefaultState(guard);
		return;
	}

	GuardStrategy*& activeStrategy = strategies[currentIndex];
	guard.SetDirectionProvider(MakeDirectionProviderFrom(activeStrategy, guard));

	// Trigger internal updates.
	activeStrategy->GetDirection(guard);

	while (currentIndex < strategies.size() && IsStrategyDone(strategies[currentIndex]))
	{
		if (dynamic_cast<GuardMoveToLastTileStrategy*>(strategies[currentIndex]))
		{
			guard.ShowSearchAlertIcon();
		}
		currentIndex++;
	}

	activeStrategy->Update(deltaTime);
}

void GuardSearchState::Exit(Guard& guard)
{
	guard.StartVisionDecrease();
	guard.StartAggressionDecrease();
}

bool GuardSearchState::IsShared() const { return false; }

GuardStateType GuardSearchState::GetStateType() const { return GuardStateType::Alert; }

GuardSearchState& GuardSearchState::operator=(GuardSearchState&& other) noexcept
{
	if (this == &other)
	{
		return *this;
	}
	
	for (GuardStrategy*& strategy : strategies)
	{
		if (strategy)
		{
			ENG_DELETE(strategy);
			strategy = nullptr;
		}
	}
	strategies.clear();

	(*(GuardState*)this) = std::move(other);

	currentIndex = other.currentIndex;
	strategies = std::move(other.strategies);

	return *this;
}

bool GuardSearchState::IsStrategyDone(GuardStrategy* strategy)
{
	auto strategyTracker = dynamic_cast <GuardSearchStrategyProgressTracker *> (strategy);
	if (strategyTracker)
	{
		return strategyTracker->IsSearchStrategyDone();
	}
	else
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "GuardSearchState.IsStrategyDone: Given strategy is not a search strategy or does not inherit the search stracker.");
		return true;
	}
}

