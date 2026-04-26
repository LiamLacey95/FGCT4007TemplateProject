#pragma once

#include "CoreMinimal.h"
#include "DQSGraphEditorUtils.h"

class SScrollBox;
class STextBlock;
class UEdGraphNode;

namespace DQSAssetEditorPanelUtils
{
	void PopulateHelpPanel(
		const TSharedPtr<SScrollBox>& HelpScrollBox,
		const TArray<FDQSChecklistItem>& ChecklistItems,
		const TOptional<FDQSNodeHelpEntry>& SelectedNodeHelp);

	void PopulateValidationPanel(
		const TSharedPtr<SScrollBox>& ValidationScrollBox,
		const TArray<FDQSValidationIssue>& ValidationIssues,
		const FText& ReadinessSummary,
		int32 BlockingIssueCount,
		int32 WarningIssueCount,
		TFunctionRef<void(const FDQSValidationIssue&)> OnJumpToIssue,
		TFunctionRef<void(const FDQSValidationIssue&)> OnFixIssue);

	FText BuildSimulationStatusText(UEdGraphNode* GraphNode);
	void UpdateSimulationButtonLabels(const TSharedPtr<STextBlock>& PrimaryLabel, const TSharedPtr<STextBlock>& AlternateLabel, UEdGraphNode* GraphNode);
}
