#pragma once

#include "CoreMinimal.h"
#include "DQSTypes.h"
#include "UObject/Object.h"
#include "DQSQuestViewModel.generated.h"

class UDialogQuestSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDQSQuestViewModelChangedSignature);

UCLASS(BlueprintType)
class DQSUI_API UDQSQuestViewModel : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	bool InitializeFromWorldContext(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System")
	const FDQSQuestStatusUpdate& GetLatestUpdate() const { return LatestUpdate; }

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System")
	const TMap<FName, FDQSQuestRuntimeState>& GetQuestStates() const { return CachedQuestStates; }

	UPROPERTY(BlueprintAssignable, Category = "Dialog Quest System")
	FDQSQuestViewModelChangedSignature OnStateChanged;

protected:
	UFUNCTION()
	void HandleQuestUpdated(const FDQSQuestStatusUpdate& Update);

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	FDQSQuestStatusUpdate LatestUpdate;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	TMap<FName, FDQSQuestRuntimeState> CachedQuestStates;

	UPROPERTY(Transient)
	TObjectPtr<UDialogQuestSubsystem> BoundSubsystem;
};
