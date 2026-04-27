#pragma once

#include "CoreMinimal.h"
#include "DQSGraphEditorUtils.h"
#include "Toolkits/AssetEditorToolkit.h"

class IDetailsView;
class SButton;
class SDockTab;
class SScrollBox;
class SGraphEditor;
class STextBlock;
class UEdGraph;
class UEdGraphNode;
class UDialogueGraphAsset;
class UQuestGraphAsset;

class DQSEDITOR_API FDQSAssetEditorToolkit : public FAssetEditorToolkit, public FGCObject
{
public:
	void InitDialogueEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UDialogueGraphAsset* Asset);
	void InitQuestEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UQuestGraphAsset* Asset);

	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;
	virtual void SaveAsset_Execute() override;

private:
	void ExtendToolbar();
	void FillToolbar(class FToolBarBuilder& ToolbarBuilder);
	void InitializeCommon(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* Asset, const FText& Title);
	void BindEditorCommands();
	TSharedRef<class SDockTab> SpawnGraphTab(const class FSpawnTabArgs& Args);
	TSharedRef<class SDockTab> SpawnDetailsTab(const class FSpawnTabArgs& Args);
	TSharedRef<class SDockTab> SpawnHelpTab(const class FSpawnTabArgs& Args);
	TSharedRef<class SDockTab> SpawnValidationTab(const class FSpawnTabArgs& Args);
	TSharedRef<class SDockTab> SpawnSimulationTab(const class FSpawnTabArgs& Args);
	void HandleSelectionChanged(const TSet<UObject*>& NewSelection);
	void HandleFinishedChangingDetails(const FPropertyChangedEvent& PropertyChangedEvent);
	void CompileEditedAsset() const;
	void ValidateEditedAsset();
	void CopySelectedNodes() const;
	bool CanCopySelectedNodes() const;
	void CutSelectedNodes();
	void PasteNodes();
	bool CanPasteNodes() const;
	void DuplicateSelectedNodes();
	void DeleteSelectedNodes();
	bool CanDeleteSelectedNodes() const;
	void SetSimpleMode(bool bUseSimpleMode);
	bool IsSimpleMode() const;
	void ToggleGuidedHelp();
	bool IsGuidedHelpEnabled() const;
	void InsertStarterTemplate();
	void ApplyFirstFixAction();
	void ApplyFixForIssue(FDQSValidationIssue Issue);
	void RefreshPanels();
	void RefreshHelpPanel();
	void RefreshValidationPanel();
	void RefreshSimulationPanel();
	void RefreshToolbarStatus();
	void UpdateGuideTabVisibility();
	void HighlightFirstUnfinishedTask();
	FText BuildGuidanceBannerText() const;
	FText BuildStatusSummaryText() const;
	FText BuildReadinessSummaryText() const;
	int32 GetBlockingIssueCount() const;
	int32 GetWarningIssueCount() const;
	UEdGraphNode* FindNodeByGuid(const FGuid& NodeId) const;
	void JumpToIssueNode(const FDQSValidationIssue& Issue) const;
	void StartSimulation();
	void ResetSimulation();
	void StepSimulationPrimary();
	void StepSimulationAlternate();
	void AdvanceSimulation(bool bUseAlternatePath);
	void UpdateSimulationActionLabels();

	UObject* GetEditedAsset() const { return EditingAsset; }

	TObjectPtr<UObject> EditingAsset = nullptr;
	TObjectPtr<UEdGraph> EditingGraph = nullptr;
	TSharedPtr<IDetailsView> DetailsView;
	TSharedPtr<SGraphEditor> GraphEditor;
	TSharedPtr<class FUICommandList> GraphEditorCommands;
	TSharedPtr<STextBlock> GuidanceBannerText;
	TSharedPtr<STextBlock> StatusSummaryText;
	TSharedPtr<SScrollBox> HelpScrollBox;
	TSharedPtr<SScrollBox> ValidationScrollBox;
	TSharedPtr<STextBlock> SimulationStatusText;
	TSharedPtr<STextBlock> SimulationPrimaryButtonText;
	TSharedPtr<STextBlock> SimulationAlternateButtonText;
	FText ToolkitTitle;
	TArray<FDQSValidationIssue> ValidationIssues;
	TArray<FDQSChecklistItem> ChecklistItems;
	FGuid SimulatedNodeId;
};
