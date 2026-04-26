#pragma once

#include "Engine/DataAsset.h"
#include "DQSTypes.h"
#include "DQSDialogueGraphAsset.generated.h"

class UEdGraph;
class UTexture2D;
class UDQSSpeakerStyleAsset;

UCLASS(BlueprintType)
class DQSRUNTIME_API UDialogueGraphAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UDialogueGraphAsset();
	virtual void PostLoad() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FDQSGraphMetadata Metadata;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	TArray<FDQSSpeakerDefinition> Speakers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FGuid EntryNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	bool bAllowResume = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	TArray<FDQSDialogueNode> Nodes;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UEdGraph> EditorGraph;

	UPROPERTY(EditAnywhere, Category = "Dialog Quest System|Editor")
	bool bUseSimpleEditorMode = true;

	UPROPERTY(EditAnywhere, Category = "Dialog Quest System|Editor")
	bool bShowGuidedHelp = true;
#endif

	const FDQSDialogueNode* FindNodeById(const FGuid& NodeId) const;
	const FDQSSpeakerDefinition* FindSpeakerDefinition(FName SpeakerId) const;
	void InvalidateNodeLookupCache() const;
	void NormalizeForRuntime();

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	FText ResolveSpeakerName(FName SpeakerId) const;

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	TSoftObjectPtr<UTexture2D> ResolveSpeakerPortrait(FName SpeakerId) const;

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System")
	TSoftObjectPtr<UDQSSpeakerStyleAsset> ResolveSpeakerStyle(FName SpeakerId) const;

private:
	void RebuildNodeLookupCache() const;

	mutable TMap<FGuid, int32> NodeLookupCache;
};
