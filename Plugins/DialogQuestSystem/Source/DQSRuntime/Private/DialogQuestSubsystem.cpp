#include "DialogQuestSubsystem.h"

#include "DQSAction.h"
#include "DQSCondition.h"
#include "DQSDialogueGraphAsset.h"
#include "DQSQuestGraphAsset.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "DQSSaveGame.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogDialogQuestSubsystem, Log, All);

namespace DQSRuntime
{
	static constexpr int32 MaxExecutionDepth = 128;

	static bool IsDialogueNodeTerminal(const EDQSDialogueNodeType NodeType)
	{
		return NodeType == EDQSDialogueNodeType::Speech
			|| NodeType == EDQSDialogueNodeType::Choice
			|| NodeType == EDQSDialogueNodeType::End;
	}

	static bool IsQuestNodeTerminal(const EDQSQuestNodeType NodeType)
	{
		return NodeType == EDQSQuestNodeType::Objective
			|| NodeType == EDQSQuestNodeType::Complete
			|| NodeType == EDQSQuestNodeType::Fail;
	}
}

UDialogQuestSubsystem::UDialogQuestSubsystem()
{
}

void UDialogQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UDialogQuestSubsystem::Deinitialize()
{
	EndDialogue();
	ResetRuntimeState();
	Super::Deinitialize();
}

bool UDialogQuestSubsystem::StartDialogue(UDialogueGraphAsset* DialogueAsset, UObject* Context)
{
	if (!DialogueAsset)
	{
		UE_LOG(LogDialogQuestSubsystem, Warning, TEXT("StartDialogue failed: DialogueAsset is null."));
		return false;
	}

	DialogueAsset->NormalizeForRuntime();

	if (!DialogueAsset->EntryNodeId.IsValid())
	{
		UE_LOG(LogDialogQuestSubsystem, Warning, TEXT("StartDialogue failed for '%s': missing Entry node/runtime entry id. Open the asset, connect Entry to the first node, validate, and save."), *GetNameSafe(DialogueAsset));
		return false;
	}

	if (ActiveDialogueSession.bIsActive)
	{
		EndDialogue();
	}

	ActiveDialogueAsset = DialogueAsset;
	ActiveDialogueContext = Context;
	ActiveDialogueSession.bIsActive = true;
	ActiveDialogueSession.DialogueAsset = DialogueAsset;
	ActiveDialogueSession.DialogueId = DialogueAsset->Metadata.GraphId;
	ActiveDialogueSession.CurrentNodeId = DialogueAsset->EntryNodeId;
	ActiveDialogueSession.Variables.Reset();
	ApplyDialogueInteractionMode();
	return RunDialogueNode(DialogueAsset->EntryNodeId, true);
}

bool UDialogQuestSubsystem::ContinueDialogue()
{
	if (!ActiveDialogueAsset || !CurrentDialogueLine.bCanContinue)
	{
		return false;
	}

	if (const FDQSDialogueNode* Node = ActiveDialogueAsset->FindNodeById(CurrentDialogueLine.NodeId))
	{
		if (Node->NextNodeId.IsValid())
		{
			return RunDialogueNode(Node->NextNodeId, true);
		}
	}

	EndDialogue();
	return true;
}

bool UDialogQuestSubsystem::ChooseDialogueOption(const int32 ChoiceIndex)
{
	if (!ActiveDialogueAsset || !ActiveDialogueSession.bIsActive)
	{
		return false;
	}

	const FDQSDialogueNode* Node = ActiveDialogueAsset->FindNodeById(CurrentDialogueLine.NodeId);
	if (!Node || !Node->Choices.IsValidIndex(ChoiceIndex))
	{
		return false;
	}

	const FDQSDialogueChoice& Choice = Node->Choices[ChoiceIndex];
	if (!Choice.TargetNodeId.IsValid())
	{
		UE_LOG(LogDialogQuestSubsystem, Warning, TEXT("Choice %d on dialogue '%s' has no target node."), ChoiceIndex, *GetNameSafe(ActiveDialogueAsset));
		return false;
	}

	return RunDialogueNode(Choice.TargetNodeId, true);
}

