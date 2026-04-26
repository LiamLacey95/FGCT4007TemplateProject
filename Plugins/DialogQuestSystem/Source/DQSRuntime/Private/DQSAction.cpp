#include "DQSAction.h"

#include "DialogQuestSubsystem.h"

namespace
{
	FName ResolveActionScopedOwnerId(const EDQSStateScope Scope, const FName ScopedOwnerId, const UDialogQuestSubsystem* Subsystem)
	{
		if (Scope == EDQSStateScope::DialogueSession && ScopedOwnerId.IsNone() && Subsystem)
		{
			return Subsystem->GetCurrentDialogueLine().DialogueId;
		}

		return ScopedOwnerId;
	}
}

void UDQSAction::Execute_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context)
{
}

FText UDQSAction::DescribeAction_Implementation() const
{
	return FText::FromString(TEXT("Custom action"));
}

void UDQSAction_SetVariable::Execute_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context)
{
	if (!Subsystem || VariableKey.IsNone())
	{
		return;
	}

	Subsystem->SetVariable(VariableKey, Value, Scope, ResolveActionScopedOwnerId(Scope, ScopedOwnerId, Subsystem));
}

FText UDQSAction_SetVariable::DescribeAction_Implementation() const
{
	return FText::Format(FText::FromString(TEXT("Set variable {0}")), FText::FromName(VariableKey));
}

void UDQSAction_SetBoolVariable::Execute_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context)
{
	if (!Subsystem || VariableKey.IsNone())
	{
		return;
	}

	Subsystem->SetVariable(VariableKey, FDQSVariableValue::MakeBool(bValue), Scope, ResolveActionScopedOwnerId(Scope, ScopedOwnerId, Subsystem));
}

FText UDQSAction_SetBoolVariable::DescribeAction_Implementation() const
{
	return FText::Format(
		FText::FromString(TEXT("Set bool variable {0} to {1}")),
		FText::FromName(VariableKey),
		bValue ? FText::FromString(TEXT("true")) : FText::FromString(TEXT("false")));
}

void UDQSAction_AddTag::Execute_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context)
{
	if (!Subsystem || !Tag.IsValid())
	{
		return;
	}

	Subsystem->AddTag(Tag);
}

FText UDQSAction_AddTag::DescribeAction_Implementation() const
{
	return FText::Format(FText::FromString(TEXT("Add tag {0}")), FText::FromName(Tag.GetTagName()));
}

void UDQSAction_RemoveTag::Execute_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context)
{
	if (!Subsystem || !Tag.IsValid())
	{
		return;
	}

	Subsystem->RemoveTag(Tag);
}

FText UDQSAction_RemoveTag::DescribeAction_Implementation() const
{
	return FText::Format(FText::FromString(TEXT("Remove tag {0}")), FText::FromName(Tag.GetTagName()));
}
