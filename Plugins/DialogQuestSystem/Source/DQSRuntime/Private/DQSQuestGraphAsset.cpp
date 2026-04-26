#include "DQSQuestGraphAsset.h"

UQuestGraphAsset::UQuestGraphAsset()
{
	Metadata.GraphId = *FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens);
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
