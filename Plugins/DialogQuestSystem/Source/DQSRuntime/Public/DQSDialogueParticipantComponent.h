#pragma once

#include "Components/ActorComponent.h"
#include "DQSDialogueParticipantComponent.generated.h"

class UDialogueGraphAsset;

UCLASS(ClassGroup = (DialogQuestSystem), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class DQSRUNTIME_API UDQSDialogueParticipantComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDQSDialogueParticipantComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	TObjectPtr<UDialogueGraphAsset> DialogueAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	bool bUseOwnerAsContext = true;

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	bool StartDialogue(UObject* WorldContextObject, UObject* OverrideContext);

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System")
	UObject* ResolveContext(UObject* OverrideContext) const;
};
