#include "DQSDialogueGraphAsset.h"

UDialogueGraphAsset::UDialogueGraphAsset()
{
	Metadata.GraphId = *FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens);
}

void UDialogueGraphAsset::PostLoad()
{
	Super::PostLoad();
	NormalizeForRuntime();
}

const FDQSDialogueNode* UDialogueGraphAsset::FindNodeById(const FGuid& NodeId) const
{
	if (!NodeId.IsValid())
	{
		return nullptr;
	}

	if (NodeLookupCache.Num() != Nodes.Num() || !NodeLookupCache.Contains(NodeId))
	{
		RebuildNodeLookupCache();
	}

	if (const int32* NodeIndex = NodeLookupCache.Find(NodeId))
	{
		return Nodes.IsValidIndex(*NodeIndex) ? &Nodes[*NodeIndex] : nullptr;
	}

	return nullptr;
}

const FDQSSpeakerDefinition* UDialogueGraphAsset::FindSpeakerDefinition(const FName SpeakerId) const
{
	return Speakers.FindByPredicate([SpeakerId](const FDQSSpeakerDefinition& Entry)
	{
		return Entry.SpeakerId == SpeakerId;
	});
}

FText UDialogueGraphAsset::ResolveSpeakerName(FName SpeakerId) const
{
	if (const FDQSSpeakerDefinition* Speaker = FindSpeakerDefinition(SpeakerId))
	{
		return Speaker->DisplayName;
	}

	return FText::FromName(SpeakerId);
}

TSoftObjectPtr<UTexture2D> UDialogueGraphAsset::ResolveSpeakerPortrait(const FName SpeakerId) const
{
	if (const FDQSSpeakerDefinition* Speaker = FindSpeakerDefinition(SpeakerId))
	{
		return Speaker->Portrait;
	}

	return nullptr;
}

TSoftObjectPtr<UDQSSpeakerStyleAsset> UDialogueGraphAsset::ResolveSpeakerStyle(const FName SpeakerId) const
{
	if (const FDQSSpeakerDefinition* Speaker = FindSpeakerDefinition(SpeakerId))
	{
		return Speaker->SpeakerStyle;
	}

	return nullptr;
}

void UDialogueGraphAsset::InvalidateNodeLookupCache() const
{
	NodeLookupCache.Reset();
}

void UDialogueGraphAsset::NormalizeForRuntime()
{
	bool bChanged = false;
	bool bHasAnyRuntimeLink = false;

	for (FDQSDialogueNode& Node : Nodes)
	{
		if (!Node.NodeId.IsValid())
		{
			Node.NodeId = FGuid::NewGuid();
			bChanged = true;
		}

		if (Node.NodeType == EDQSDialogueNodeType::Condition)
		{
			Node.NodeType = EDQSDialogueNodeType::ConditionedStart;
			bChanged = true;
		}

		for (FDQSDialogueChoice& Choice : Node.Choices)
		{
			if (!Choice.ChoiceId.IsValid())
			{
				Choice.ChoiceId = FGuid::NewGuid();
				bChanged = true;
			}

			if (!Choice.Conditions.IsEmpty())
			{
				Choice.Conditions.Reset();
				bChanged = true;
			}

			if (!Choice.Actions.IsEmpty())
			{
				Choice.Actions.Reset();
				bChanged = true;
			}
		}

		bHasAnyRuntimeLink |= Node.NextNodeId.IsValid()
			|| Node.AlternateNodeId.IsValid()
			|| Node.JumpTargetNodeId.IsValid();
		for (const FDQSDialogueChoice& Choice : Node.Choices)
		{
			bHasAnyRuntimeLink |= Choice.TargetNodeId.IsValid();
		}
	}

	if (!EntryNodeId.IsValid())
	{
		if (const FDQSDialogueNode* EntryNode = Nodes.FindByPredicate([](const FDQSDialogueNode& Node)
		{
			return Node.NodeType == EDQSDialogueNodeType::Entry;
		}))
		{
			EntryNodeId = EntryNode->NodeId;
			bChanged = true;
		}
	}

	// Early starter assets created before graph compilation may contain nodes but no saved links.
	// Keep that specific case usable by connecting a simple linear flow in array order.
	if (!bHasAnyRuntimeLink && Nodes.Num() > 1)
	{
		for (int32 Index = 0; Index < Nodes.Num() - 1; ++Index)
		{
			FDQSDialogueNode& Node = Nodes[Index];
			if (Node.NodeType != EDQSDialogueNodeType::End && Node.NodeType != EDQSDialogueNodeType::Choice)
			{
				Node.NextNodeId = Nodes[Index + 1].NodeId;
				bChanged = true;
			}
		}
	}

	if (bChanged)
	{
		InvalidateNodeLookupCache();
	}
}

void UDialogueGraphAsset::RebuildNodeLookupCache() const
{
	NodeLookupCache.Reset();
	for (int32 Index = 0; Index < Nodes.Num(); ++Index)
	{
		if (Nodes[Index].NodeId.IsValid())
		{
			NodeLookupCache.Add(Nodes[Index].NodeId, Index);
		}
	}
}
