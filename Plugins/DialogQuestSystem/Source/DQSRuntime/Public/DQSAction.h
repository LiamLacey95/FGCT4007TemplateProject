#pragma once

#include "DQSTypes.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "DQSAction.generated.h"

class UDialogQuestSubsystem;

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class DQSRUNTIME_API UDQSAction : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Dialog Quest System")
	void Execute(UDialogQuestSubsystem* Subsystem, UObject* Context);

	virtual void Execute_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context);

	UFUNCTION(BlueprintNativeEvent, Category = "Dialog Quest System")
	FText DescribeAction() const;

	virtual FText DescribeAction_Implementation() const;
};

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class DQSRUNTIME_API UDQSAction_SetVariable : public UDQSAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Action")
	FName VariableKey = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Action")
	FDQSVariableValue Value = FDQSVariableValue::MakeBool(true);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Action")
	EDQSStateScope Scope = EDQSStateScope::Global;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Action", meta = (EditCondition = "Scope != EDQSStateScope::Global"))
	FName ScopedOwnerId = NAME_None;

	virtual void Execute_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context) override;
	virtual FText DescribeAction_Implementation() const override;
};

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class DQSRUNTIME_API UDQSAction_SetBoolVariable : public UDQSAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Action")
	FName VariableKey = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Action")
	bool bValue = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Action")
	EDQSStateScope Scope = EDQSStateScope::Global;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Action", meta = (EditCondition = "Scope != EDQSStateScope::Global"))
	FName ScopedOwnerId = NAME_None;

	virtual void Execute_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context) override;
	virtual FText DescribeAction_Implementation() const override;
};

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class DQSRUNTIME_API UDQSAction_AddTag : public UDQSAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Action")
	FGameplayTag Tag;

	virtual void Execute_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context) override;
	virtual FText DescribeAction_Implementation() const override;
};

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class DQSRUNTIME_API UDQSAction_RemoveTag : public UDQSAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Action")
	FGameplayTag Tag;

	virtual void Execute_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context) override;
	virtual FText DescribeAction_Implementation() const override;
};
