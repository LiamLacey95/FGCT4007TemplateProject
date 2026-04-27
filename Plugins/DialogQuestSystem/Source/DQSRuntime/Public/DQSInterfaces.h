#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "DQSInterfaces.generated.h"

class UDialogueGraphAsset;

UINTERFACE(BlueprintType)
class DQSRUNTIME_API UDQSDialogueProviderInterface : public UInterface
{
	GENERATED_BODY()
};

class DQSRUNTIME_API IDQSDialogueProviderInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialog Quest System")
	UDialogueGraphAsset* GetDialogueAsset() const;
};

UINTERFACE(BlueprintType)
class DQSRUNTIME_API UDQSEventSourceInterface : public UInterface
{
	GENERATED_BODY()
};

class DQSRUNTIME_API IDQSEventSourceInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialog Quest System")
	FGameplayTag GetDialogQuestEventTag() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialog Quest System")
	int32 GetDialogQuestEventDelta() const;
};

UINTERFACE(BlueprintType)
class DQSRUNTIME_API UDQSDialogueEventReceiverInterface : public UInterface
{
	GENERATED_BODY()
};

class DQSRUNTIME_API IDQSDialogueEventReceiverInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialog Quest System")
	void ReceiveDQSDialogueEvent(FGameplayTag EventTag, FName EventName, UObject* Context);
};
