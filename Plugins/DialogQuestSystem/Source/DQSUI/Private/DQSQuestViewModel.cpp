#include "DQSQuestViewModel.h"

#include "DialogQuestSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

bool UDQSQuestViewModel::InitializeFromWorldContext(UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return false;
	}

	UGameInstance* GameInstance = WorldContextObject->GetWorld() ? WorldContextObject->GetWorld()->GetGameInstance() : nullptr;
	BoundSubsystem = GameInstance ? GameInstance->GetSubsystem<UDialogQuestSubsystem>() : nullptr;
	if (!BoundSubsystem)
	{
		return false;
	}

	BoundSubsystem->OnQuestUpdated.RemoveAll(this);
	BoundSubsystem->OnQuestUpdated.AddDynamic(this, &UDQSQuestViewModel::HandleQuestUpdated);
	CachedQuestStates = BoundSubsystem->GetQuestStates();
	OnStateChanged.Broadcast();
	return true;
}

void UDQSQuestViewModel::HandleQuestUpdated(const FDQSQuestStatusUpdate& Update)
{
	LatestUpdate = Update;
	CachedQuestStates = BoundSubsystem ? BoundSubsystem->GetQuestStates() : TMap<FName, FDQSQuestRuntimeState>{};
	OnStateChanged.Broadcast();
}
