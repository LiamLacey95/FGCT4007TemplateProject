#include "DQSGraphCompiler.h"

#include "DQSEdGraphNodes.h"
#include "DQSDialogueGraphAsset.h"
#include "DQSQuestGraphAsset.h"

namespace
{
	template <typename NodeType>
	NodeType* GetLinkedNode(const UEdGraphPin* Pin)
	{
		if (!Pin || Pin->LinkedTo.IsEmpty())
		{
			return nullptr;
		}

		return Cast<NodeType>(Pin->LinkedTo[0]->GetOwningNode());
	}

	template <typename NodeStructType>
	void CollectReachable(const TMap<FGuid, TArray<FGuid>>& Adjacency, const FGuid& NodeId, TSet<FGuid>& Reachable)
	{
		if (!NodeId.IsValid() || Reachable.Contains(NodeId))
		{
			return;
		}

		Reachable.Add(NodeId);
		if (const TArray<FGuid>* Targets = Adjacency.Find(NodeId))
		{
			for (const FGuid& TargetId : *Targets)
			{
				CollectReachable<NodeStructType>(Adjacency, TargetId, Reachable);
			}
		}
	}
}

void FDQSGraphCompiler::CompileDialogueAsset(UDialogueGraphAsset* Asset)
{
	if (!Asset || !Asset->EditorGraph)
	{
		return;
	}

	Asset->Nodes.Reset();
	Asset->EntryNodeId.Invalidate();
	Asset->InvalidateNodeLookupCache();

	for (UEdGraphNode* GraphNode : Asset->EditorGraph->Nodes)
	{
		UDQSDialogueEdGraphNode* DialogueNode = Cast<UDQSDialogueEdGraphNode>(GraphNode);
		if (!DialogueNode)
		{
			continue;
		}

		FDQSDialogueNode NodeData = DialogueNode->NodeData;
		if (!NodeData.NodeId.IsValid())
		{
			NodeData.NodeId = FGuid::NewGuid();
			DialogueNode->NodeData.NodeId = NodeData.NodeId;
		}

		if (NodeData.NodeType == EDQSDialogueNodeType::Condition)
		{
			NodeData.NodeType = EDQSDialogueNodeType::ConditionedStart;
			DialogueNode->NodeData.NodeType = EDQSDialogueNodeType::ConditionedStart;
		}

		bool bClearedDeprecatedChoiceData = false;
		for (FDQSDialogueChoice& Choice : NodeData.Choices)
		{
			if (!Choice.Conditions.IsEmpty() || !Choice.Actions.IsEmpty())
			{
				Choice.Conditions.Reset();
				Choice.Actions.Reset();
				bClearedDeprecatedChoiceData = true;
			}
		}
		if (bClearedDeprecatedChoiceData)
		{
			DialogueNode->NodeData.Choices = NodeData.Choices;
		}

		const TArray<UEdGraphPin*>& Pins = DialogueNode->Pins;
		TArray<UEdGraphPin*> OutputPins;
		for (UEdGraphPin* Pin : Pins)
		{
			if (Pin && Pin->Direction == EGPD_Output)
			{
				OutputPins.Add(Pin);
			}
		}

		if (NodeData.NodeType == EDQSDialogueNodeType::Choice)
		{
			for (int32 Index = 0; Index < NodeData.Choices.Num() && Index < OutputPins.Num(); ++Index)
			{
				if (const UDQSDialogueEdGraphNode* TargetNode = GetLinkedNode<UDQSDialogueEdGraphNode>(OutputPins[Index]))
				{
					NodeData.Choices[Index].TargetNodeId = TargetNode->NodeData.NodeId;
				}
			}
		}
		else if (NodeData.NodeType == EDQSDialogueNodeType::Condition || NodeData.NodeType == EDQSDialogueNodeType::ConditionedStart)
		{
			NodeData.NextNodeId = OutputPins.Num() > 0 && GetLinkedNode<UDQSDialogueEdGraphNode>(OutputPins[0]) ? GetLinkedNode<UDQSDialogueEdGraphNode>(OutputPins[0])->NodeData.NodeId : FGuid();
			NodeData.AlternateNodeId = OutputPins.Num() > 1 && GetLinkedNode<UDQSDialogueEdGraphNode>(OutputPins[1]) ? GetLinkedNode<UDQSDialogueEdGraphNode>(OutputPins[1])->NodeData.NodeId : FGuid();
		}
		else if (NodeData.NodeType == EDQSDialogueNodeType::Jump)
		{
			NodeData.JumpTargetNodeId = OutputPins.Num() > 0 && GetLinkedNode<UDQSDialogueEdGraphNode>(OutputPins[0]) ? GetLinkedNode<UDQSDialogueEdGraphNode>(OutputPins[0])->NodeData.NodeId : FGuid();
		}
		else
		{
			NodeData.NextNodeId = OutputPins.Num() > 0 && GetLinkedNode<UDQSDialogueEdGraphNode>(OutputPins[0]) ? GetLinkedNode<UDQSDialogueEdGraphNode>(OutputPins[0])->NodeData.NodeId : FGuid();
		}

		if (NodeData.NodeType == EDQSDialogueNodeType::Entry)
		{
			Asset->EntryNodeId = NodeData.NodeId;
		}

		Asset->Nodes.Add(NodeData);
	}

	Asset->InvalidateNodeLookupCache();
}

