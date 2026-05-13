#include "DQSQuestGraphAsset.h"

UQuestGraphAsset::UQuestGraphAsset()
{
	Metadata.GraphId = *FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens);
}

void UQuestGraphAsset::PostLoad()
{
	Super::PostLoad();
	NormalizeForRuntime();
}

const FDQSQuestNode* UQuestGraphAsset::FindNodeById(const FGuid& NodeId) const
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

void UQuestGraphAsset::InvalidateNodeLookupCache() const
{
	NodeLookupCache.Reset();
}

void UQuestGraphAsset::NormalizeForRuntime()
{
	bool bChanged = false;
	bool bHasAnyRuntimeLink = false;

	for (FDQSQuestNode& Node : Nodes)
	{
		if (!Node.NodeId.IsValid())
		{
			Node.NodeId = FGuid::NewGuid();
			bChanged = true;
		}

		bHasAnyRuntimeLink |= Node.NextNodeId.IsValid() || Node.AlternateNodeId.IsValid();
	}

	if (!EntryNodeId.IsValid())
	{
		if (const FDQSQuestNode* StartNode = Nodes.FindByPredicate([](const FDQSQuestNode& Node)
		{
			return Node.NodeType == EDQSQuestNodeType::Start;
		}))
		{
			EntryNodeId = StartNode->NodeId;
			bChanged = true;
		}
	}

	if (!bHasAnyRuntimeLink && Nodes.Num() > 1)
	{
		for (int32 Index = 0; Index < Nodes.Num() - 1; ++Index)
		{
			FDQSQuestNode& Node = Nodes[Index];
			if (Node.NodeType != EDQSQuestNodeType::Complete && Node.NodeType != EDQSQuestNodeType::Fail)
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

void UQuestGraphAsset::RebuildNodeLookupCache() const
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
