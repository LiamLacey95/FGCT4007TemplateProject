#include "DQSDialogueViewModel.h"

#include "DialogQuestSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

bool UDQSDialogueViewModel::InitializeFromWorldContext(UObject* WorldContextObject)
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

	BoundSubsystem->OnDialogueUpdated.RemoveAll(this);
	BoundSubsystem->OnDialogueEnded.RemoveAll(this);
	BoundSubsystem->OnDialogueUpdated.AddDynamic(this, &UDQSDialogueViewModel::HandleDialogueUpdated);
	BoundSubsystem->OnDialogueEnded.AddDynamic(this, &UDQSDialogueViewModel::HandleDialogueEnded);

	bDialogueActive = BoundSubsystem->IsDialogueActive();
	CurrentLine = BoundSubsystem->GetCurrentDialogueLine();
	OnStateChanged.Broadcast();
	return true;
}

void UDQSDialogueViewModel::HandleDialogueUpdated(const FDQSActiveDialogueLine& ActiveLine)
{
	bDialogueActive = true;
	CurrentLine = ActiveLine;
	OnStateChanged.Broadcast();
}

void UDQSDialogueViewModel::HandleDialogueEnded()
{
	bDialogueActive = false;
	CurrentLine = FDQSActiveDialogueLine{};
	OnStateChanged.Broadcast();
}
