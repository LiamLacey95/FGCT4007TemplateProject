#include "DQSEdGraphNodes.h"

#include "DQSGraphEditorUtils.h"
#include "EdGraph/EdGraphPin.h"
#include "Framework/Commands/GenericCommands.h"
#include "GraphEditorActions.h"
#include "ScopedTransaction.h"
#include "ToolMenus.h"

namespace
{
	const FName FlowPinCategory(TEXT("DQSFlow"));

	FText GetDialogueTypeName(const EDQSDialogueNodeType NodeType)
	{
		return StaticEnum<EDQSDialogueNodeType>()->GetDisplayNameTextByValue(static_cast<int64>(NodeType));
	}

	FText GetQuestTypeName(const EDQSQuestNodeType NodeType)
	{
		return StaticEnum<EDQSQuestNodeType>()->GetDisplayNameTextByValue(static_cast<int64>(NodeType));
	}

	FText GetDialoguePaletteCategory(const EDQSDialogueNodeType NodeType)
	{
		switch (NodeType)
		{
		case EDQSDialogueNodeType::Speech:
		case EDQSDialogueNodeType::Choice:
			return FText::FromString(TEXT("Most Common"));
		case EDQSDialogueNodeType::Entry:
		case EDQSDialogueNodeType::End:
			return FText::FromString(TEXT("Endings"));
		case EDQSDialogueNodeType::QuestHook:
			return FText::FromString(TEXT("Quest"));
		case EDQSDialogueNodeType::Condition:
		case EDQSDialogueNodeType::ConditionedStart:
			return FText::FromString(TEXT("Branching"));
		case EDQSDialogueNodeType::Action:
		case EDQSDialogueNodeType::Jump:
			return FText::FromString(TEXT("Logic"));
		default:
			return FText::FromString(TEXT("Logic"));
		}
	}

	FText GetQuestPaletteCategory(const EDQSQuestNodeType NodeType)
	{
		switch (NodeType)
		{
		case EDQSQuestNodeType::Objective:
		case EDQSQuestNodeType::Complete:
			return FText::FromString(TEXT("Most Common"));
		case EDQSQuestNodeType::Start:
			return FText::FromString(TEXT("Flow"));
		case EDQSQuestNodeType::Condition:
		case EDQSQuestNodeType::Branch:
			return FText::FromString(TEXT("Branching"));
		case EDQSQuestNodeType::Reward:
		case EDQSQuestNodeType::Subquest:
			return FText::FromString(TEXT("Quest"));
		default:
			return FText::FromString(TEXT("Endings"));
		}
	}

	void ResetChoiceIds(TArray<FDQSDialogueChoice>& Choices)
	{
		for (FDQSDialogueChoice& Choice : Choices)
		{
			if (!Choice.ChoiceId.IsValid())
			{
				Choice.ChoiceId = FGuid::NewGuid();
			}
		}
	}

	bool HasDialogueNodeType(const UEdGraph* Graph, const EDQSDialogueNodeType NodeType)
	{
		if (!Graph)
		{
			return false;
		}

		for (const UEdGraphNode* GraphNode : Graph->Nodes)
		{
			const UDQSDialogueEdGraphNode* DialogueNode = Cast<UDQSDialogueEdGraphNode>(GraphNode);
			if (DialogueNode && DialogueNode->NodeData.NodeType == NodeType)
			{
				return true;
			}
		}

		return false;
	}

	bool HasQuestNodeType(const UEdGraph* Graph, const EDQSQuestNodeType NodeType)
	{
		if (!Graph)
		{
			return false;
		}

		for (const UEdGraphNode* GraphNode : Graph->Nodes)
		{
			const UDQSQuestEdGraphNode* QuestNode = Cast<UDQSQuestEdGraphNode>(GraphNode);
			if (QuestNode && QuestNode->NodeData.NodeType == NodeType)
			{
				return true;
			}
		}

		return false;
	}

