#include "DQSAssetEditorPanelUtils.h"

#include "DQSEdGraphNodes.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Text/STextBlock.h"

namespace
{
	FLinearColor GetSeverityColor(const EDQSValidationSeverity Severity)
	{
		switch (Severity)
		{
		case EDQSValidationSeverity::Blocking:
			return FLinearColor(0.74f, 0.23f, 0.20f, 1.0f);
		case EDQSValidationSeverity::Warning:
			return FLinearColor(0.82f, 0.60f, 0.20f, 1.0f);
		default:
			return FLinearColor(0.30f, 0.58f, 0.82f, 1.0f);
		}
	}

	FText GetSeverityLabel(const EDQSValidationSeverity Severity)
	{
		switch (Severity)
		{
		case EDQSValidationSeverity::Blocking:
			return FText::FromString(TEXT("Blocking"));
		case EDQSValidationSeverity::Warning:
			return FText::FromString(TEXT("Warning"));
		default:
			return FText::FromString(TEXT("Suggestion"));
		}
	}
}

void DQSAssetEditorPanelUtils::PopulateHelpPanel(
	const TSharedPtr<SScrollBox>& HelpScrollBox,
	const TArray<FDQSChecklistItem>& ChecklistItems,
	const TOptional<FDQSNodeHelpEntry>& SelectedNodeHelp)
{
	if (!HelpScrollBox.IsValid())
	{
		return;
	}

	HelpScrollBox->ClearChildren();
	HelpScrollBox->AddSlot()
	[
		SNew(SBorder)
		.Padding(FMargin(12.0f))
		.BorderBackgroundColor(FLinearColor(0.10f, 0.15f, 0.20f, 1.0f))
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("1. Edit the named starter nodes.\n2. Drag from a pin to create and auto-connect the next node.\n3. Keep the checklist green before expanding the graph.\n4. Switch to Full Editor only when you need advanced flow control.")))
			.AutoWrapText(true)
		]
	];

	for (const FDQSChecklistItem& Item : ChecklistItems)
	{
		HelpScrollBox->AddSlot()
		[
			SNew(SBorder)
			.Padding(FMargin(10.0f))
			.BorderBackgroundColor(Item.bCompleted ? FLinearColor(0.16f, 0.30f, 0.20f, 1.0f) : FLinearColor(0.22f, 0.18f, 0.10f, 1.0f))
			[
				SNew(STextBlock)
				.Text(FText::Format(FText::FromString(TEXT("{0}\n{1}")), Item.Label, Item.Guidance))
				.AutoWrapText(true)
			]
		];
	}

	if (SelectedNodeHelp.IsSet())
	{
		const FDQSNodeHelpEntry& Help = SelectedNodeHelp.GetValue();
		HelpScrollBox->AddSlot()[SNew(SSeparator)];
		HelpScrollBox->AddSlot()
		[
			SNew(STextBlock)
			.Text(FText::Format(FText::FromString(TEXT("Selected Node: {0}")), Help.Title))
		];
		HelpScrollBox->AddSlot()
		[
			SNew(STextBlock)
			.Text(FText::Format(FText::FromString(TEXT("{0}\nUse when: {1}\nBeginner mistake: {2}\nExample: {3}")), Help.Purpose, Help.UseWhen, Help.BeginnerMistake, Help.Example))
			.AutoWrapText(true)
		];
	}
}