void UDialogQuestSubsystem::EndDialogue()
{
	const bool bWasActive = ActiveDialogueSession.bIsActive;

	ActiveDialogueAsset = nullptr;
	ActiveDialogueContext = nullptr;
	ActiveDialogueSession = FDQSDialogueSessionState{};
	CurrentDialogueLine = FDQSActiveDialogueLine{};
	RestoreDialogueInteractionMode();

	if (bWasActive)
	{
		OnDialogueEnded.Broadcast();
	}
}

bool UDialogQuestSubsystem::StartQuest(UQuestGraphAsset* QuestAsset)
{
	if (!QuestAsset || QuestAsset->Metadata.GraphId.IsNone() || !QuestAsset->EntryNodeId.IsValid())
	{
		return false;
	}

	FDQSQuestRuntimeState& QuestState = QuestStates.FindOrAdd(QuestAsset->Metadata.GraphId);
	QuestState.QuestId = QuestAsset->Metadata.GraphId;
	QuestState.QuestAsset = QuestAsset;
	QuestState.Status = EDQSQuestStatus::Active;
	QuestState.ActiveNodeId = QuestAsset->EntryNodeId;
	return ProcessQuest(QuestAsset->Metadata.GraphId, true);
}

bool UDialogQuestSubsystem::AdvanceQuestObjective(const FName QuestId, const FGameplayTag ObjectiveTag, const int32 Delta)
{
	FDQSQuestRuntimeState* QuestState = QuestStates.Find(QuestId);
	if (!QuestState || QuestState->Status != EDQSQuestStatus::Active)
	{
		return false;
	}

	const UQuestGraphAsset* QuestAsset = ResolveQuestAssetFromState(*QuestState);
	if (!QuestAsset)
	{
		return false;
	}

	const FDQSQuestNode* ActiveNode = QuestAsset->FindNodeById(QuestState->ActiveNodeId);
	if (!ActiveNode || ActiveNode->NodeType != EDQSQuestNodeType::Objective || ActiveNode->ObjectiveTag != ObjectiveTag)
	{
		return false;
	}

	const FName ObjectiveKey = ObjectiveTag.GetTagName();
	const int32 NewCount = QuestState->ObjectiveProgress.FindRef(ObjectiveKey) + Delta;
	QuestState->ObjectiveProgress.Add(ObjectiveKey, NewCount);

	if (NewCount >= ActiveNode->TargetCount)
	{
		QuestState->ActiveNodeId = ActiveNode->NextNodeId;
		return ProcessQuest(QuestId, true);
	}

	return UpdateQuestStatus(QuestId, QuestState->Status, ActiveNode->Description, ObjectiveTag, NewCount, ActiveNode->TargetCount);
}

bool UDialogQuestSubsystem::FailQuest(const FName QuestId)
{
	return UpdateQuestStatus(QuestId, EDQSQuestStatus::Failed, FText::FromString(TEXT("Quest failed")));
}

bool UDialogQuestSubsystem::CompleteQuest(const FName QuestId)
{
	return UpdateQuestStatus(QuestId, EDQSQuestStatus::Completed, FText::FromString(TEXT("Quest completed")));
}

void UDialogQuestSubsystem::TriggerDialogueEvent(const FGameplayTag EventTag, const FName EventName, UObject* Context)
{
	UObject* EventContext = Context ? Context : ActiveDialogueContext.Get();
	OnDialogueEventTriggered.Broadcast(EventTag, EventName, EventContext);
}

void UDialogQuestSubsystem::SetVariable(const FName Key, FDQSVariableValue Value, const EDQSStateScope Scope, const FName ScopedOwnerId)
{
	if (Key.IsNone())
	{
		return;
	}

	if (TMap<FName, FDQSVariableValue>* VariableStore = ResolveMutableVariableStore(Scope, ScopedOwnerId))
	{
		VariableStore->Add(Key, Value);
	}
}

bool UDialogQuestSubsystem::GetVariable(const FName Key, FDQSVariableValue& OutValue, const EDQSStateScope Scope, const FName ScopedOwnerId) const
{
	if (Key.IsNone())
	{
		return false;
	}

	if (const TMap<FName, FDQSVariableValue>* VariableStore = ResolveVariableStore(Scope, ScopedOwnerId))
	{
		if (const FDQSVariableValue* Value = VariableStore->Find(Key))
		{
			OutValue = *Value;
			return true;
		}
	}

	return false;
}

