#pragma once

#include "CoreMinimal.h"
#include "DQSTypes.h"
#include "UObject/Object.h"
#include "DQSDialogueViewModel.generated.h"

class UDialogQuestSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDQSDialogueViewModelChangedSignature);

UCLASS(BlueprintType)
class DQSUI_API UDQSDialogueViewModel : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	bool InitializeFromWorldContext(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System")
	bool IsDialogueActive() const { return bDialogueActive; }

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System")
	const FDQSActiveDialogueLine& GetCurrentLine() const { return CurrentLine; }

	UPROPERTY(BlueprintAssignable, Category = "Dialog Quest System")
	FDQSDialogueViewModelChangedSignature OnStateChanged;

protected:
	UFUNCTION()
	void HandleDialogueUpdated(const FDQSActiveDialogueLine& ActiveLine);

	UFUNCTION()
	void HandleDialogueEnded();

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	bool bDialogueActive = false;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	FDQSActiveDialogueLine CurrentLine;

	UPROPERTY(Transient)
	TObjectPtr<UDialogQuestSubsystem> BoundSubsystem;
};
