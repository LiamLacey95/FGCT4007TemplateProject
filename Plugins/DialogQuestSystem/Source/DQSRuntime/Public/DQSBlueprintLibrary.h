#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "DQSTypes.h"
#include "DQSBlueprintLibrary.generated.h"

class UDialogQuestSubsystem;
class UDialogueGraphAsset;

UCLASS()
class DQSRUNTIME_API UDQSBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Dialog Quest System", meta = (WorldContext = "WorldContextObject"))
	static UDialogQuestSubsystem* GetDialogQuestSubsystem(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System", meta = (WorldContext = "WorldContextObject"))
	static bool StartDialogueFromProvider(UObject* WorldContextObject, UObject* DialogueProvider, UObject* Context);

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System")
	static UDialogueGraphAsset* ResolveDialogueFromProvider(UObject* DialogueProvider);
};