void UDialogQuestSubsystem::AddTag(const FGameplayTag Tag)
{
	if (Tag.IsValid())
	{
		GlobalTags.AddTag(Tag);
	}
}

void UDialogQuestSubsystem::RemoveTag(const FGameplayTag Tag)
{
	if (Tag.IsValid())
	{
		GlobalTags.RemoveTag(Tag);
	}
}

bool UDialogQuestSubsystem::HasTag(const FGameplayTag Tag) const
{
	return GlobalTags.HasTagExact(Tag);
}

bool UDialogQuestSubsystem::SaveDialogQuestState(const FString& SlotName, const int32 UserIndex)
{
	UDQSSaveGame* SaveGame = Cast<UDQSSaveGame>(UGameplayStatics::CreateSaveGameObject(UDQSSaveGame::StaticClass()));
	if (!SaveGame)
	{
		return false;
	}

	SaveGame->Snapshot = BuildSnapshot();
	return UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, UserIndex);
}

bool UDialogQuestSubsystem::LoadDialogQuestState(const FString& SlotName, const int32 UserIndex)
{
	USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex);
	const UDQSSaveGame* SaveGame = Cast<UDQSSaveGame>(LoadedGame);
	if (!SaveGame)
	{
		return false;
	}

	RestoreSnapshot(SaveGame->Snapshot);
	return true;
}

TArray<FDQSQuestRuntimeState> UDialogQuestSubsystem::GetAllQuestStates() const
{
	TArray<FDQSQuestRuntimeState> States;
	QuestStates.GenerateValueArray(States);
	return States;
}

bool UDialogQuestSubsystem::GetQuestState(const FName QuestId, FDQSQuestRuntimeState& OutQuestState) const
{
	if (const FDQSQuestRuntimeState* QuestState = QuestStates.Find(QuestId))
	{
		OutQuestState = *QuestState;
		return true;
	}

	return false;
}

bool UDialogQuestSubsystem::RunDialogueNode(const FGuid& NodeId, const bool bExecuteSideEffects)
{
	if (!ActiveDialogueAsset || !NodeId.IsValid())
	{
		return false;
	}

	FGuid CurrentNodeId = NodeId;
	for (int32 Depth = 0; Depth < DQSRuntime::MaxExecutionDepth; ++Depth)
	{
		const FDQSDialogueNode* Node = ActiveDialogueAsset->FindNodeById(CurrentNodeId);
		if (!Node)
		{
			UE_LOG(LogDialogQuestSubsystem, Warning, TEXT("Dialogue '%s' could not resolve node %s."), *GetNameSafe(ActiveDialogueAsset), *CurrentNodeId.ToString());
			EndDialogue();
			return false;
		}

		ActiveDialogueSession.CurrentNodeId = Node->NodeId;

		switch (Node->NodeType)
		{
		case EDQSDialogueNodeType::Entry:
			if (!ResolveDialogueTargetNode(*Node, true, CurrentNodeId))
			{
				EndDialogue();
				return false;
			}
			break;
		case EDQSDialogueNodeType::Condition:
		case EDQSDialogueNodeType::ConditionedStart:
			if (!ResolveDialogueTargetNode(*Node, EvaluateConditions(Node->Conditions, ActiveDialogueContext), CurrentNodeId))
			{
				EndDialogue();
				return false;
			}
			break;
		case EDQSDialogueNodeType::Action:
			if (bExecuteSideEffects)
			{
				ExecuteActions(Node->Actions, ActiveDialogueContext);
			}
			if (!ResolveDialogueTargetNode(*Node, true, CurrentNodeId))
			{
				EndDialogue();
				return false;
			}
			break;
		case EDQSDialogueNodeType::QuestHook:
			if (bExecuteSideEffects)
			{
				ApplyQuestHook(Node->QuestHook);
			}
			if (!ResolveDialogueTargetNode(*Node, true, CurrentNodeId))
			{
				EndDialogue();
				return false;
			}
			break;
		case EDQSDialogueNodeType::Jump:
			if (!ResolveDialogueTargetNode(*Node, true, CurrentNodeId))
			{
				EndDialogue();
				return false;
			}
			break;
		case EDQSDialogueNodeType::Speech:
		case EDQSDialogueNodeType::Choice:
			PresentDialogueNode(*Node);
			return true;
		case EDQSDialogueNodeType::End:
			EndDialogue();
			return true;
		default:
			break;
		}

		check(CurrentNodeId.IsValid());
	}

	UE_LOG(LogDialogQuestSubsystem, Warning, TEXT("Dialogue '%s' exceeded execution depth while evaluating flow nodes."), *GetNameSafe(ActiveDialogueAsset));
	return false;
}

