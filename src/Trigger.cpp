#include <SDL3/SDL_log.h>
#include <utility>

#include "Trigger.h"
#include "ActivationExpression.h"
#include "GameTime.h"
#include "MemoryTracker.h"
#include "SystemActionRegistry.h"

using json = nlohmann::json;

Trigger::Trigger(const std::string& name, int xPosition, int yPosition, std::vector<SystemAction> eventsVec, ActivationExpression* activationExpression, TriggerFlags triggerFlags, TriggerOrigin origin) :
    position({ xPosition, yPosition }),
    events(eventsVec),
    activationExpression(activationExpression),
    flags(triggerFlags),
    triggerName(name),
    triggerId(FNV1aHash::HashString(triggerName))
{
    this->triggerState.origin = origin;
	this->triggerState.createdTime = GameTime::PeekPlayTimeMiliseconds();
}

Trigger::Trigger(const std::string& name, const SDL_Point& tilePosition, std::vector<SystemAction> eventsVec, ActivationExpression* activationExpression, TriggerFlags triggerFlags, TriggerOrigin origin) :
    Trigger(name, tilePosition.x, tilePosition.y, eventsVec, activationExpression, triggerFlags, origin) {}

Trigger::Trigger(Trigger&& other) noexcept :
    position(other.position),
    events(std::exchange(other.events, {})),
    activationExpression(std::exchange(other.activationExpression, nullptr)),
    flags(other.flags),
    lastProcessedFrame(other.lastProcessedFrame),
    triggerState(other.triggerState),
    triggerId(other.triggerId),
    triggerName(other.triggerName) {}

Trigger::~Trigger()
{
    if (activationExpression)
    {
        ENG_DELETE(activationExpression);
        activationExpression = nullptr;
    }
}

bool Trigger::TryFire(Player* player, MapUpdateFlags updateMask)
{
    if (events.empty())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Trigger.Fire: Events vector is empty. No reason to fire. Defaulting to FALSE.");
        return false;
    }

    if (!activationExpression)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Trigger.Fire: Attempted to fire a null activation expression. Defaulting to FALSE.");
        return false;
    }

    if (IsTriggered() && !IsRepeatable())
    {
        if (!HasFlag(flags, TriggerFlags::AutoDelete))
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Trigger.Fire: Trigger has been activated, but is not auto-deleted or repeatable. This could be intentional.");
        }

        return false;
    }

    if (!activationExpression->Evaluate(this, player, updateMask))
    {
        return false;
    }

    triggerState.triggeredFlag = true;
    ++triggerState.timesActivated;
    triggerState.lastActivatedTime = GameTime::PeekPlayTimeMiliseconds();

    TriggerEvents();

    return true;
}

void Trigger::SetTriggerFlags(TriggerFlags triggerFlags, bool enable)
{
    if (enable)
    {
        flags |= triggerFlags;
    }
    else
    {
        flags &= ~triggerFlags;
    }
}

TriggerFlags Trigger::GetTriggerFlags() const { return flags; }

MapUpdateFlags Trigger::GetUpdateFlags() const
{
    if (!activationExpression)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Trigger.Fire: Attempted to get update flags of a null activation expression. Defaulting to NONE");
        return MapUpdateFlags::None;
    }

    return activationExpression->GetUpdateFlags();
}

void Trigger::SetTriggeredState(bool triggered) { triggerState.triggeredFlag = triggered; }

HashID Trigger::GetTriggerId() const { return triggerId; }

std::string Trigger::GetTriggerName() { return triggerName; }

bool Trigger::ShouldAutoDelete() const { return IsTriggered() && HasFlag(flags, TriggerFlags::AutoDelete); }

const std::vector<SystemAction>& Trigger::GetEvents() const { return events; }

void Trigger::SetEvents(const std::vector<SystemAction>& newEvents) { events = newEvents; }

void Trigger::AddEvent(const SystemAction& event) { events.push_back(event); }

ActivationExpression* Trigger::GetActivatorExpression() const { return activationExpression; }

void Trigger::SetActivatorExpression(ActivationExpression* newExpression)
{
    if (activationExpression)
    {
        ENG_DELETE(activationExpression);
    }

    activationExpression = newExpression;
    newExpression = nullptr;
}

SDL_Point Trigger::GetPosition() const { return position; }

void Trigger::SetPosition(int x, int y) { position = SDL_Point { x, y }; }