	bool TryAutoconnectNewNode(UEdGraphNode* NewNode, UEdGraphPin* FromPin)
	{
		if (!NewNode || !FromPin || !NewNode->GetGraph())
		{
			return false;
		}

		const EEdGraphPinDirection WantedDirection = FromPin->Direction == EGPD_Output ? EGPD_Input : EGPD_Output;
		UEdGraphPin* CandidatePin = nullptr;
		for (UEdGraphPin* Pin : NewNode->Pins)
		{
			if (Pin && Pin->Direction == WantedDirection && Pin->PinType.PinCategory == FromPin->PinType.PinCategory)
			{
				CandidatePin = Pin;
				break;
			}
		}

		if (!CandidatePin)
		{
			return false;
		}

		UEdGraphPin* OutputPin = FromPin->Direction == EGPD_Output ? FromPin : CandidatePin;
		UEdGraphPin* InputPin = FromPin->Direction == EGPD_Input ? FromPin : CandidatePin;
		const UEdGraphSchema* Schema = NewNode->GetGraph()->GetSchema();
		if (!Schema || !Schema->TryCreateConnection(OutputPin, InputPin))
		{
			return false;
		}

		OutputPin->GetOwningNode()->NodeConnectionListChanged();
		InputPin->GetOwningNode()->NodeConnectionListChanged();
		return true;
	}

