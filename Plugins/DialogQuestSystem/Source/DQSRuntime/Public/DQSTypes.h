#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/SoftObjectPtr.h"
#include "DQSTypes.generated.h"

class UDQSAction;
class UDQSCondition;
class UDQSSpeakerStyleAsset;
class UDialogueGraphAsset;
class UQuestGraphAsset;
class USoundBase;
class UTexture2D;

UENUM(BlueprintType)
enum class EDQSValueType : uint8
{
	None,
	Bool,
	Int,
	Float,
	Name,
	String
};

UENUM(BlueprintType)
enum class EDQSStateScope : uint8
{
	Global,
	Quest,
	DialogueSession
};

UENUM(BlueprintType)
enum class EDQSDialogueNodeType : uint8
{
	Entry,
	Speech,
	Choice,
	Condition UMETA(DisplayName = "Condition (Legacy)"),
	Action UMETA(DisplayName = "Set Condition"),
	QuestHook,
	Jump,
	End,
	ConditionedStart UMETA(DisplayName = "Conditioned Branch")
};

UENUM(BlueprintType)
enum class EDQSQuestNodeType : uint8
{
	Start,
	Objective,
	Condition,
	Branch,
	Reward,
	Fail,
	Complete,
	Subquest
};

UENUM(BlueprintType)
enum class EDQSQuestStatus : uint8
{
	Inactive,
	Available,
	Active,
	Completed,
	Failed
};

UENUM(BlueprintType)
enum class EDQSQuestOperation : uint8
{
	None,
	StartQuest,
	AdvanceObjective,
	CompleteQuest,
	FailQuest
};

USTRUCT(BlueprintType)
struct DQSRUNTIME_API FDQSVariableValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	EDQSValueType Type = EDQSValueType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	bool BoolValue = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	int32 IntValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	float FloatValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FName NameValue = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FString StringValue;

	static FDQSVariableValue MakeBool(bool bValue);
	static FDQSVariableValue MakeInt(int32 Value);
	static FDQSVariableValue MakeFloat(float Value);
	static FDQSVariableValue MakeName(FName Value);
	static FDQSVariableValue MakeString(const FString& Value);
};

USTRUCT(BlueprintType)
struct DQSRUNTIME_API FDQSGraphMetadata
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FName GraphId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FGameplayTagContainer Tags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System", meta = (MultiLine = true))
	FText AuthorNotes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dialog Quest System")
	bool bLastValidationSucceeded = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dialog Quest System")
	FText LastValidationSummary;
};

USTRUCT(BlueprintType)
struct DQSRUNTIME_API FDQSSpeakerDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FName SpeakerId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	TSoftObjectPtr<UTexture2D> Portrait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	TSoftObjectPtr<UDQSSpeakerStyleAsset> SpeakerStyle;
};

USTRUCT(BlueprintType)
struct DQSRUNTIME_API FDQSDialoguePresentationOverride
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	TSoftObjectPtr<UTexture2D> PortraitOverride;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	TSoftObjectPtr<UDQSSpeakerStyleAsset> SpeakerStyleOverride;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	bool bHidePortrait = false;
};

USTRUCT(BlueprintType)
struct DQSRUNTIME_API FDQSQuestHook
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	EDQSQuestOperation Operation = EDQSQuestOperation::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	TSoftObjectPtr<UQuestGraphAsset> QuestAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FGameplayTag ObjectiveTag;
};

USTRUCT(BlueprintType)
struct DQSRUNTIME_API FDQSDialogueChoice
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System", AdvancedDisplay)
	FGuid ChoiceId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System", AdvancedDisplay)
	FGuid TargetNodeId;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Dialog Quest System", meta = (DeprecatedProperty, DeprecationMessage = "Use Conditioned Branch nodes instead of per-choice conditions."), AdvancedDisplay)
	TArray<TObjectPtr<UDQSCondition>> Conditions;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Dialog Quest System", meta = (DeprecatedProperty, DeprecationMessage = "Use Set Condition nodes instead of per-choice actions."), AdvancedDisplay)
	TArray<TObjectPtr<UDQSAction>> Actions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FText FailureReason;
};

