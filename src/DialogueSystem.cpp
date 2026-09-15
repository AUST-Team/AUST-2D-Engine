#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "DialogueSystem.h"

#include "AppRenderSystem.h"
#include "Configuration.h"
#include "FontManager.h"
#include "DialogueNarrationState.h"
#include "DialogueChoiceState.h"
#include "MemoryTracker.h"
#include "UIFactory.h"

DialogueSystem::DialogueSystem()
{
	ui = UIFactory::CreateDialogue();

    const GameConfiguration& config = Configuration::Get();

    int newWidth = AppRenderSystem::GetInstance().GetWidth();
    int newHeight = AppRenderSystem::GetInstance().GetHeight();
    int baseWidth = config.window.width;
    int baseHeight = config.window.height;

    if (newHeight != baseWidth || newWidth != baseHeight)
    {
        ResizeUI(newWidth, newHeight, baseWidth, baseHeight);
    }
}

DialogueSystem::~DialogueSystem()
{
    if(ui)
    {
        ENG_DELETE(ui);
        ui = nullptr;
	}
}

void DialogueSystem::HandleEvent(const SDL_Event& e) 
{
    if (ui && isActiveFlag)
    {
        ui->HandleEvent(e);
    }
}

void DialogueSystem::Update(double deltaTime)
{
    if (ui && isActiveFlag)
    {
        ui->Update(deltaTime);
    }
}

void DialogueSystem::Render(SDL_Renderer* renderer) const
{
    if (ui && isActiveFlag)
    {
        ui->Render(renderer);
    }
}

void DialogueSystem::HandleCommand(const Command* command)
{
    if (ui && isActiveFlag)
    {
        ui->HandleCommand(command);
    }
}

void DialogueSystem::StartDialogue(const std::vector<std::string>& newLines, std::function<void()> callback)
{
    if (!ui)
    {
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueSystem.StartDialogue: UI is not initialized.\n");
		return;
    }

    isActiveFlag = true;
    DialogueNarrationState* narrationState = ENG_NEW(DialogueNarrationState, 
        *ui, 
        newLines, 
        [this, callback]() {
            if (callback)
            {
                callback();
            }

            if (!ui->HasPendingState())
            {
                isActiveFlag = false;
            }
        }
    );
    ui->ScheduleStateChange(narrationState);
}

void DialogueSystem::StartChoice(const std::vector<std::string>& choices, const std::vector<std::function<void()>>& callbacks, const std::string& question, const std::function<void()>& onEndCallback)
{
    if (!ui)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueSystem.StartChoice: UI is not initialized.\n");
        return;
    }

    isActiveFlag = true;

    std::vector<std::function<void()>> choiceCallbacks;
    choiceCallbacks.reserve(callbacks.size());

    for (const std::function<void()>& callback : callbacks)
    {
        choiceCallbacks.push_back([this, cb = callback]() {
            if (cb)
            {
                cb();
            }

            if (!ui->HasPendingState())
            {
                isActiveFlag = false;
            }
        });
    }

    const std::function<void()>& onDialogueEndCallback = [this, onEndCallback]() {
        if (onEndCallback)
        {
            onEndCallback();
        }

        if (!ui->HasPendingState())
        {
            isActiveFlag = false;
        }
	};

    DialogueChoiceState* choiceState = ENG_NEW(DialogueChoiceState, 
        *ui,
        question,
        choices,
        choiceCallbacks,
        onDialogueEndCallback
    );

    ui->ScheduleStateChange(choiceState);
}

void DialogueSystem::EndDialogue()
{ 
    if(!ui)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueSystem.EndDialogue: UI is not initialized.\n");
        return;
	}

    isActiveFlag = false;
    ui->ScheduleStateChange(nullptr);
}

void DialogueSystem::ResizeUI(int newWidth, int newHeight, int oldWidth, int oldHeight)
{
    if (!ui)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueSystem.ResizeUI: UI is not initialized.\n");
		return;
    }

    const float widthScale = static_cast<float> (newWidth) / oldWidth;
    const float heightScale = static_cast<float> (newHeight) / oldHeight;

    ui->Resize(widthScale, heightScale);
    return;
}

void DialogueSystem::SetOnDialogueEnd(std::function<void()> callback) 
{
    if(!ui)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DialogueSystem.SetOnDialogueEnd: UI is not initialized.\n");
        return;
	}

    ui->SetOnDialogueEnd([this, callback]() {
        isActiveFlag = false;
        if (callback)
        {
            callback();
        }
    });
}

void DialogueSystem::SetActive(bool newValue) { isActiveFlag = newValue; }

bool DialogueSystem::IsActive() const { return isActiveFlag; }
