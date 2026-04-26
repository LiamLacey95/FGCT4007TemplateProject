#pragma once

#include "Engine/DataAsset.h"
#include "DQSTypes.h"
#include "DQSQuestGraphAsset.generated.h"

class UEdGraph;

UCLASS(BlueprintType)
class DQSRUNTIME_API UQuestGraphAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UQuestGraphAsset();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FDQSGraphMetadata Metadata;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FGuid EntryNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	TArray<FDQSQuestNode> Nodes;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UEdGraph> EditorGraph;

	UPROPERTY(EditAnywhere, Category = "Dialog Quest System|Editor")
	bool bUseSimpleEditorMode = true;

	UPROPERTY(EditAnywhere, Category = "Dialog Quest System|Editor")
	bool bShowGuidedHelp = true;
#endif

	const FDQSQuestNode* FindNodeById(const FGuid& NodeId) const;
	void InvalidateNodeLookupCache() const;

private:
	void RebuildNodeLookupCache() const;

	mutable TMap<FGuid, int32> NodeLookupCache;
};