void UDialogQuestSubsystem::PresentDialogueNode(const FDQSDialogueNode& Node)
{
	CurrentDialogueLine = FDQSActiveDialogueLine{};
	CurrentDialogueLine.DialogueId = ActiveDialogueAsset ? ActiveDialogueAsset->Metadata.GraphId : NAME_None;
	CurrentDialogueLine.NodeId = Node.NodeId;
	CurrentDialogueLine.SpeakerId = Node.SpeakerId;
	CurrentDialogueLine.SpeakerName = ActiveDialogueAsset ? ActiveDialogueAsset->ResolveSpeakerName(Node.SpeakerId) : FText::GetEmpty();
	CurrentDialogueLine.Text = Node.Text;
	CurrentDialogueLine.Presentation = Node.Presentation;
	CurrentDialogueLine.Portrait = Node.Presentation.bHidePortrait
		? nullptr
		: (Node.Presentation.PortraitOverride.IsNull()
			? (ActiveDialogueAsset ? ActiveDialogueAsset->ResolveSpeakerPortrait(Node.SpeakerId) : nullptr)
			: Node.Presentation.PortraitOverride);
	CurrentDialogueLine.SpeakerStyle = Node.Presentation.SpeakerStyleOverride.IsNull()
		? (ActiveDialogueAsset ? ActiveDialogueAsset->ResolveSpeakerStyle(Node.SpeakerId) : nullptr)
		: Node.Presentation.SpeakerStyleOverride;
	CurrentDialogueLine.PresentationTags = Node.PresentationTags;
	CurrentDialogueLine.bCanContinue = Node.Choices.IsEmpty() && Node.NextNodeId.IsValid();

	for (const FDQSDialogueChoice& Choice : Node.Choices)
	{
		FDQSDialogueChoicePayload Payload;
		Payload.Text = Choice.Text;
		Payload.bIsAvailable = Choice.TargetNodeId.IsValid();
		Payload.DisabledReason = Payload.bIsAvailable ? FText::GetEmpty() : FText::FromString(TEXT("This response is not connected."));
		CurrentDialogueLine.Choices.Add(Payload);
	}

	OnDialogueUpdated.Broadcast(CurrentDialogueLine);
}

bool UDialogQuestSubsystem::EvaluateConditions(const TArray<TObjectPtr<UDQSCondition>>& Conditions, UObject* Context) const
{
	for (const UDQSCondition* Condition : Conditions)
	{
		if (Condition && !Condition->Evaluate(const_cast<UDialogQuestSubsystem*>(this), Context))
		{
			return false;
		}
	}

	return true;
}

void UDialogQuestSubsystem::ExecuteActions(const TArray<TObjectPtr<UDQSAction>>& Actions, UObject* Context)
{
	for (UDQSAction* Action : Actions)
	{
		if (Action)
		{
			Action->Execute(this, Context);
		}
	}
}

bool UDialogQuestSubsystem::ApplyQuestHook(const FDQSQuestHook& QuestHook)
{
	UQuestGraphAsset* QuestAsset = QuestHook.QuestAsset.LoadSynchronous();

	switch (QuestHook.Operation)
	{
	case EDQSQuestOperation::StartQuest:
		return StartQuest(QuestAsset);
	case EDQSQuestOperation::AdvanceObjective:
		return QuestAsset && AdvanceQuestObjective(QuestAsset->Metadata.GraphId, QuestHook.ObjectiveTag, 1);
	case EDQSQuestOperation::CompleteQuest:
		return QuestAsset && CompleteQuest(QuestAsset->Metadata.GraphId);
	case EDQSQuestOperation::FailQuest:
		return QuestAsset && FailQuest(QuestAsset->Metadata.GraphId);
	default:
		return false;
	}
}

