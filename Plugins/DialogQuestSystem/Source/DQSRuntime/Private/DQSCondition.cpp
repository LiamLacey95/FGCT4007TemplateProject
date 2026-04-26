#include "DQSCondition.h"

#include "DialogQuestSubsystem.h"

namespace
{
	bool EvaluateIntComparison(const int32 LeftValue, const EDQSIntComparisonOperation Operation, const int32 RightValue)
	{
		switch (Operation)
		{
		case EDQSIntComparisonOperation::Equal:
			return LeftValue == RightValue;
		case EDQSIntComparisonOperation::NotEqual:
			return LeftValue != RightValue;
		case EDQSIntComparisonOperation::Greater:
			return LeftValue > RightValue;
		case EDQSIntComparisonOperation::GreaterOrEqual:
			return LeftValue >= RightValue;
		case EDQSIntComparisonOperation::Less:
			return LeftValue < RightValue;
		case EDQSIntComparisonOperation::LessOrEqual:
			return LeftValue <= RightValue;
		default:
			return false;
		}
	}

	FName GetScopedOwnerId(const EDQSStateScope Scope, const FName ScopedOwnerId, const UDialogQuestSubsystem* Subsystem)
	{
		if (Scope == EDQSStateScope::DialogueSession && ScopedOwnerId.IsNone() && Subsystem)
		{
			return Subsystem->GetCurrentDialogueLine().DialogueId;
		}

		return ScopedOwnerId;
	}
}

bool UDQSCondition::Evaluate_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context) const
{
	return true;
}

FText UDQSCondition::DescribeCondition_Implementation() const
{
	return FText::FromString(TEXT("Custom condition"));
}

bool UDQSCondition_HasTag::Evaluate_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context) const
{
	if (!Subsystem || !Tag.IsValid())
	{
		return false;
	}

	const bool bHasTag = Subsystem->HasTag(Tag);
	return bInvertResult ? !bHasTag : bHasTag;
}

FText UDQSCondition_HasTag::DescribeCondition_Implementation() const
{
	if (!Tag.IsValid())
	{
		return FText::FromString(TEXT("HasTag: [Unset Tag]"));
	}

	return bInvertResult
		? FText::Format(FText::FromString(TEXT("Does not have tag {0}")), FText::FromName(Tag.GetTagName()))
		: FText::Format(FText::FromString(TEXT("Has tag {0}")), FText::FromName(Tag.GetTagName()));
}

bool UDQSCondition_BoolVariable::Evaluate_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context) const
{
	if (!Subsystem || VariableKey.IsNone())
	{
		return false;
	}

	FDQSVariableValue VariableValue;
	const FName OwnerId = GetScopedOwnerId(Scope, ScopedOwnerId, Subsystem);
	const bool bHasValue = Subsystem->GetVariable(VariableKey, VariableValue, Scope, OwnerId);
	const bool bResolvedValue = bHasValue
		? (VariableValue.Type == EDQSValueType::Bool ? VariableValue.BoolValue : VariableValue.IntValue != 0)
		: (bTreatMissingAsFalse ? false : bExpectedValue);

	return bResolvedValue == bExpectedValue;
}

FText UDQSCondition_BoolVariable::DescribeCondition_Implementation() const
{
	return FText::Format(
		FText::FromString(TEXT("Bool variable {0} is {1}")),
		FText::FromName(VariableKey),
		bExpectedValue ? FText::FromString(TEXT("true")) : FText::FromString(TEXT("false")));
}

bool UDQSCondition_IntVariable::Evaluate_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context) const
{
	if (!Subsystem || VariableKey.IsNone())
	{
		return false;
	}

	FDQSVariableValue VariableValue;
	const FName OwnerId = GetScopedOwnerId(Scope, ScopedOwnerId, Subsystem);
	const bool bHasValue = Subsystem->GetVariable(VariableKey, VariableValue, Scope, OwnerId);
	int32 ResolvedValue = 0;

	if (bHasValue)
	{
		switch (VariableValue.Type)
		{
		case EDQSValueType::Int:
			ResolvedValue = VariableValue.IntValue;
			break;
		case EDQSValueType::Bool:
			ResolvedValue = VariableValue.BoolValue ? 1 : 0;
			break;
		case EDQSValueType::Float:
			ResolvedValue = FMath::RoundToInt(VariableValue.FloatValue);
			break;
		default:
			ResolvedValue = 0;
			break;
		}
	}
	else if (!bTreatMissingAsZero)
	{
		return false;
	}

	return EvaluateIntComparison(ResolvedValue, Operation, CompareValue);
}

FText UDQSCondition_IntVariable::DescribeCondition_Implementation() const
{
	return FText::Format(
		FText::FromString(TEXT("Int variable {0} compared to {1}")),
		FText::FromName(VariableKey),
		FText::AsNumber(CompareValue));
}

bool UDQSCondition_QuestStatus::Evaluate_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context) const
{
	if (!Subsystem || QuestId.IsNone())
	{
		return false;
	}

	FDQSQuestRuntimeState QuestState;
	const bool bHasQuest = Subsystem->GetQuestState(QuestId, QuestState);
	if (!bHasQuest)
	{
		return bInvertResult;
	}

	const bool bMatches = QuestState.Status == RequiredStatus;
	return bInvertResult ? !bMatches : bMatches;
}

FText UDQSCondition_QuestStatus::DescribeCondition_Implementation() const
{
	return FText::Format(
		FText::FromString(TEXT("Quest {0} status is {1}")),
		FText::FromName(QuestId),
		StaticEnum<EDQSQuestStatus>()->GetDisplayNameTextByValue(static_cast<int64>(RequiredStatus)));
}