	bool IsChoiceArrayEdit(const FPropertyChangedEvent& PropertyChangedEvent)
	{
		const FProperty* ChangedProperty = PropertyChangedEvent.Property ? PropertyChangedEvent.Property : PropertyChangedEvent.MemberProperty;
		return ChangedProperty && ChangedProperty->GetFName() == GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, Choices);
	}

	FName GetChoicePinName(const FDQSDialogueChoice& Choice, const int32 Index)
	{
		if (!Choice.Text.IsEmpty())
		{
			return FName(*Choice.Text.ToString());
		}

		return FName(*FString::Printf(TEXT("Choice %d"), Index + 1));
	}

	UDQSDialogueEdGraphNode* CreateDialogueGraphNode(UEdGraph* ParentGraph, const FVector2D Location, const EDQSDialogueNodeType NodeType)
	{
		if (!ParentGraph)
		{
			return nullptr;
		}

		UDQSDialogueEdGraphNode* GraphNode = NewObject<UDQSDialogueEdGraphNode>(ParentGraph, UDQSDialogueEdGraphNode::StaticClass(), NAME_None, RF_Transactional);
		GraphNode->CreateNewGuid();
		GraphNode->NodePosX = Location.X;
		GraphNode->NodePosY = Location.Y;
		GraphNode->NodeData.NodeType = NodeType;
		GraphNode->NodeData.Title = GetDialogueTypeName(NodeType);
		if (!GraphNode->NodeData.NodeId.IsValid())
		{
			GraphNode->NodeData.NodeId = FGuid::NewGuid();
		}
		if (NodeType == EDQSDialogueNodeType::Choice)
		{
			GraphNode->NodeData.Choices.SetNum(2);
			ResetChoiceIds(GraphNode->NodeData.Choices);
		}

		ParentGraph->AddNode(GraphNode, true, false);
		GraphNode->PostPlacedNewNode();
		GraphNode->AllocateDefaultPins();
		GraphNode->AutowireNewNode(nullptr);
		GraphNode->SnapToGrid(16);
		return GraphNode;
	}

	UDQSQuestEdGraphNode* CreateQuestGraphNode(UEdGraph* ParentGraph, const FVector2D Location, const EDQSQuestNodeType NodeType)
	{
		if (!ParentGraph)
		{
			return nullptr;
		}

		UDQSQuestEdGraphNode* GraphNode = NewObject<UDQSQuestEdGraphNode>(ParentGraph, UDQSQuestEdGraphNode::StaticClass(), NAME_None, RF_Transactional);
		GraphNode->CreateNewGuid();
		GraphNode->NodePosX = Location.X;
		GraphNode->NodePosY = Location.Y;
		GraphNode->NodeData.NodeType = NodeType;
		GraphNode->NodeData.Title = GetQuestTypeName(NodeType);
		if (!GraphNode->NodeData.NodeId.IsValid())
		{
			GraphNode->NodeData.NodeId = FGuid::NewGuid();
		}

		ParentGraph->AddNode(GraphNode, true, false);
		GraphNode->PostPlacedNewNode();
		GraphNode->AllocateDefaultPins();
		GraphNode->AutowireNewNode(nullptr);
		GraphNode->SnapToGrid(16);
		return GraphNode;
	}

	bool IsOnlyDialogueRootNode(const UDQSDialogueEdGraphNode* Node)
	{
		if (!Node || !Node->GetGraph() || Node->NodeData.NodeType != EDQSDialogueNodeType::Entry)
		{
			return false;
		}

		int32 EntryCount = 0;
		for (const UEdGraphNode* GraphNode : Node->GetGraph()->Nodes)
		{
			const UDQSDialogueEdGraphNode* DialogueNode = Cast<UDQSDialogueEdGraphNode>(GraphNode);
			if (DialogueNode && DialogueNode->NodeData.NodeType == EDQSDialogueNodeType::Entry)
			{
				++EntryCount;
			}
		}

		return EntryCount <= 1;
	}

	bool IsOnlyQuestRootNode(const UDQSQuestEdGraphNode* Node)
	{
		if (!Node || !Node->GetGraph() || Node->NodeData.NodeType != EDQSQuestNodeType::Start)
		{
			return false;
		}

		int32 StartCount = 0;
		for (const UEdGraphNode* GraphNode : Node->GetGraph()->Nodes)
		{
			const UDQSQuestEdGraphNode* QuestNode = Cast<UDQSQuestEdGraphNode>(GraphNode);
			if (QuestNode && QuestNode->NodeData.NodeType == EDQSQuestNodeType::Start)
			{
				++StartCount;
			}
		}

		return StartCount <= 1;
	}

	struct FDialogueSchemaAction_NewNode final : public FEdGraphSchemaAction
	{
		EDQSDialogueNodeType NodeType = EDQSDialogueNodeType::Speech;

		FDialogueSchemaAction_NewNode(const FText& InNodeCategory, const FText& InMenuDesc, const EDQSDialogueNodeType InNodeType)
			: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, FText::GetEmpty(), 0)
			, NodeType(InNodeType)
		{
		}

		virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override
		{
			UDQSDialogueEdGraphNode* Node = CreateDialogueGraphNode(ParentGraph, Location, NodeType);
			if (!Node)
			{
				return nullptr;
			}

			if (FromPin)
			{
				TryAutoconnectNewNode(Node, FromPin);
			}
			if (ParentGraph)
			{
				ParentGraph->NotifyGraphChanged();
			}
			return Node;
		}
	};

	struct FQuestSchemaAction_NewNode final : public FEdGraphSchemaAction
	{
		EDQSQuestNodeType NodeType = EDQSQuestNodeType::Objective;

		FQuestSchemaAction_NewNode(const FText& InNodeCategory, const FText& InMenuDesc, const EDQSQuestNodeType InNodeType)
			: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, FText::GetEmpty(), 0)
			, NodeType(InNodeType)
		{
		}

		virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override
		{
			UDQSQuestEdGraphNode* Node = CreateQuestGraphNode(ParentGraph, Location, NodeType);
			if (!Node)
			{
				return nullptr;
			}

			if (FromPin)
			{
				TryAutoconnectNewNode(Node, FromPin);
			}
			if (ParentGraph)
			{
				ParentGraph->NotifyGraphChanged();
			}
			return Node;
		}
	};
}