bool UDialogQuestSubsystem::ProcessQuest(const FName QuestId, const bool bBroadcastUpdates)
{
	FDQSQuestRuntimeState* QuestState = QuestStates.Find(QuestId);
	const UQuestGraphAsset* QuestAsset = QuestState ? ResolveQuestAssetFromState(*QuestState) : nullptr;
	if (!QuestState || !QuestAsset)
	{
		return false;
	}

	FGuid CurrentNodeId = QuestState->ActiveNodeId;
	for (int32 Depth = 0; Depth < DQSRuntime::MaxExecutionDepth; ++Depth)
	{
		const FDQSQuestNode* Node = QuestAsset->FindNodeById(CurrentNodeId);
		if (!Node)
		{
			UE_LOG(LogDialogQuestSubsystem, Warning, TEXT("Quest '%s' could not resolve node %s."), *QuestId.ToString(), *CurrentNodeId.ToString());
			return false;
		}

		QuestState->ActiveNodeId = Node->NodeId;

		switch (Node->NodeType)
		{
		case EDQSQuestNodeType::Start:
			if (!ResolveQuestTargetNode(*Node, true, CurrentNodeId))
			{
				return false;
			}
			break;
		case EDQSQuestNodeType::Condition:
		case EDQSQuestNodeType::Branch:
			if (!ResolveQuestTargetNode(*Node, EvaluateConditions(Node->Conditions, nullptr), CurrentNodeId))
			{
				return false;
			}
			break;
		case EDQSQuestNodeType::Reward:
			ExecuteActions(Node->Actions, nullptr);
			if (!ResolveQuestTargetNode(*Node, true, CurrentNodeId))
			{
				return false;
			}
			break;
		case EDQSQuestNodeType::Subquest:
			if (UQuestGraphAsset* Subquest = Node->SubquestAsset.LoadSynchronous())
			{
				StartQuest(Subquest);
			}
			else
			{
				UE_LOG(LogDialogQuestSubsystem, Warning, TEXT("Quest '%s' has a Subquest node without a valid subquest asset."), *QuestId.ToString());
				return false;
			}
			if (!ResolveQuestTargetNode(*Node, true, CurrentNodeId))
			{
				return false;
			}
			break;
		case EDQSQuestNodeType::Objective:
		{
			const FName ObjectiveKey = Node->ObjectiveTag.GetTagName();
			const int32 CurrentCount = QuestState->ObjectiveProgress.FindRef(ObjectiveKey);
			if (CurrentCount >= Node->TargetCount)
			{
				if (!ResolveQuestTargetNode(*Node, true, CurrentNodeId))
				{
					return false;
				}
				break;
			}

			QuestState->ActiveNodeId = Node->NodeId;
			return !bBroadcastUpdates || UpdateQuestStatus(QuestId, EDQSQuestStatus::Active, Node->Description, Node->ObjectiveTag, CurrentCount, Node->TargetCount);
		}
		case EDQSQuestNodeType::Complete:
			return UpdateQuestStatus(QuestId, EDQSQuestStatus::Completed, Node->Description, Node->ObjectiveTag);
		case EDQSQuestNodeType::Fail:
			return UpdateQuestStatus(QuestId, EDQSQuestStatus::Failed, Node->Description, Node->ObjectiveTag);
		default:
			return false;
		}
	}

	UE_LOG(LogDialogQuestSubsystem, Warning, TEXT("Quest '%s' exceeded execution depth while evaluating flow nodes."), *QuestId.ToString());
	return false;
}

bool UDialogQuestSubsystem::UpdateQuestStatus(const FName QuestId, const EDQSQuestStatus NewStatus, const FText& Message, const FGameplayTag& ObjectiveTag, const int32 CurrentCount, const int32 TargetCount)
{
	FDQSQuestRuntimeState* QuestState = QuestStates.Find(QuestId);
	const UQuestGraphAsset* QuestAsset = QuestState ? ResolveQuestAssetFromState(*QuestState) : nullptr;
	if (!QuestState || !QuestAsset)
	{
		return false;
	}

	QuestState->Status = NewStatus;

	FDQSQuestStatusUpdate Update;
	Update.QuestId = QuestId;
	Update.Status = NewStatus;
	Update.Title = QuestAsset->Metadata.DisplayName;
	Update.Message = Message;
	Update.ObjectiveTag = ObjectiveTag;
	Update.CurrentCount = CurrentCount;
	Update.TargetCount = TargetCount;
	OnQuestUpdated.Broadcast(Update);
	return true;
}