void DQSAssetEditorPanelUtils::PopulateValidationPanel(
	const TSharedPtr<SScrollBox>& ValidationScrollBox,
	const TArray<FDQSValidationIssue>& ValidationIssues,
	const FText& ReadinessSummary,
	const int32 BlockingIssueCount,
	const int32 WarningIssueCount,
	TFunctionRef<void(const FDQSValidationIssue&)> OnJumpToIssue,
	TFunctionRef<void(const FDQSValidationIssue&)> OnFixIssue)
{
	if (!ValidationScrollBox.IsValid())
	{
		return;
	}

	const int32 SuggestionCount = ValidationIssues.FilterByPredicate([](const FDQSValidationIssue& Issue)
	{
		return Issue.Severity == EDQSValidationSeverity::Suggestion;
	}).Num();

	ValidationScrollBox->ClearChildren();
	ValidationScrollBox->AddSlot()
	[
		SNew(SBorder)
		.Padding(FMargin(10.0f))
		.BorderBackgroundColor(FLinearColor(0.10f, 0.13f, 0.18f, 1.0f))
		[
			SNew(STextBlock)
			.Text(FText::Format(
				FText::FromString(TEXT("{0}\nBlocking: {1}  Warnings: {2}  Suggestions: {3}")),
				ReadinessSummary,
				FText::AsNumber(BlockingIssueCount),
				FText::AsNumber(WarningIssueCount),
				FText::AsNumber(SuggestionCount)))
			.AutoWrapText(true)
		]
	];

	if (ValidationIssues.IsEmpty())
	{
		ValidationScrollBox->AddSlot()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("No validation issues. This graph is in good shape.")))
			.AutoWrapText(true)
		];
		return;
	}

	for (const FDQSValidationIssue& Issue : ValidationIssues)
	{
		ValidationScrollBox->AddSlot()
		[
			SNew(SBorder)
			.Padding(8.0f)
			.BorderBackgroundColor(GetSeverityColor(Issue.Severity))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(STextBlock)
					.Text(FText::Format(FText::FromString(TEXT("{0}: {1}")), GetSeverityLabel(Issue.Severity), Issue.Title))
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(0.0f, 4.0f)
				[
					SNew(STextBlock)
					.Text(FText::Format(FText::FromString(TEXT("{0}\nHow to fix: {1}")), Issue.Message, Issue.Guidance))
					.AutoWrapText(true)
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(0.0f, 6.0f)
				[
					SNew(SUniformGridPanel)
					.SlotPadding(FMargin(6.0f, 0.0f))
					+ SUniformGridPanel::Slot(0, 0)
					[
						SNew(SButton)
						.Text(FText::FromString(TEXT("Jump To Node")))
						.IsEnabled(Issue.NodeId.IsValid())
						.OnClicked_Lambda([Issue, &OnJumpToIssue]()
						{
							OnJumpToIssue(Issue);
							return FReply::Handled();
						})
					]
					+ SUniformGridPanel::Slot(1, 0)
					[
						SNew(SButton)
						.Text(FText::FromString(TEXT("Fix It")))
						.IsEnabled(Issue.bCanFix)
						.OnClicked_Lambda([Issue, &OnFixIssue]()
						{
							OnFixIssue(Issue);
							return FReply::Handled();
						})
					]
				]
			]
		];
	}
}

FText DQSAssetEditorPanelUtils::BuildSimulationStatusText(UEdGraphNode* GraphNode)
{
	if (const UDQSDialogueEdGraphNode* DialogueNode = Cast<UDQSDialogueEdGraphNode>(GraphNode))
	{
		return FText::Format(
			FText::FromString(TEXT("Current simulated dialogue node: {0}\nType: {1}\nText: {2}")),
			DialogueNode->GetNodeTitle(ENodeTitleType::ListView),
			StaticEnum<EDQSDialogueNodeType>()->GetDisplayNameTextByValue(static_cast<int64>(DialogueNode->NodeData.NodeType)),
			DialogueNode->NodeData.Text);
	}

	if (const UDQSQuestEdGraphNode* QuestNode = Cast<UDQSQuestEdGraphNode>(GraphNode))
	{
		return FText::Format(
			FText::FromString(TEXT("Current simulated quest node: {0}\nType: {1}\nDescription: {2}")),
			QuestNode->GetNodeTitle(ENodeTitleType::ListView),
			StaticEnum<EDQSQuestNodeType>()->GetDisplayNameTextByValue(static_cast<int64>(QuestNode->NodeData.NodeType)),
			QuestNode->NodeData.Description);
	}

	return FText::FromString(TEXT("Start simulation to preview the current graph path."));
}

void DQSAssetEditorPanelUtils::UpdateSimulationButtonLabels(const TSharedPtr<STextBlock>& PrimaryLabel, const TSharedPtr<STextBlock>& AlternateLabel, UEdGraphNode* GraphNode)
{
	if (!PrimaryLabel.IsValid() || !AlternateLabel.IsValid())
	{
		return;
	}

	FText PrimaryText = FText::FromString(TEXT("Next"));
	FText AlternateText = FText::FromString(TEXT("Alternative Branch"));

	if (const UDQSDialogueEdGraphNode* DialogueNode = Cast<UDQSDialogueEdGraphNode>(GraphNode))
	{
		if (DialogueNode->NodeData.NodeType == EDQSDialogueNodeType::Choice)
		{
			PrimaryText = FText::FromString(TEXT("Choice 1"));
			AlternateText = FText::FromString(TEXT("Choice 2"));
		}
		else if (DialogueNode->NodeData.NodeType == EDQSDialogueNodeType::ConditionedStart || DialogueNode->NodeData.NodeType == EDQSDialogueNodeType::Condition)
		{
			PrimaryText = FText::FromString(TEXT("Matched Branch"));
			AlternateText = FText::FromString(TEXT("Fallback Branch"));
		}
	}
	else if (const UDQSQuestEdGraphNode* QuestNode = Cast<UDQSQuestEdGraphNode>(GraphNode))
	{
		if (QuestNode->NodeData.NodeType == EDQSQuestNodeType::Condition)
		{
			PrimaryText = FText::FromString(TEXT("Next Node"));
			AlternateText = FText::FromString(TEXT("Alternative Node"));
		}
		else if (QuestNode->NodeData.NodeType == EDQSQuestNodeType::Branch)
		{
			PrimaryText = FText::FromString(TEXT("Primary Branch"));
			AlternateText = FText::FromString(TEXT("Alternative Branch"));
		}
	}

	PrimaryLabel->SetText(PrimaryText);
	AlternateLabel->SetText(AlternateText);
}