void UDQSDialogueEdGraphNode::RefreshPinsAfterDetailsChange()
{
	Modify();
	if (GetGraph())
	{
		GetGraph()->Modify();
	}

	if (NodeData.NodeType == EDQSDialogueNodeType::Choice)
	{
		ResetChoiceIds(NodeData.Choices);
		int32 ChoiceOutputCount = 0;
		for (UEdGraphPin* Pin : Pins)
		{
			if (Pin && Pin->Direction == EGPD_Output)
			{
				++ChoiceOutputCount;
			}
		}

		while (ChoiceOutputCount < NodeData.Choices.Num())
		{
			const int32 ChoiceIndex = ChoiceOutputCount;
			CreatePin(EGPD_Output, FlowPinCategory, NAME_None, nullptr, GetChoicePinName(NodeData.Choices[ChoiceIndex], ChoiceIndex));
			++ChoiceOutputCount;
		}

		while (ChoiceOutputCount > NodeData.Choices.Num())
		{
			for (int32 PinIndex = Pins.Num() - 1; PinIndex >= 0; --PinIndex)
			{
				UEdGraphPin* Pin = Pins[PinIndex];
				if (Pin && Pin->Direction == EGPD_Output)
				{
					Pin->BreakAllPinLinks();
					RemovePin(Pin);
					break;
				}
			}
			--ChoiceOutputCount;
		}

		for (int32 ChoiceIndex = 0, OutputIndex = 0; ChoiceIndex < NodeData.Choices.Num(); ++ChoiceIndex)
		{
			for (; OutputIndex < Pins.Num(); ++OutputIndex)
			{
				UEdGraphPin* Pin = Pins[OutputIndex];
				if (Pin && Pin->Direction == EGPD_Output)
				{
					Pin->PinName = GetChoicePinName(NodeData.Choices[ChoiceIndex], ChoiceIndex);
					++OutputIndex;
					break;
				}
			}
		}

		NodeConnectionListChanged();
		if (GetGraph())
		{
			GetGraph()->NotifyGraphChanged();
		}
		return;
	}

	ReconstructNode();
	if (GetGraph())
	{
		GetGraph()->NotifyGraphChanged();
	}
}

void UDQSDialogueEdGraphNode::AddChoiceOutput()
{
	if (NodeData.NodeType != EDQSDialogueNodeType::Choice)
	{
		return;
	}

	Modify();
	GetGraph()->Modify();

	FDQSDialogueChoice& NewChoice = NodeData.Choices.AddDefaulted_GetRef();
	NewChoice.ChoiceId = FGuid::NewGuid();
	NewChoice.Text = FText::Format(FText::FromString(TEXT("Choice {0}")), NodeData.Choices.Num());
	RefreshPinsAfterDetailsChange();
}

void UDQSDialogueEdGraphNode::RemoveLastChoiceOutput()
{
	if (!CanRemoveChoiceOutput())
	{
		return;
	}

	Modify();
	GetGraph()->Modify();
	NodeData.Choices.RemoveAt(NodeData.Choices.Num() - 1);
	RefreshPinsAfterDetailsChange();
}

bool UDQSDialogueEdGraphNode::CanRemoveChoiceOutput() const
{
	return NodeData.NodeType == EDQSDialogueNodeType::Choice && NodeData.Choices.Num() > 1;
}

void UDQSDialogueEdGraphNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, FlowPinCategory, NAME_None, nullptr, TEXT("In"));

	switch (NodeData.NodeType)
	{
	case EDQSDialogueNodeType::Condition:
		CreatePin(EGPD_Output, FlowPinCategory, NAME_None, nullptr, TEXT("Next Node"));
		CreatePin(EGPD_Output, FlowPinCategory, NAME_None, nullptr, TEXT("Alternative Node (Failed Condition)"));
		break;
	case EDQSDialogueNodeType::ConditionedStart:
		CreatePin(EGPD_Output, FlowPinCategory, NAME_None, nullptr, TEXT("Matched Branch"));
		CreatePin(EGPD_Output, FlowPinCategory, NAME_None, nullptr, TEXT("Fallback Branch"));
		break;
	case EDQSDialogueNodeType::Choice:
		ResetChoiceIds(NodeData.Choices);
		if (NodeData.Choices.IsEmpty())
		{
			NodeData.Choices.SetNum(2);
			ResetChoiceIds(NodeData.Choices);
		}
		for (int32 Index = 0; Index < NodeData.Choices.Num(); ++Index)
		{
			CreatePin(EGPD_Output, FlowPinCategory, NAME_None, nullptr, GetChoicePinName(NodeData.Choices[Index], Index));
		}
		break;
	case EDQSDialogueNodeType::End:
		break;
	default:
		CreatePin(EGPD_Output, FlowPinCategory, NAME_None, nullptr, TEXT("Next"));
		break;
	}
}