FDQSSaveSnapshot UDialogQuestSubsystem::BuildSnapshot() const
{
	FDQSSaveSnapshot Snapshot;
	Snapshot.GlobalVariables = GlobalVariables;
	Snapshot.GlobalTags = GlobalTags;
	Snapshot.QuestStates = QuestStates;
	Snapshot.DialogueSession = ActiveDialogueSession;
	return Snapshot;
}

void UDialogQuestSubsystem::RestoreSnapshot(const FDQSSaveSnapshot& Snapshot)
{
	GlobalVariables = Snapshot.GlobalVariables;
	GlobalTags = Snapshot.GlobalTags;
	QuestStates = Snapshot.QuestStates;
	ActiveDialogueSession = Snapshot.DialogueSession;
	CurrentDialogueLine = FDQSActiveDialogueLine{};
	ActiveDialogueContext = nullptr;
	ActiveDialogueAsset = nullptr;

	if (ActiveDialogueSession.bIsActive)
	{
		ActiveDialogueAsset = ActiveDialogueSession.DialogueAsset.LoadSynchronous();
		if (ActiveDialogueAsset)
		{
			ActiveDialogueAsset->NormalizeForRuntime();
		}
		if (ActiveDialogueAsset && ActiveDialogueSession.CurrentNodeId.IsValid())
		{
			RunDialogueNode(ActiveDialogueSession.CurrentNodeId, false);
		}
		else
		{
			EndDialogue();
		}
	}
}

const UQuestGraphAsset* UDialogQuestSubsystem::ResolveQuestAssetFromState(const FDQSQuestRuntimeState& QuestState) const
{
	return QuestState.QuestAsset.LoadSynchronous();
}

void UDialogQuestSubsystem::ApplyDialogueInteractionMode()
{
	UGameInstance* GameInstance = GetGameInstance();
	UWorld* World = GameInstance ? GameInstance->GetWorld() : nullptr;
	APlayerController* PlayerController = GameInstance ? GameInstance->GetFirstLocalPlayerController(World) : nullptr;
	if (!PlayerController)
	{
		PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	}

	if (!PlayerController)
	{
		return;
	}

	DialoguePlayerController = PlayerController;
	bPreviousShowMouseCursor = PlayerController->bShowMouseCursor;
	bPreviousEnableClickEvents = PlayerController->bEnableClickEvents;
	bPreviousEnableMouseOverEvents = PlayerController->bEnableMouseOverEvents;
	bPreviousIgnoreMoveInput = PlayerController->IsMoveInputIgnored();
	bPreviousIgnoreLookInput = PlayerController->IsLookInputIgnored();
	bRestoreGameAndUIInputMode = bPreviousShowMouseCursor || bPreviousEnableClickEvents || bPreviousEnableMouseOverEvents;

	PlayerController->SetIgnoreMoveInput(true);
	PlayerController->SetIgnoreLookInput(true);
	PlayerController->bShowMouseCursor = true;
	PlayerController->bEnableClickEvents = true;
	PlayerController->bEnableMouseOverEvents = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputMode);

	if (APawn* ControlledPawn = PlayerController->GetPawn())
	{
		if (UPawnMovementComponent* MovementComponent = ControlledPawn->GetMovementComponent())
		{
			MovementComponent->StopMovementImmediately();
		}

		if (ACharacter* Character = Cast<ACharacter>(ControlledPawn))
		{
			if (UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement())
			{
				CharacterMovement->StopMovementImmediately();
			}
		}
	}

	bDialogueInteractionModeApplied = true;
}

void UDialogQuestSubsystem::RestoreDialogueInteractionMode()
{
	if (!bDialogueInteractionModeApplied)
	{
		return;
	}

	if (APlayerController* PlayerController = DialoguePlayerController.Get())
	{
		PlayerController->SetIgnoreMoveInput(bPreviousIgnoreMoveInput);
		PlayerController->SetIgnoreLookInput(bPreviousIgnoreLookInput);
		PlayerController->bShowMouseCursor = bPreviousShowMouseCursor;
		PlayerController->bEnableClickEvents = bPreviousEnableClickEvents;
		PlayerController->bEnableMouseOverEvents = bPreviousEnableMouseOverEvents;

		if (bRestoreGameAndUIInputMode)
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetHideCursorDuringCapture(false);
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputMode);
		}
		else
		{
			FInputModeGameOnly InputMode;
			PlayerController->SetInputMode(InputMode);
		}
	}

	DialoguePlayerController = nullptr;
	bDialogueInteractionModeApplied = false;
}

