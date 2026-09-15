#include "TogglePauseCommand.h"

#include "AppStateManager.h"
#include "PauseMenuState.h"
#include "MemoryTracker.h"
#include "Audio.h"

void TogglePauseCommand::Execute()
{
	if ( !AppStateManager::GetInstance().IsCurrentStatePause() )
	{
		Audio::PauseAll();
		AppStateManager::GetInstance().SchedulePushState(ENG_NEW(PauseMenuState));
	}
	else
	{
		Audio::StopTag("Pause");
		Audio::ResumeAll();
		AppStateManager::GetInstance().SchedulePop();
	}
}

CommandType TogglePauseCommand::GetType() const { return CommandType::Menu; }