void FDQSGraphCompiler::CompileQuestAsset(UQuestGraphAsset* Asset)
{
	if (!Asset || !Asset->EditorGraph)
	{
		return;
	}

	Asset->Nodes.Reset();
	Asset->EntryNodeId.Invalidate();
	Asset->InvalidateNodeLookupCache();

	for (UEdGraphNode* GraphNode : Asset->EditorGraph->Nodes)
	{
		UDQSQuestEdGraphNode* QuestNode = Cast<UDQSQuestEdGraphNode>(GraphNode);
		if (!QuestNode)
		{
			continue;
		}

		FDQSQuestNode NodeData = QuestNode->NodeData;
		if (!NodeData.NodeId.IsValid())
		{
			NodeData.NodeId = FGuid::NewGuid();
			QuestNode->NodeData.NodeId = NodeData.NodeId;
		}

		TArray<UEdGraphPin*> OutputPins;
		for (UEdGraphPin* Pin : QuestNode->Pins)
		{
			if (Pin && Pin->Direction == EGPD_Output)
			{
				OutputPins.Add(Pin);
			}
		}

		if (NodeData.NodeType == EDQSQuestNodeType::Condition || NodeData.NodeType == EDQSQuestNodeType::Branch)
		{
			NodeData.NextNodeId = OutputPins.Num() > 0 && GetLinkedNode<UDQSQuestEdGraphNode>(OutputPins[0]) ? GetLinkedNode<UDQSQuestEdGraphNode>(OutputPins[0])->NodeData.NodeId : FGuid();
			NodeData.AlternateNodeId = OutputPins.Num() > 1 && GetLinkedNode<UDQSQuestEdGraphNode>(OutputPins[1]) ? GetLinkedNode<UDQSQuestEdGraphNode>(OutputPins[1])->NodeData.NodeId : FGuid();
		}
		else
		{
			NodeData.NextNodeId = OutputPins.Num() > 0 && GetLinkedNode<UDQSQuestEdGraphNode>(OutputPins[0]) ? GetLinkedNode<UDQSQuestEdGraphNode>(OutputPins[0])->NodeData.NodeId : FGuid();
		}

		if (NodeData.NodeType == EDQSQuestNodeType::Start)
		{
			Asset->EntryNodeId = NodeData.NodeId;
		}

		Asset->Nodes.Add(NodeData);
	}

	Asset->InvalidateNodeLookupCache();
}

void FDQSGraphCompiler::ValidateDialogueAsset(UDialogueGraphAsset* Asset, TArray<FText>& OutMessages)
{
	CompileDialogueAsset(Asset);
	OutMessages.Reset();

	if (!Asset)
	{
		OutMessages.Add(FText::FromString(TEXT("Dialogue asset is null.")));
		return;
	}

	if (!Asset->EntryNodeId.IsValid())
	{
		OutMessages.Add(FText::FromString(TEXT("Dialogue graph is missing an Entry node.")));
	}

	TMap<FGuid, TArray<FGuid>> Adjacency;
	for (const FDQSDialogueNode& Node : Asset->Nodes)
	{
		TArray<FGuid>& Links = Adjacency.FindOrAdd(Node.NodeId);
		if (Node.NextNodeId.IsValid())
		{
			Links.Add(Node.NextNodeId);
		}
		if (Node.AlternateNodeId.IsValid())
		{
			Links.Add(Node.AlternateNodeId);
		}
		if (Node.JumpTargetNodeId.IsValid())
		{
			Links.Add(Node.JumpTargetNodeId);
		}
		for (const FDQSDialogueChoice& Choice : Node.Choices)
		{
			if (Choice.TargetNodeId.IsValid())
			{
				Links.Add(Choice.TargetNodeId);
			}
		}
	}

	TSet<FGuid> Reachable;
	CollectReachable<FDQSDialogueNode>(Adjacency, Asset->EntryNodeId, Reachable);
	for (const FDQSDialogueNode& Node : Asset->Nodes)
	{
		if (!Reachable.Contains(Node.NodeId))
		{
			OutMessages.Add(FText::Format(FText::FromString(TEXT("Node '{0}' is unreachable.")), Node.Title));
		}
	}

	Asset->Metadata.bLastValidationSucceeded = OutMessages.IsEmpty();
	Asset->Metadata.LastValidationSummary = OutMessages.IsEmpty() ? FText::FromString(TEXT("Dialogue graph validation passed.")) : OutMessages[0];
}

void FDQSGraphCompiler::ValidateQuestAsset(UQuestGraphAsset* Asset, TArray<FText>& OutMessages)
{
	CompileQuestAsset(Asset);
	OutMessages.Reset();

	if (!Asset)
	{
		OutMessages.Add(FText::FromString(TEXT("Quest asset is null.")));
		return;
	}

	if (!Asset->EntryNodeId.IsValid())
	{
		OutMessages.Add(FText::FromString(TEXT("Quest graph is missing a Start node.")));
	}

	TMap<FGuid, TArray<FGuid>> Adjacency;
	for (const FDQSQuestNode& Node : Asset->Nodes)
	{
		TArray<FGuid>& Links = Adjacency.FindOrAdd(Node.NodeId);
		if (Node.NextNodeId.IsValid())
		{
			Links.Add(Node.NextNodeId);
		}
		if (Node.AlternateNodeId.IsValid())
		{
			Links.Add(Node.AlternateNodeId);
		}
	}

	TSet<FGuid> Reachable;
	CollectReachable<FDQSQuestNode>(Adjacency, Asset->EntryNodeId, Reachable);
	for (const FDQSQuestNode& Node : Asset->Nodes)
	{
		if (!Reachable.Contains(Node.NodeId))
		{
			OutMessages.Add(FText::Format(FText::FromString(TEXT("Node '{0}' is unreachable.")), Node.Title));
		}
	}

	Asset->Metadata.bLastValidationSucceeded = OutMessages.IsEmpty();
	Asset->Metadata.LastValidationSummary = OutMessages.IsEmpty() ? FText::FromString(TEXT("Quest graph validation passed.")) : OutMessages[0];
}

