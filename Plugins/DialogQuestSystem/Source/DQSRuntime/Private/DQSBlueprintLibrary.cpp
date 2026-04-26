#include "DQSBlueprintLibrary.h"

#include "DialogQuestSubsystem.h"
#include "DQSInterfaces.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

UDialogQuestSubsystem* UDQSBlueprintLibrary::GetDialogQuestSubsystem(UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<UDialogQuestSubsystem>() : nullptr;
}

bool UDQSBlueprintLibrary::StartDialogueFromProvider(UObject* WorldContextObject, UObject* DialogueProvider, UObject* Context)
{
	UDialogQuestSubsystem* Subsystem = GetDialogQuestSubsystem(WorldContextObject);
	UDialogueGraphAsset* DialogueAsset = ResolveDialogueFromProvider(DialogueProvider);
	return Subsystem && DialogueAsset ? Subsystem->StartDialogue(DialogueAsset, Context ? Context : DialogueProvider) : false;
}

UDialogueGraphAsset* UDQSBlueprintLibrary::ResolveDialogueFromProvider(UObject* DialogueProvider)
{
	if (!DialogueProvider || !DialogueProvider->GetClass()->ImplementsInterface(UDQSDialogueProviderInterface::StaticClass()))
	{
		return nullptr;
	}

	return IDQSDialogueProviderInterface::Execute_GetDialogueAsset(DialogueProvider);
}