USTRUCT(BlueprintType)
struct DQSRUNTIME_API FDQSDialogueNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System", AdvancedDisplay)
	FGuid NodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	EDQSDialogueNodeType NodeType = EDQSDialogueNodeType::Speech;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FName SpeakerId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System", meta = (MultiLine = true))
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FGameplayTagContainer PresentationTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	TSoftObjectPtr<USoundBase> VoiceSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FDQSDialoguePresentationOverride Presentation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System", AdvancedDisplay)
	FGuid NextNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System", AdvancedDisplay)
	FGuid AlternateNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System", AdvancedDisplay)
	FGuid JumpTargetNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	TArray<FDQSDialogueChoice> Choices;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Dialog Quest System")
	TArray<TObjectPtr<UDQSCondition>> Conditions;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Dialog Quest System")
	TArray<TObjectPtr<UDQSAction>> Actions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FDQSQuestHook QuestHook;
};

USTRUCT(BlueprintType)
struct DQSRUNTIME_API FDQSQuestNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System", AdvancedDisplay)
	FGuid NodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	EDQSQuestNodeType NodeType = EDQSQuestNodeType::Objective;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FGameplayTag ObjectiveTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	int32 TargetCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System", AdvancedDisplay)
	FGuid NextNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System", AdvancedDisplay)
	FGuid AlternateNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	TSoftObjectPtr<UQuestGraphAsset> SubquestAsset;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Dialog Quest System")
	TArray<TObjectPtr<UDQSCondition>> Conditions;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Dialog Quest System")
	TArray<TObjectPtr<UDQSAction>> Actions;
};

USTRUCT(BlueprintType)
struct DQSRUNTIME_API FDQSDialogueChoicePayload
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	FText Text;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	bool bIsAvailable = true;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	FText DisabledReason;
};

USTRUCT(BlueprintType)
struct DQSRUNTIME_API FDQSActiveDialogueLine
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	FName DialogueId = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	FGuid NodeId;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	FName SpeakerId = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	FText SpeakerName;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	FText Text;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	TSoftObjectPtr<UTexture2D> Portrait;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	TSoftObjectPtr<UDQSSpeakerStyleAsset> SpeakerStyle;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	FGameplayTagContainer PresentationTags;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	FDQSDialoguePresentationOverride Presentation;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	TArray<FDQSDialogueChoicePayload> Choices;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	bool bCanContinue = false;
};

USTRUCT(BlueprintType)
struct DQSRUNTIME_API FDQSDialogueSessionState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	bool bIsActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FName DialogueId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	TSoftObjectPtr<UDialogueGraphAsset> DialogueAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FGuid CurrentNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	TMap<FName, FDQSVariableValue> Variables;
};

USTRUCT(BlueprintType)
struct DQSRUNTIME_API FDQSQuestRuntimeState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FName QuestId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	TSoftObjectPtr<UQuestGraphAsset> QuestAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	EDQSQuestStatus Status = EDQSQuestStatus::Inactive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FGuid ActiveNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	TMap<FName, int32> ObjectiveProgress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	TMap<FName, FDQSVariableValue> Variables;
};

USTRUCT(BlueprintType)
struct DQSRUNTIME_API FDQSQuestStatusUpdate
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	FName QuestId = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	EDQSQuestStatus Status = EDQSQuestStatus::Inactive;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	FText Title;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	FText Message;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	FGameplayTag ObjectiveTag;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	int32 CurrentCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	int32 TargetCount = 0;
};

USTRUCT(BlueprintType)
struct DQSRUNTIME_API FDQSSaveSnapshot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	TMap<FName, FDQSVariableValue> GlobalVariables;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FGameplayTagContainer GlobalTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	TMap<FName, FDQSQuestRuntimeState> QuestStates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FDQSDialogueSessionState DialogueSession;
};






