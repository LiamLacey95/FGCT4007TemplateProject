#include "DQSDialogueParticipantComponent.h"

#include "DQSBlueprintLibrary.h"
#include "DialogQuestSubsystem.h"
#include "GameFramework/Actor.h"

DEFINE_LOG_CATEGORY_STATIC(LogDQSDialogueParticipant, Log, All);

UDQSDialogueParticipantComponent::UDQSDialogueParticipantComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UDQSDialogueParticipantComponent::StartDialogue(UObject* WorldContextObject, UObject* OverrideContext)
{
	if (!DialogueAsset)
	{
		UE_LOG(LogDQSDialogueParticipant, Warning, TEXT("StartDialogue failed on '%s': Dialogue Asset is not set on the DQSDialogueParticipantComponent."), *GetNameSafe(GetOwner()));
		return false;
	}

	UObject* EffectiveWorldContext = WorldContextObject ? WorldContextObject : Cast<UObject>(GetOwner());
	if (UDialogQuestSubsystem* Subsystem = UDQSBlueprintLibrary::GetDialogQuestSubsystem(EffectiveWorldContext))
	{
		return Subsystem->StartDialogue(DialogueAsset, ResolveContext(OverrideContext));
	}

	UE_LOG(LogDQSDialogueParticipant, Warning, TEXT("StartDialogue failed on '%s': could not find DialogQuestSubsystem from world context '%s'."), *GetNameSafe(GetOwner()), *GetNameSafe(EffectiveWorldContext));
	return false;
}

UObject* UDQSDialogueParticipantComponent::ResolveContext(UObject* OverrideContext) const
{
	if (OverrideContext)
	{
		return OverrideContext;
	}

	return bUseOwnerAsContext ? Cast<UObject>(GetOwner()) : nullptr;
}
