#include "DQSDialogueParticipantComponent.h"

#include "DQSBlueprintLibrary.h"
#include "DialogQuestSubsystem.h"
#include "GameFramework/Actor.h"

UDQSDialogueParticipantComponent::UDQSDialogueParticipantComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UDQSDialogueParticipantComponent::StartDialogue(UObject* WorldContextObject, UObject* OverrideContext)
{
	if (!DialogueAsset)
	{
		return false;
	}

	UObject* EffectiveWorldContext = WorldContextObject ? WorldContextObject : Cast<UObject>(GetOwner());
	if (UDialogQuestSubsystem* Subsystem = UDQSBlueprintLibrary::GetDialogQuestSubsystem(EffectiveWorldContext))
	{
		return Subsystem->StartDialogue(DialogueAsset, ResolveContext(OverrideContext));
	}

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