FText UDQSDialogueEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return NodeData.Title.IsEmpty() ? GetDialogueTypeName(NodeData.NodeType) : NodeData.Title;
}

FText UDQSDialogueEdGraphNode::GetTooltipText() const
{
	const FDQSNodeHelpEntry Help = FDQSGraphEditorUtils::GetDialogueNodeHelp(NodeData.NodeType);
	return FText::Format(FText::FromString(TEXT("{0}\n\nUse when: {1}\nCommon beginner mistake: {2}\nExample: {3}")), Help.Purpose, Help.UseWhen, Help.BeginnerMistake, Help.Example);
}

FLinearColor UDQSDialogueEdGraphNode::GetNodeTitleColor() const
{
	switch (NodeData.NodeType)
	{
	case EDQSDialogueNodeType::Speech:
		return FLinearColor(0.82f, 0.53f, 0.18f);
	case EDQSDialogueNodeType::Choice:
		return FLinearColor(0.18f, 0.45f, 0.82f);
	case EDQSDialogueNodeType::QuestHook:
		return FLinearColor(0.25f, 0.72f, 0.41f);
	default:
		return FLinearColor(0.35f, 0.35f, 0.35f);
	}
}

void UDQSDialogueEdGraphNode::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();
	if (!NodeData.NodeId.IsValid())
	{
		NodeData.NodeId = FGuid::NewGuid();
	}
}

void UDQSDialogueEdGraphNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (NodeData.NodeType == EDQSDialogueNodeType::Choice && IsChoiceArrayEdit(PropertyChangedEvent))
	{
		RefreshPinsAfterDetailsChange();
		return;
	}
	ReconstructNode();
	if (GetGraph())
	{
		GetGraph()->NotifyGraphChanged();
	}
}

void UDQSDialogueEdGraphNode::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	if (NodeData.NodeType != EDQSDialogueNodeType::Choice)
	{
		return;
	}

	bool bChangedChoices = IsChoiceArrayEdit(PropertyChangedEvent);
	for (const FEditPropertyChain::TDoubleLinkedListNode* Node = PropertyChangedEvent.PropertyChain.GetHead(); Node; Node = Node->GetNextNode())
	{
		const FProperty* Property = Node->GetValue();
		if (Property && Property->GetFName() == GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, Choices))
		{
			bChangedChoices = true;
			break;
		}
	}

	if (!bChangedChoices)
	{
		return;
	}

	RefreshPinsAfterDetailsChange();
}

bool UDQSDialogueEdGraphNode::CanUserDeleteNode() const
{
	return NodeData.NodeType != EDQSDialogueNodeType::Entry || !IsOnlyDialogueRootNode(this);
}

bool UDQSDialogueEdGraphNode::CanDuplicateNode() const
{
	return NodeData.NodeType != EDQSDialogueNodeType::Entry;
}

bool UDQSDialogueEdGraphNode::CanEditChange(const FProperty* InProperty) const
{
	if (!Super::CanEditChange(InProperty) || !InProperty)
	{
		return false;
	}

	const FName PropertyName = InProperty->GetFName();
	return PropertyName != GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, NodeType)
		&& PropertyName != GET_MEMBER_NAME_CHECKED(FDQSDialogueNode, NodeId);
}

void UDQSQuestEdGraphNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, FlowPinCategory, NAME_None, nullptr, TEXT("In"));

	switch (NodeData.NodeType)
	{
	case EDQSQuestNodeType::Condition:
	case EDQSQuestNodeType::Branch:
		CreatePin(EGPD_Output, FlowPinCategory, NAME_None, nullptr, TEXT("Next Node"));
		CreatePin(EGPD_Output, FlowPinCategory, NAME_None, nullptr, TEXT("Alternative Node"));
		break;
	case EDQSQuestNodeType::Complete:
	case EDQSQuestNodeType::Fail:
		break;
	default:
		CreatePin(EGPD_Output, FlowPinCategory, NAME_None, nullptr, TEXT("Next"));
		break;
	}
}

FText UDQSQuestEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return NodeData.Title.IsEmpty() ? GetQuestTypeName(NodeData.NodeType) : NodeData.Title;
}

