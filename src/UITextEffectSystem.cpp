#include <SDL3/SDL.h>

#include "UITextEffectSystem.h"

#include "TypewriterEffect.h"
#include "MemoryTracker.h"

UITextEffectSystem::~UITextEffectSystem() { Clear(); }

void UITextEffectSystem::AddEffect(const std::string& id, const std::string& text, float delay)
{
    auto it = effects.find(id);
    if (it != effects.end())
    {
        ENG_DELETE(it->second.effect);
    }

    TypewriterEffect* tw = ENG_NEW(TypewriterEffect, text, delay);
    effects.emplace(id, TypewriterEntry{ .effect = tw, .destroy = false });
}

void UITextEffectSystem::Update(double deltaTime)
{
    for (auto it = effects.begin(); it != effects.end();)
    {
        it->second.effect->Update(deltaTime);

        // Uses a clock-algorithm-like destroy.
        if (it->second.effect->IsFinished() && !it->second.destroy)
        {
            it->second.destroy = true;
            ++it;
        }
        else if (it->second.destroy)
        {
            ENG_DELETE(it->second.effect);
            it->second.effect = nullptr;
            it = effects.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void UITextEffectSystem::ForceFinish(const std::string& id)
{
    auto it = effects.find(id);
    if (it != effects.end() && it->second.effect)
    {
        it->second.effect->ForceFinish();
    }
}

void UITextEffectSystem::ForceFinishAll()
{
    for (auto& it : effects)
    {
        if (it.second.effect)
        {
            it.second.effect->ForceFinish();
        }
    }
}

void UITextEffectSystem::Clear()
{
    for (auto& [_, entry] : effects)
    {
        if (entry.effect)
        {
            ENG_DELETE(entry.effect);
            entry.effect = nullptr;
        }
    }
    effects.clear();
}

std::string UITextEffectSystem::GetText(const std::string& id) const
{
    auto it = effects.find(id);

    if (it != effects.end())
    {
        return it->second.effect->GetText();
    }

    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "UITextEffectSystem.GetText: Failed to locate effect for id %s.", id.c_str());
    return "";
}

bool UITextEffectSystem::HasEffect(const std::string& id) const { return effects.find(id) != effects.end(); }

bool UITextEffectSystem::IsFinished(const std::string& id) const
{
    auto it = effects.find(id);
    return (it != effects.end()) && it->second.effect->IsFinished();
}