void Trigger::SetPosition(const SDL_Point& newPosition) { position = newPosition; }

void Trigger::SetLastProcessedFrame(uint64_t newFrame) { lastProcessedFrame = newFrame; }

uint64_t Trigger::GetLastProcessedFrame() const { return lastProcessedFrame; }

SpatialBucket Trigger::GetSpatialBucket() const
{
    if (!activationExpression)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Trigger.Fire: Attempted to get spatial radius of a null activation expression. Defaulting to TILE ONLY");
        return SpatialBucket::TileOnly;
    }

    return activationExpression->GetSpatialBucket();
}

const nlohmann::json& Trigger::GetTemplateJSON() const { return templateJSON; }

void Trigger::SetTemplateJSON(const nlohmann::json& newTemplate) { templateJSON = newTemplate; }

void Trigger::SetTriggerOrigin(TriggerOrigin newOrigin) { triggerState.origin = newOrigin; }

uint64_t Trigger::GetLastActivatedTime() const { return triggerState.lastActivatedTime; }

uint64_t Trigger::GetCreatedTime() const { return triggerState.createdTime; }

int Trigger::GetActivationCount() const { return triggerState.timesActivated; }

json Trigger::SerializeRuntimeState() const
{
    json j;

    j["triggered"] = IsTriggered();
    j["timesFired"] = triggerState.timesActivated;
    j["lastActivation"] = triggerState.lastActivatedTime;
	j["createdTime"] = triggerState.createdTime;

    return j;
}

void Trigger::ApplyRuntimeState(const nlohmann::json& runtimeState)
{
    /*
        What this function expects: 

        Note that 'runtimeState' is the default JSON object.

        {
            "triggered": true,
            "timesFired": 2,
            "lastActivation": 132,
			"createdTime": 120
        }
    */

    if (!runtimeState.is_object())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Trigger.ApplyRuntimeState: Expected object, got non-object.");
        return;
    }

    if (!runtimeState.contains("triggered") || !runtimeState["triggered"].is_boolean())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Trigger.ApplyRuntimeState: Trigger 'triggered' flag is missing or not a boolean. Defaulting to 'false'");
    }

    triggerState.triggeredFlag = runtimeState.value("triggered", false);

    if (!runtimeState.contains("timesFired") || !runtimeState["timesFired"].is_number_unsigned())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Trigger.ApplyRuntimeState: Trigger 'timesFired' is missing or not a unsigned. Defaulting to 0");
    }

    triggerState.timesActivated = runtimeState.value("timesFired", 0u);

    if (!runtimeState.contains("lastActivation") || !runtimeState["lastActivation"].is_number_unsigned())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Trigger.ApplyRuntimeState: Trigger 'lastActivation' is missing or not a unsigned. Defaulting to 0");
    }

    triggerState.lastActivatedTime = runtimeState.value("lastActivation", 0u);

    if(!runtimeState.contains("createdTime") || !runtimeState["createdTime"].is_number_unsigned())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Trigger.ApplyRuntimeState: Trigger 'createdTime' is missing or not a unsigned. Defaulting to current time.");
	}

	triggerState.createdTime = runtimeState.value("createdTime", GameTime::PeekPlayTimeMiliseconds());

    // 'origin' is not decided here, instead, if the save data has 'template', then it is a dynamic trigger, otherwise, static.
    // There's also the 'isRuntimeFlag' in the trigger save data.
}

bool Trigger::IsTriggered() const { return triggerState.triggeredFlag; }

bool Trigger::IsRepeatable() const { return HasFlag(flags, TriggerFlags::Repeatable); }

bool Trigger::IsRuntime() const { return triggerState.origin == TriggerOrigin::Runtime; }

Trigger& Trigger::operator=(Trigger&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    if (activationExpression)
    {
        ENG_DELETE(activationExpression);
    }

    position = other.position;
    events = std::exchange(other.events, {});
    activationExpression = std::exchange(other.activationExpression, nullptr);
    flags = other.flags;
    lastProcessedFrame = other.lastProcessedFrame;
    triggerState = other.triggerState;
    triggerId = other.triggerId;
    triggerName = other.triggerName;

    return *this;
}

void Trigger::TriggerEvents()
{
    if (events.empty())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Trigger.TriggerEvents: Events vector is empty.");
        return;
    }

    for (const SystemAction& action : events)
    {
        SystemActionRegistry::GetInstance().Execute(action);
    }
}