FText UDQSQuestEdGraphNode::GetTooltipText() const
{
	const FDQSNodeHelpEntry Help = FDQSGraphEditorUtils::GetQuestNodeHelp(NodeData.NodeType);
	return FText::Format(FText::FromString(TEXT("{0}\n\nUse when: {1}\nCommon beginner mistake: {2}\nExample: {3}")), Help.Purpose, Help.UseWhen, Help.BeginnerMistake, Help.Example);
}

FLinearColor UDQSQuestEdGraphNode::GetNodeTitleColor() const
{
	switch (NodeData.NodeType)
	{
	case EDQSQuestNodeType::Objective:
		return FLinearColor(0.15f, 0.62f, 0.28f);
	case EDQSQuestNodeType::Fail:
		return FLinearColor(0.75f, 0.2f, 0.2f);
	case EDQSQuestNodeType::Complete:
		return FLinearColor(0.2f, 0.5f, 0.78f);
	default:
		return FLinearColor(0.35f, 0.35f, 0.35f);
	}
}

void UDQSQuestEdGraphNode::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();
	if (!NodeData.NodeId.IsValid())
	{
		NodeData.NodeId = FGuid::NewGuid();
	}
}

void UDQSQuestEdGraphNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ReconstructNode();
	if (GetGraph())
	{
		GetGraph()->NotifyGraphChanged();
	}
}

bool UDQSQuestEdGraphNode::CanUserDeleteNode() const
{
	return NodeData.NodeType != EDQSQuestNodeType::Start || !IsOnlyQuestRootNode(this);
}

bool UDQSQuestEdGraphNode::CanDuplicateNode() const
{
	return NodeData.NodeType != EDQSQuestNodeType::Start;
}

bool UDQSQuestEdGraphNode::CanEditChange(const FProperty* InProperty) const
{
	if (!Super::CanEditChange(InProperty) || !InProperty)
	{
		return false;
	}

	const FName PropertyName = InProperty->GetFName();
	return PropertyName != GET_MEMBER_NAME_CHECKED(FDQSQuestNode, NodeType)
		&& PropertyName != GET_MEMBER_NAME_CHECKED(FDQSQuestNode, NodeId);
}

void UDQSDialogueGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const bool bSimpleMode = FDQSGraphEditorUtils::IsSimpleMode(ContextMenuBuilder.CurrentGraph ? ContextMenuBuilder.CurrentGraph->GetOuter() : nullptr);
	for (int32 Value = 0; Value <= static_cast<int32>(EDQSDialogueNodeType::ConditionedStart); ++Value)
	{
		const EDQSDialogueNodeType NodeType = static_cast<EDQSDialogueNodeType>(Value);
		if (NodeType == EDQSDialogueNodeType::Entry && HasDialogueNodeType(ContextMenuBuilder.CurrentGraph, EDQSDialogueNodeType::Entry))
		{
			continue;
		}

		if (NodeType == EDQSDialogueNodeType::Condition)
		{
			continue;
		}

		if (bSimpleMode && NodeType == EDQSDialogueNodeType::Jump)
		{
			continue;
		}

		ContextMenuBuilder.AddAction(MakeShared<FDialogueSchemaAction_NewNode>(GetDialoguePaletteCategory(NodeType), GetDialogueTypeName(NodeType), NodeType));
	}
}

void UDQSDialogueGraphSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	if (Context && Context->Node)
	{
		FToolMenuSection& Section = Menu->AddSection("DQSDialogueNodeActions", FText::FromString(TEXT("Node Actions")));
		Section.AddMenuEntry(FGenericCommands::Get().Delete);
		Section.AddMenuEntry(FGenericCommands::Get().Cut);
		Section.AddMenuEntry(FGenericCommands::Get().Copy);
		Section.AddMenuEntry(FGenericCommands::Get().Duplicate);
		Section.AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);

		if (UDQSDialogueEdGraphNode* DialogueNode = Cast<UDQSDialogueEdGraphNode>(const_cast<UEdGraphNode*>(Context->Node.Get())))
		{
			if (DialogueNode->NodeData.NodeType == EDQSDialogueNodeType::Choice)
			{
				TWeakObjectPtr<UDQSDialogueEdGraphNode> WeakNode = DialogueNode;

				Section.AddMenuEntry(
					"DQSDialogueAddChoiceOutput",
					FText::FromString(TEXT("Add Choice Output")),
					FText::FromString(TEXT("Add another response option and output pin to this choice node.")),
					FSlateIcon(),
					FUIAction(FExecuteAction::CreateLambda([WeakNode]()
					{
						if (UDQSDialogueEdGraphNode* Node = WeakNode.Get())
						{
							const FScopedTransaction Transaction(FText::FromString(TEXT("Add Choice Output")));
							Node->AddChoiceOutput();
						}
					})));

				Section.AddMenuEntry(
					"DQSDialogueRemoveChoiceOutput",
					FText::FromString(TEXT("Remove Last Choice Output")),
					FText::FromString(TEXT("Remove the last response option and its output pin from this choice node.")),
					FSlateIcon(),
					FUIAction(
						FExecuteAction::CreateLambda([WeakNode]()
						{
							if (UDQSDialogueEdGraphNode* Node = WeakNode.Get())
							{
								const FScopedTransaction Transaction(FText::FromString(TEXT("Remove Choice Output")));
								Node->RemoveLastChoiceOutput();
							}
						}),
						FCanExecuteAction::CreateLambda([WeakNode]()
						{
							if (const UDQSDialogueEdGraphNode* Node = WeakNode.Get())
							{
								return Node->CanRemoveChoiceOutput();
							}
							return false;
						})));
			}
		}
	}

	Super::GetContextMenuActions(Menu, Context);
}

const FPinConnectionResponse UDQSDialogueGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if (!A || !B || A == B || A->Direction == B->Direction || A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Invalid connection"));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

void UDQSDialogueGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	if (!HasDialogueNodeType(&Graph, EDQSDialogueNodeType::Entry))
	{
		CreateDialogueGraphNode(&Graph, FVector2D(0.0f, 0.0f), EDQSDialogueNodeType::Entry);
		Graph.NotifyGraphChanged();
	}
}

void UDQSQuestGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const bool bSimpleMode = FDQSGraphEditorUtils::IsSimpleMode(ContextMenuBuilder.CurrentGraph ? ContextMenuBuilder.CurrentGraph->GetOuter() : nullptr);
	for (int32 Value = 0; Value <= static_cast<int32>(EDQSQuestNodeType::Subquest); ++Value)
	{
		const EDQSQuestNodeType NodeType = static_cast<EDQSQuestNodeType>(Value);
		if (NodeType == EDQSQuestNodeType::Start && HasQuestNodeType(ContextMenuBuilder.CurrentGraph, EDQSQuestNodeType::Start))
		{
			continue;
		}

		if (bSimpleMode && (NodeType == EDQSQuestNodeType::Condition || NodeType == EDQSQuestNodeType::Subquest))
		{
			continue;
		}

		ContextMenuBuilder.AddAction(MakeShared<FQuestSchemaAction_NewNode>(GetQuestPaletteCategory(NodeType), GetQuestTypeName(NodeType), NodeType));
	}
}

void UDQSQuestGraphSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	if (Context && Context->Node)
	{
		FToolMenuSection& Section = Menu->AddSection("DQSQuestNodeActions", FText::FromString(TEXT("Node Actions")));
		Section.AddMenuEntry(FGenericCommands::Get().Delete);
		Section.AddMenuEntry(FGenericCommands::Get().Cut);
		Section.AddMenuEntry(FGenericCommands::Get().Copy);
		Section.AddMenuEntry(FGenericCommands::Get().Duplicate);
		Section.AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);
	}

	Super::GetContextMenuActions(Menu, Context);
}

const FPinConnectionResponse UDQSQuestGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if (!A || !B || A == B || A->Direction == B->Direction || A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Invalid connection"));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

void UDQSQuestGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	if (!HasQuestNodeType(&Graph, EDQSQuestNodeType::Start))
	{
		CreateQuestGraphNode(&Graph, FVector2D(0.0f, 0.0f), EDQSQuestNodeType::Start);
		Graph.NotifyGraphChanged();
	}
}