TMap<FName, FDQSVariableValue>* UDialogQuestSubsystem::ResolveMutableVariableStore(const EDQSStateScope Scope, FName ScopedOwnerId)
{
	switch (Scope)
	{
	case EDQSStateScope::Global:
		return &GlobalVariables;
	case EDQSStateScope::Quest:
		if (FDQSQuestRuntimeState* QuestState = QuestStates.Find(ScopedOwnerId))
		{
			return &QuestState->Variables;
		}
		break;
	case EDQSStateScope::DialogueSession:
		if (ScopedOwnerId.IsNone())
		{
			ScopedOwnerId = ActiveDialogueSession.DialogueId;
		}
		if (ActiveDialogueSession.bIsActive && ActiveDialogueSession.DialogueId == ScopedOwnerId)
		{
			return &ActiveDialogueSession.Variables;
		}
		break;
	default:
		break;
	}

	return nullptr;
}

const TMap<FName, FDQSVariableValue>* UDialogQuestSubsystem::ResolveVariableStore(const EDQSStateScope Scope, FName ScopedOwnerId) const
{
	switch (Scope)
	{
	case EDQSStateScope::Global:
		return &GlobalVariables;
	case EDQSStateScope::Quest:
		if (const FDQSQuestRuntimeState* QuestState = QuestStates.Find(ScopedOwnerId))
		{
			return &QuestState->Variables;
		}
		break;
	case EDQSStateScope::DialogueSession:
		if (ScopedOwnerId.IsNone())
		{
			ScopedOwnerId = ActiveDialogueSession.DialogueId;
		}
		if (ActiveDialogueSession.bIsActive && ActiveDialogueSession.DialogueId == ScopedOwnerId)
		{
			return &ActiveDialogueSession.Variables;
		}
		break;
	default:
		break;
	}

	return nullptr;
}

bool UDialogQuestSubsystem::ResolveDialogueTargetNode(const FDQSDialogueNode& Node, const bool bMatchedPath, FGuid& OutTargetNodeId) const
{
	switch (Node.NodeType)
	{
	case EDQSDialogueNodeType::Condition:
	case EDQSDialogueNodeType::ConditionedStart:
		OutTargetNodeId = bMatchedPath ? Node.NextNodeId : Node.AlternateNodeId;
		break;
	case EDQSDialogueNodeType::Jump:
		OutTargetNodeId = Node.JumpTargetNodeId;
		break;
	default:
		OutTargetNodeId = Node.NextNodeId;
		break;
	}

	if (OutTargetNodeId.IsValid())
	{
		return true;
	}

	if (!DQSRuntime::IsDialogueNodeTerminal(Node.NodeType))
	{
		UE_LOG(LogDialogQuestSubsystem, Warning, TEXT("Dialogue node '%s' is missing a required outbound connection."), *Node.Title.ToString());
	}

	return false;
}

bool UDialogQuestSubsystem::ResolveQuestTargetNode(const FDQSQuestNode& Node, const bool bMatchedPath, FGuid& OutTargetNodeId) const
{
	switch (Node.NodeType)
	{
	case EDQSQuestNodeType::Condition:
	case EDQSQuestNodeType::Branch:
		OutTargetNodeId = bMatchedPath ? Node.NextNodeId : Node.AlternateNodeId;
		break;
	default:
		OutTargetNodeId = Node.NextNodeId;
		break;
	}

	if (OutTargetNodeId.IsValid())
	{
		return true;
	}

	if (!DQSRuntime::IsQuestNodeTerminal(Node.NodeType))
	{
		UE_LOG(LogDialogQuestSubsystem, Warning, TEXT("Quest node '%s' is missing a required outbound connection."), *Node.Title.ToString());
	}

	return false;
}

void UDialogQuestSubsystem::ResetRuntimeState()
{
	QuestStates.Reset();
	GlobalVariables.Reset();
	GlobalTags.Reset();
	ActiveDialogueSession = FDQSDialogueSessionState{};
	CurrentDialogueLine = FDQSActiveDialogueLine{};
	ActiveDialogueAsset = nullptr;
	ActiveDialogueContext = nullptr;
}





