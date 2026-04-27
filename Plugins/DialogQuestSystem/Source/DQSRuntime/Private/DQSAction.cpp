#include "DQSAction.h"

#include "DQSInterfaces.h"
#include "DialogQuestSubsystem.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"

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

	FName ResolveDialogueEventName(const FGameplayTag EventTag, const FName EventName)
	{
		return EventName.IsNone() && EventTag.IsValid() ? EventTag.GetTagName() : EventName;
	}

	void SendDialogueEventToObject(UObject* Target, const FGameplayTag EventTag, const FName EventName, UObject* Context)
	{
		if (Target && Target->GetClass()->ImplementsInterface(UDQSDialogueEventReceiverInterface::StaticClass()))
		{
			IDQSDialogueEventReceiverInterface::Execute_ReceiveDQSDialogueEvent(Target, EventTag, EventName, Context);
		}
	}

	UWorld* ResolveActionWorld(UDialogQuestSubsystem* Subsystem, UObject* Context)
	{
		if (Context && Context->GetWorld())
		{
			return Context->GetWorld();
		}

		return Subsystem ? Subsystem->GetWorld() : nullptr;
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

void UDQSAction_TriggerDialogueEvent::Execute_Implementation(UDialogQuestSubsystem* Subsystem, UObject* Context)
{
	if (!Subsystem)
	{
		return;
	}

	const FName ResolvedEventName = ResolveDialogueEventName(EventTag, EventName);
	Subsystem->TriggerDialogueEvent(EventTag, ResolvedEventName, Context);

	switch (TargetMode)
	{
	case EDQSDialogueEventTargetMode::SubsystemOnly:
		break;
	case EDQSDialogueEventTargetMode::ContextObject:
		SendDialogueEventToObject(Context, EventTag, ResolvedEventName, Context);
		break;
	case EDQSDialogueEventTargetMode::AllReceivers:
		if (UWorld* World = ResolveActionWorld(Subsystem, Context))
		{
			for (TActorIterator<AActor> It(World); It; ++It)
			{
				SendDialogueEventToObject(*It, EventTag, ResolvedEventName, Context);
			}
		}
		break;
	case EDQSDialogueEventTargetMode::ActorsWithTag:
		if (UWorld* World = ResolveActionWorld(Subsystem, Context))
		{
			for (TActorIterator<AActor> It(World); It; ++It)
			{
				if (!ActorTag.IsNone() && It->ActorHasTag(ActorTag))
				{
					SendDialogueEventToObject(*It, EventTag, ResolvedEventName, Context);
				}
			}
		}
		break;
	case EDQSDialogueEventTargetMode::ActorsOfClass:
		if (UWorld* World = ResolveActionWorld(Subsystem, Context))
		{
			const UClass* RequiredClass = TargetActorClass ? *TargetActorClass : AActor::StaticClass();
			for (TActorIterator<AActor> It(World); It; ++It)
			{
				if (It->IsA(RequiredClass))
				{
					SendDialogueEventToObject(*It, EventTag, ResolvedEventName, Context);
				}
			}
		}
		break;
	default:
		break;
	}
}

FText UDQSAction_TriggerDialogueEvent::DescribeAction_Implementation() const
{
	const FName ResolvedEventName = ResolveDialogueEventName(EventTag, EventName);
	if (!ResolvedEventName.IsNone())
	{
		return FText::Format(FText::FromString(TEXT("Trigger dialogue event {0}")), FText::FromName(ResolvedEventName));
	}

	return FText::FromString(TEXT("Trigger dialogue event"));
}
