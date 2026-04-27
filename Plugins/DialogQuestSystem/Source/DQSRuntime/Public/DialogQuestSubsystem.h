#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "DQSTypes.h"
#include "DialogQuestSubsystem.generated.h"

class UDialogueGraphAsset;
class UDQSAction;
class UDQSCondition;
class UQuestGraphAsset;
class APlayerController;
struct FInputModeDataBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDQSDialogueLineUpdatedSignature, const FDQSActiveDialogueLine&, ActiveLine);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDQSDialogueEndedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDQSQuestUpdatedSignature, const FDQSQuestStatusUpdate&, Update);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDQSDialogueEventTriggeredSignature, FGameplayTag, EventTag, FName, EventName, UObject*, Context);

UCLASS(BlueprintType)
class DQSRUNTIME_API UDialogQuestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UDialogQuestSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UPROPERTY(BlueprintAssignable, Category = "Dialog Quest System")
	FDQSDialogueLineUpdatedSignature OnDialogueUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Dialog Quest System")
	FDQSDialogueEndedSignature OnDialogueEnded;

	UPROPERTY(BlueprintAssignable, Category = "Dialog Quest System")
	FDQSQuestUpdatedSignature OnQuestUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Dialog Quest System")
	FDQSDialogueEventTriggeredSignature OnDialogueEventTriggered;

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	bool StartDialogue(UDialogueGraphAsset* DialogueAsset, UObject* Context);

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	bool ContinueDialogue();

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	bool ChooseDialogueOption(int32 ChoiceIndex);

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	void EndDialogue();

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	bool StartQuest(UQuestGraphAsset* QuestAsset);

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	bool AdvanceQuestObjective(FName QuestId, FGameplayTag ObjectiveTag, int32 Delta = 1);

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	bool FailQuest(FName QuestId);

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	bool CompleteQuest(FName QuestId);

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	void TriggerDialogueEvent(FGameplayTag EventTag, FName EventName, UObject* Context);

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	void SetVariable(FName Key, FDQSVariableValue Value, EDQSStateScope Scope = EDQSStateScope::Global, FName ScopedOwnerId = NAME_None);

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	bool GetVariable(FName Key, FDQSVariableValue& OutValue, EDQSStateScope Scope = EDQSStateScope::Global, FName ScopedOwnerId = NAME_None) const;

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	void AddTag(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	void RemoveTag(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	bool HasTag(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	bool SaveDialogQuestState(const FString& SlotName = TEXT("DialogQuestSystem"), int32 UserIndex = 0);

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	bool LoadDialogQuestState(const FString& SlotName = TEXT("DialogQuestSystem"), int32 UserIndex = 0);

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System")
	bool IsDialogueActive() const { return ActiveDialogueSession.bIsActive; }

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System")
	FDQSActiveDialogueLine GetCurrentDialogueLine() const { return CurrentDialogueLine; }

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System")
	TArray<FDQSQuestRuntimeState> GetAllQuestStates() const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System")
	bool GetQuestState(FName QuestId, FDQSQuestRuntimeState& OutQuestState) const;

	const TMap<FName, FDQSQuestRuntimeState>& GetQuestStates() const { return QuestStates; }

protected:
	UPROPERTY(Transient)
	TObjectPtr<UDialogueGraphAsset> ActiveDialogueAsset;

	UPROPERTY(Transient)
	TObjectPtr<UObject> ActiveDialogueContext;

	UPROPERTY(VisibleAnywhere, Category = "Dialog Quest System")
	FDQSDialogueSessionState ActiveDialogueSession;

	UPROPERTY(VisibleAnywhere, Category = "Dialog Quest System")
	FDQSActiveDialogueLine CurrentDialogueLine;

	UPROPERTY(VisibleAnywhere, Category = "Dialog Quest System")
	TMap<FName, FDQSQuestRuntimeState> QuestStates;

	UPROPERTY(VisibleAnywhere, Category = "Dialog Quest System")
	TMap<FName, FDQSVariableValue> GlobalVariables;

	UPROPERTY(VisibleAnywhere, Category = "Dialog Quest System")
	FGameplayTagContainer GlobalTags;

private:
	void ApplyDialogueInteractionMode();
	void RestoreDialogueInteractionMode();
	bool RunDialogueNode(const FGuid& NodeId, bool bExecuteSideEffects);
	void PresentDialogueNode(const FDQSDialogueNode& Node);
	bool EvaluateConditions(const TArray<TObjectPtr<UDQSCondition>>& Conditions, UObject* Context) const;
	void ExecuteActions(const TArray<TObjectPtr<UDQSAction>>& Actions, UObject* Context);
	bool ApplyQuestHook(const FDQSQuestHook& QuestHook);
	bool ProcessQuest(FName QuestId, bool bBroadcastUpdates);
	bool UpdateQuestStatus(FName QuestId, EDQSQuestStatus NewStatus, const FText& Message, const FGameplayTag& ObjectiveTag = FGameplayTag(), int32 CurrentCount = 0, int32 TargetCount = 0);
	TMap<FName, FDQSVariableValue>* ResolveMutableVariableStore(EDQSStateScope Scope, FName ScopedOwnerId);
	const TMap<FName, FDQSVariableValue>* ResolveVariableStore(EDQSStateScope Scope, FName ScopedOwnerId) const;
	bool ResolveDialogueTargetNode(const FDQSDialogueNode& Node, bool bMatchedPath, FGuid& OutTargetNodeId) const;
	bool ResolveQuestTargetNode(const FDQSQuestNode& Node, bool bMatchedPath, FGuid& OutTargetNodeId) const;
	void ResetRuntimeState();
	FDQSSaveSnapshot BuildSnapshot() const;
	void RestoreSnapshot(const FDQSSaveSnapshot& Snapshot);
	const UQuestGraphAsset* ResolveQuestAssetFromState(const FDQSQuestRuntimeState& QuestState) const;

	UPROPERTY(Transient)
	TWeakObjectPtr<APlayerController> DialoguePlayerController;

	UPROPERTY(Transient)
	bool bDialogueInteractionModeApplied = false;

	UPROPERTY(Transient)
	bool bPreviousShowMouseCursor = false;

	UPROPERTY(Transient)
	bool bPreviousEnableClickEvents = false;

	UPROPERTY(Transient)
	bool bPreviousEnableMouseOverEvents = false;

	UPROPERTY(Transient)
	bool bPreviousIgnoreMoveInput = false;

	UPROPERTY(Transient)
	bool bPreviousIgnoreLookInput = false;

	UPROPERTY(Transient)
	bool bRestoreGameAndUIInputMode = false;
};
