#pragma once

#include "DQSTypes.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "DQSCondition.generated.h"

class UDialogQuestSubsystem;

UENUM(BlueprintType)
enum class EDQSIntComparisonOperation : uint8
{
	Equal,
	NotEqual,
	Greater,
	GreaterOrEqual,
	Less,
	LessOrEqual
};

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class DQSRUNTIME_API UDQSCondition : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Dialog Quest System")
	bool Evaluate(UDialogQuestSubsystem* Subsystem, UObject* Context) const;

	virtual bool Evaluate_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Dialog Quest System")
	FText DescribeCondition() const;

	virtual FText DescribeCondition_Implementation() const;
};

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class DQSRUNTIME_API UDQSCondition_HasTag : public UDQSCondition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Condition")
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Condition")
	bool bInvertResult = false;

	virtual bool Evaluate_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context) const override;
	virtual FText DescribeCondition_Implementation() const override;
};

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class DQSRUNTIME_API UDQSCondition_BoolVariable : public UDQSCondition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Condition")
	FName VariableKey = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Condition")
	EDQSStateScope Scope = EDQSStateScope::Global;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Condition", meta = (EditCondition = "Scope != EDQSStateScope::Global"))
	FName ScopedOwnerId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Condition")
	bool bExpectedValue = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Condition")
	bool bTreatMissingAsFalse = true;

	virtual bool Evaluate_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context) const override;
	virtual FText DescribeCondition_Implementation() const override;
};

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class DQSRUNTIME_API UDQSCondition_IntVariable : public UDQSCondition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Condition")
	FName VariableKey = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Condition")
	EDQSStateScope Scope = EDQSStateScope::Global;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Condition", meta = (EditCondition = "Scope != EDQSStateScope::Global"))
	FName ScopedOwnerId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Condition")
	EDQSIntComparisonOperation Operation = EDQSIntComparisonOperation::GreaterOrEqual;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Condition")
	int32 CompareValue = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Condition")
	bool bTreatMissingAsZero = true;

	virtual bool Evaluate_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context) const override;
	virtual FText DescribeCondition_Implementation() const override;
};

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class DQSRUNTIME_API UDQSCondition_QuestStatus : public UDQSCondition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Condition")
	FName QuestId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Condition")
	EDQSQuestStatus RequiredStatus = EDQSQuestStatus::Active;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Condition")
	bool bInvertResult = false;

	virtual bool Evaluate_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context) const override;
	virtual FText DescribeCondition_Implementation() const override;
};
