#include <optional>
#include <random>
#include <array>

#include "GuardRandomSearchStrategy.h"
#include "FloatUtils.h"
#include "Unused.h"

GuardRandomSearchStrategy::GuardRandomSearchStrategy(float searchTime, float dirChangeCooldown) :
    searchDuration(searchTime),
	changeCooldown(dirChangeCooldown),
    currentDirection(GetRandomDirection()) 
{
    if (FloatUtils::IsZeroF(changeCooldown))
    {
        changeCooldown = 30.0f;
    }
}

Direction GuardRandomSearchStrategy::GetDirection(Guard& guard)
{
    UNUSED(guard);

    if (IsSearchStrategyDone()) 
    {
		return GetSentinel<Direction>();
    }

    return currentDirection;
}

void GuardRandomSearchStrategy::Update(double deltaTime)
{
    searchTimer += static_cast<float>(deltaTime);

    if (FloatUtils::IsEqualF(searchTimer, changeCooldown))
    {
        currentDirection = GetRandomDirection();
    }
}

bool GuardRandomSearchStrategy::IsSearchStrategyDone() const { return FloatUtils::IsGreaterOrEqualF(searchTimer, searchDuration); }

bool GuardRandomSearchStrategy::IsDirectionOpposite(Direction first, Direction second) const
{
    return (first == Direction::Up && second == Direction::Down) ||
        (first == Direction::Down && second == Direction::Up) ||
        (first == Direction::Left && second == Direction::Right) ||
        (first == Direction::Right && second == Direction::Left);
}

Direction GuardRandomSearchStrategy::GetRandomDirection() const
{
    static std::minstd_rand gen{ std::random_device{}() };
    static std::uniform_int_distribution<int> dirDist(0, 3);

    std::array<Direction, 4> directions = {
        Direction::Up,
        Direction::Down,
        Direction::Left,
        Direction::Right
    };

    Direction newDir;
    do 
    {
        newDir = directions[dirDist(gen)];
    } while (IsDirectionOpposite(currentDirection, newDir));

    return newDir;
}
