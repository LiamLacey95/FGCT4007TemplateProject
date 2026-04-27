#include "DQSAssetEditorToolkit.h"

#include "DQSAssetEditorPanelUtils.h"
#include "DQSDialogueGraphAsset.h"
#include "DQSEdGraphNodes.h"
#include "DQSGraphEditorUtils.h"
#include "DQSGraphCompiler.h"
#include "DQSQuestGraphAsset.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraphUtilities.h"
#include "Framework/Commands/GenericCommands.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GraphEditor.h"
#include "IDetailsView.h"
#include "Modules/ModuleManager.h"
#include "HAL/PlatformApplicationMisc.h"
#include "PropertyEditorModule.h"
#include "ScopedTransaction.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Text/STextBlock.h"

namespace
{
	static const FName GraphTabId(TEXT("DialogQuestSystem.Graph"));
	static const FName DetailsTabId(TEXT("DialogQuestSystem.Details"));
	static const FName HelpTabId(TEXT("DialogQuestSystem.Help"));
	static const FName ValidationTabId(TEXT("DialogQuestSystem.Validation"));
	static const FName SimulationTabId(TEXT("DialogQuestSystem.Simulation"));

	void RefreshGraphNodePins(UEdGraph* Graph)
	{
		if (!Graph)
		{
			return;
		}

		for (UEdGraphNode* Node : Graph->Nodes)
		{
			if (Node)
			{
				Node->ReconstructNode();
			}
		}
	}

	void ResetDialogueNodeCopyIds(UDQSDialogueEdGraphNode* Node)
	{
		if (!Node)
		{
			return;
		}

		Node->CreateNewGuid();
		Node->NodeData.NodeId = FGuid::NewGuid();
		for (FDQSDialogueChoice& Choice : Node->NodeData.Choices)
		{
			Choice.ChoiceId = FGuid::NewGuid();
		}
	}

	void ResetQuestNodeCopyIds(UDQSQuestEdGraphNode* Node)
	{
		if (!Node)
		{
			return;
		}

		Node->CreateNewGuid();
		Node->NodeData.NodeId = FGuid::NewGuid();
	}
}

void FDQSAssetEditorToolkit::InitDialogueEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UDialogueGraphAsset* Asset)
{
	EditingAsset = Asset;
	EditingGraph = Asset ? Asset->EditorGraph : nullptr;
	InitializeCommon(Mode, InitToolkitHost, Asset, FText::FromString(TEXT("Dialogue Graph")));
}

void FDQSAssetEditorToolkit::InitQuestEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UQuestGraphAsset* Asset)
{
	EditingAsset = Asset;
	EditingGraph = Asset ? Asset->EditorGraph : nullptr;
	InitializeCommon(Mode, InitToolkitHost, Asset, FText::FromString(TEXT("Quest Graph")));
}

FName FDQSAssetEditorToolkit::GetToolkitFName() const
{
	return TEXT("DialogQuestSystemAssetEditor");
}

FText FDQSAssetEditorToolkit::GetBaseToolkitName() const
{
	return ToolkitTitle;
}

FString FDQSAssetEditorToolkit::GetWorldCentricTabPrefix() const
{
	return TEXT("DQS");
}

FLinearColor FDQSAssetEditorToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.85f, 0.65f, 0.18f);
}

void FDQSAssetEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
	InTabManager->RegisterTabSpawner(GraphTabId, FOnSpawnTab::CreateSP(this, &FDQSAssetEditorToolkit::SpawnGraphTab));
	InTabManager->RegisterTabSpawner(DetailsTabId, FOnSpawnTab::CreateSP(this, &FDQSAssetEditorToolkit::SpawnDetailsTab));
	InTabManager->RegisterTabSpawner(HelpTabId, FOnSpawnTab::CreateSP(this, &FDQSAssetEditorToolkit::SpawnHelpTab));
	InTabManager->RegisterTabSpawner(ValidationTabId, FOnSpawnTab::CreateSP(this, &FDQSAssetEditorToolkit::SpawnValidationTab));
	InTabManager->RegisterTabSpawner(SimulationTabId, FOnSpawnTab::CreateSP(this, &FDQSAssetEditorToolkit::SpawnSimulationTab));
}

void FDQSAssetEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	InTabManager->UnregisterTabSpawner(GraphTabId);
	InTabManager->UnregisterTabSpawner(DetailsTabId);
	InTabManager->UnregisterTabSpawner(HelpTabId);
	InTabManager->UnregisterTabSpawner(ValidationTabId);
	InTabManager->UnregisterTabSpawner(SimulationTabId);
}

void FDQSAssetEditorToolkit::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EditingAsset);
	Collector.AddReferencedObject(EditingGraph);
}

FString FDQSAssetEditorToolkit::GetReferencerName() const
{
	return TEXT("FDQSAssetEditorToolkit");
}

void FDQSAssetEditorToolkit::SaveAsset_Execute()
{
	CompileEditedAsset();
	ValidateEditedAsset();
	RefreshPanels();
	FAssetEditorToolkit::SaveAsset_Execute();
}

void FDQSAssetEditorToolkit::InitializeCommon(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* Asset, const FText& Title)
{
	ToolkitTitle = Title;
	RefreshGraphNodePins(EditingGraph);

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	FDetailsViewArgs DetailsArgs;
	DetailsArgs.bAllowSearch = true;
	DetailsArgs.bHideSelectionTip = true;
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsArgs);
	DetailsView->OnFinishedChangingProperties().AddSP(this, &FDQSAssetEditorToolkit::HandleFinishedChangingDetails);
	DetailsView->SetObject(Asset);

	BindEditorCommands();

	SGraphEditor::FGraphEditorEvents GraphEvents;
	GraphEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FDQSAssetEditorToolkit::HandleSelectionChanged);

	GraphEditor = SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.GraphToEdit(EditingGraph)
		.GraphEvents(GraphEvents)
		.IsEditable(true);

	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("DialogQuestSystemAssetEditorLayout_v2")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.72f)
				->AddTab(GraphTabId, ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()
				->SetSizeCoefficient(0.28f)
				->SetOrientation(Orient_Vertical)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.42f)
					->AddTab(DetailsTabId, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.26f)
					->AddTab(HelpTabId, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.20f)
					->AddTab(ValidationTabId, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.12f)
					->AddTab(SimulationTabId, ETabState::OpenedTab)
				)
			)
		);

	InitAssetEditor(Mode, InitToolkitHost, GetToolkitFName(), Layout, true, true, Asset);
	ExtendToolbar();
	ValidateEditedAsset();
	UpdateGuideTabVisibility();
	if (GuidanceBannerText.IsValid())
	{
		GuidanceBannerText->SetText(BuildGuidanceBannerText());
	}
	HighlightFirstUnfinishedTask();
}

void FDQSAssetEditorToolkit::BindEditorCommands()
{
	if (!GraphEditorCommands.IsValid())
	{
		GraphEditorCommands = MakeShared<FUICommandList>();
	}

	GraphEditorCommands->MapAction(
		FGenericCommands::Get().Copy,
		FExecuteAction::CreateSP(this, &FDQSAssetEditorToolkit::CopySelectedNodes),
		FCanExecuteAction::CreateSP(this, &FDQSAssetEditorToolkit::CanCopySelectedNodes));

	GraphEditorCommands->MapAction(
		FGenericCommands::Get().Cut,
		FExecuteAction::CreateSP(this, &FDQSAssetEditorToolkit::CutSelectedNodes),
		FCanExecuteAction::CreateSP(this, &FDQSAssetEditorToolkit::CanDeleteSelectedNodes));

	GraphEditorCommands->MapAction(
		FGenericCommands::Get().Paste,
		FExecuteAction::CreateSP(this, &FDQSAssetEditorToolkit::PasteNodes),
		FCanExecuteAction::CreateSP(this, &FDQSAssetEditorToolkit::CanPasteNodes));

	GraphEditorCommands->MapAction(
		FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateSP(this, &FDQSAssetEditorToolkit::DuplicateSelectedNodes),
		FCanExecuteAction::CreateSP(this, &FDQSAssetEditorToolkit::CanCopySelectedNodes));

	GraphEditorCommands->MapAction(
		FGenericCommands::Get().Delete,
		FExecuteAction::CreateSP(this, &FDQSAssetEditorToolkit::DeleteSelectedNodes),
		FCanExecuteAction::CreateSP(this, &FDQSAssetEditorToolkit::CanDeleteSelectedNodes));
}

void FDQSAssetEditorToolkit::ExtendToolbar()
{
	TSharedRef<FExtender> ToolbarExtender = MakeShared<FExtender>();
	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		GraphEditorCommands,
		FToolBarExtensionDelegate::CreateSP(this, &FDQSAssetEditorToolkit::FillToolbar));

	AddToolbarExtender(ToolbarExtender);
	RegenerateMenusAndToolbars();
}

void FDQSAssetEditorToolkit::FillToolbar(FToolBarBuilder& ToolbarBuilder)
{
	ToolbarBuilder.BeginSection(TEXT("DialogQuestSystemUX"));
	ToolbarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateSP(this, &FDQSAssetEditorToolkit::SetSimpleMode, true)),
		NAME_None,
		FText::FromString(TEXT("Guided Mode")),
		FText::FromString(TEXT("Show the guided beginner-friendly authoring workflow.")));
	ToolbarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateSP(this, &FDQSAssetEditorToolkit::SetSimpleMode, false)),
		NAME_None,
		FText::FromString(TEXT("Full Editor")),
		FText::FromString(TEXT("Show the full authoring surface.")));
	ToolbarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateSP(this, &FDQSAssetEditorToolkit::ToggleGuidedHelp)),
		NAME_None,
		FText::FromString(TEXT("Guide Panel")),
		FText::FromString(TEXT("Open or close the guided workflow panel.")));
	ToolbarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateSP(this, &FDQSAssetEditorToolkit::ValidateEditedAsset)),
		NAME_None,
		FText::FromString(TEXT("Validate")),
		FText::FromString(TEXT("Refresh validation issues and beginner guidance.")));
	ToolbarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateSP(this, &FDQSAssetEditorToolkit::ApplyFirstFixAction)),
		NAME_None,
		FText::FromString(TEXT("Fix First Issue")),
		FText::FromString(TEXT("Apply the first available guided fix for the current graph.")));
	ToolbarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateSP(this, &FDQSAssetEditorToolkit::InsertStarterTemplate)),
		NAME_None,
		FText::FromString(TEXT("Insert Template")),
		FText::FromString(TEXT("Insert a guided starter pattern with helper comments.")));
	ToolbarBuilder.AddSeparator();
	ToolbarBuilder.AddWidget(
		SNew(SBorder)
		.Padding(FMargin(10.0f, 6.0f))
		.BorderBackgroundColor(FLinearColor(0.11f, 0.14f, 0.18f, 1.0f))
		[
			SAssignNew(StatusSummaryText, STextBlock)
			.Text(BuildStatusSummaryText())
		]);
	ToolbarBuilder.EndSection();
}

TSharedRef<SDockTab> FDQSAssetEditorToolkit::SpawnGraphTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.Label(FText::FromString(TEXT("Graph")))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(8.0f, 8.0f, 8.0f, 6.0f)
			[
				SNew(SBorder)
				.Padding(FMargin(14.0f, 10.0f))
				.BorderBackgroundColor(FLinearColor(0.09f, 0.13f, 0.18f, 1.0f))
				[
					SAssignNew(GuidanceBannerText, STextBlock)
					.AutoWrapText(true)
					.Text(BuildGuidanceBannerText())
				]
			]
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				GraphEditor.ToSharedRef()
			]
		];
}

TSharedRef<SDockTab> FDQSAssetEditorToolkit::SpawnDetailsTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.Label(FText::FromString(TEXT("Details")))
		[
			DetailsView.ToSharedRef()
		];
}

TSharedRef<SDockTab> FDQSAssetEditorToolkit::SpawnHelpTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.Label(FText::FromString(TEXT("Guide")))
		[
			SNew(SBorder)
			.Padding(FMargin(8.0f))
			.BorderBackgroundColor(FLinearColor(0.08f, 0.11f, 0.15f, 1.0f))
			[
				SAssignNew(HelpScrollBox, SScrollBox)
			]
		];
}

TSharedRef<SDockTab> FDQSAssetEditorToolkit::SpawnValidationTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.Label(FText::FromString(TEXT("Validation")))
		[
			SNew(SBorder)
			.Padding(FMargin(8.0f))
			.BorderBackgroundColor(FLinearColor(0.09f, 0.11f, 0.14f, 1.0f))
			[
				SAssignNew(ValidationScrollBox, SScrollBox)
			]
		];
}

TSharedRef<SDockTab> FDQSAssetEditorToolkit::SpawnSimulationTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.Label(FText::FromString(TEXT("Simulation")))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10.0f)
			[
				SAssignNew(SimulationStatusText, STextBlock)
				.AutoWrapText(true)
				.Text(FText::FromString(TEXT("Start simulation to preview the current graph path.")))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(10.0f, 0.0f))
			[
				SNew(SUniformGridPanel)
				.SlotPadding(FMargin(6.0f))
				+ SUniformGridPanel::Slot(0, 0)
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Start")))
					.OnClicked_Lambda([this]() { StartSimulation(); return FReply::Handled(); })
				]
				+ SUniformGridPanel::Slot(1, 0)
				[
					SNew(SButton)
					.OnClicked_Lambda([this]() { StepSimulationPrimary(); return FReply::Handled(); })
					[
						SAssignNew(SimulationPrimaryButtonText, STextBlock)
						.Text(FText::FromString(TEXT("Next")))
					]
				]
				+ SUniformGridPanel::Slot(2, 0)
				[
					SNew(SButton)
					.OnClicked_Lambda([this]() { StepSimulationAlternate(); return FReply::Handled(); })
					[
						SAssignNew(SimulationAlternateButtonText, STextBlock)
						.Text(FText::FromString(TEXT("Alternative Branch")))
					]
				]
				+ SUniformGridPanel::Slot(3, 0)
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Reset")))
					.OnClicked_Lambda([this]() { ResetSimulation(); return FReply::Handled(); })
				]
			]
		];
}

void FDQSAssetEditorToolkit::HandleSelectionChanged(const TSet<UObject*>& NewSelection)
{
	if (NewSelection.IsEmpty())
	{
		DetailsView->SetObject(EditingAsset);
		RefreshHelpPanel();
		return;
	}

	if (UObject* SelectedObject = *NewSelection.CreateConstIterator())
	{
		DetailsView->SetObject(SelectedObject);
		RefreshHelpPanel();
	}
}

void FDQSAssetEditorToolkit::HandleFinishedChangingDetails(const FPropertyChangedEvent& PropertyChangedEvent)
{
	if (!GraphEditor.IsValid() || !EditingGraph)
	{
		return;
	}

	bool bRefreshedChoiceNode = false;
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
	for (UObject* SelectedObject : SelectedNodes)
	{
		UDQSDialogueEdGraphNode* DialogueNode = Cast<UDQSDialogueEdGraphNode>(SelectedObject);
		if (DialogueNode && DialogueNode->NodeData.NodeType == EDQSDialogueNodeType::Choice)
		{
			DialogueNode->RefreshPinsAfterDetailsChange();
			bRefreshedChoiceNode = true;
		}
	}

	if (bRefreshedChoiceNode)
	{
		GraphEditor->NotifyGraphChanged();
		ValidateEditedAsset();
	}
}

void FDQSAssetEditorToolkit::CompileEditedAsset() const
{
	if (UDialogueGraphAsset* DialogueAsset = Cast<UDialogueGraphAsset>(EditingAsset))
	{
		FDQSGraphCompiler::CompileDialogueAsset(DialogueAsset);
	}
	else if (UQuestGraphAsset* QuestAsset = Cast<UQuestGraphAsset>(EditingAsset))
	{
		FDQSGraphCompiler::CompileQuestAsset(QuestAsset);
	}
}

void FDQSAssetEditorToolkit::ValidateEditedAsset()
{
	if (UDialogueGraphAsset* DialogueAsset = Cast<UDialogueGraphAsset>(EditingAsset))
	{
		FDQSGraphEditorUtils::BuildDialogueValidationIssues(DialogueAsset, ValidationIssues);
		FDQSGraphEditorUtils::BuildDialogueChecklist(DialogueAsset, ChecklistItems);
	}
	else if (UQuestGraphAsset* QuestAsset = Cast<UQuestGraphAsset>(EditingAsset))
	{
		FDQSGraphEditorUtils::BuildQuestValidationIssues(QuestAsset, ValidationIssues);
		FDQSGraphEditorUtils::BuildQuestChecklist(QuestAsset, ChecklistItems);
	}

	RefreshPanels();
}

void FDQSAssetEditorToolkit::CopySelectedNodes() const
{
	if (!GraphEditor.IsValid())
	{
		return;
	}

	FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
	TSet<UObject*> ExportableNodes;
	for (UObject* SelectedObject : SelectedNodes)
	{
		if (const UEdGraphNode* Node = Cast<UEdGraphNode>(SelectedObject))
		{
			if (Node->CanDuplicateNode())
			{
				ExportableNodes.Add(SelectedObject);
			}
		}
	}

	if (ExportableNodes.IsEmpty())
	{
		return;
	}

	FString ExportedText;
	FEdGraphUtilities::ExportNodesToText(ExportableNodes, ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

bool FDQSAssetEditorToolkit::CanCopySelectedNodes() const
{
	if (!GraphEditor.IsValid())
	{
		return false;
	}

	for (UObject* SelectedObject : GraphEditor->GetSelectedNodes())
	{
		if (const UEdGraphNode* Node = Cast<UEdGraphNode>(SelectedObject))
		{
			if (Node->CanDuplicateNode())
			{
				return true;
			}
		}
	}

	return false;
}

void FDQSAssetEditorToolkit::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedNodes();
}

void FDQSAssetEditorToolkit::PasteNodes()
{
	if (!GraphEditor.IsValid() || !EditingGraph)
	{
		return;
	}

	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);
	if (ClipboardContent.IsEmpty() || !FEdGraphUtilities::CanImportNodesFromText(EditingGraph, ClipboardContent))
	{
		return;
	}

	const FScopedTransaction Transaction(FText::FromString(TEXT("Paste Dialog Quest Nodes")));
	EditingGraph->Modify();
	GraphEditor->ClearSelectionSet();

	TSet<UEdGraphNode*> PastedNodes;
	FEdGraphUtilities::ImportNodesFromText(EditingGraph, ClipboardContent, PastedNodes);
	FEdGraphUtilities::PostProcessPastedNodes(PastedNodes);

	TArray<UEdGraphNode*> PastedNodeArray = PastedNodes.Array();
	const FIntRect Bounds = FEdGraphUtilities::CalculateApproximateNodeBoundaries(PastedNodeArray);
	const FVector2f PasteLocation = GraphEditor->GetPasteLocation2f();
	const FVector2f BoundsCenter((Bounds.Min.X + Bounds.Max.X) * 0.5f, (Bounds.Min.Y + Bounds.Max.Y) * 0.5f);
	const FVector2f Offset = PasteLocation - BoundsCenter;

	for (UEdGraphNode* Node : PastedNodes)
	{
		if (!Node)
		{
			continue;
		}

		Node->Modify();
		Node->NodePosX = FMath::RoundToInt(Node->NodePosX + Offset.X);
		Node->NodePosY = FMath::RoundToInt(Node->NodePosY + Offset.Y);
		Node->SnapToGrid(16);

		if (UDQSDialogueEdGraphNode* DialogueNode = Cast<UDQSDialogueEdGraphNode>(Node))
		{
			ResetDialogueNodeCopyIds(DialogueNode);
		}
		else if (UDQSQuestEdGraphNode* QuestNode = Cast<UDQSQuestEdGraphNode>(Node))
		{
			ResetQuestNodeCopyIds(QuestNode);
		}

		GraphEditor->SetNodeSelection(Node, true);
	}

	EditingGraph->NotifyGraphChanged();
	CompileEditedAsset();
	ValidateEditedAsset();
}

bool FDQSAssetEditorToolkit::CanPasteNodes() const
{
	if (!EditingGraph)
	{
		return false;
	}

	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);
	return !ClipboardContent.IsEmpty() && FEdGraphUtilities::CanImportNodesFromText(EditingGraph, ClipboardContent);
}

void FDQSAssetEditorToolkit::DuplicateSelectedNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

void FDQSAssetEditorToolkit::DeleteSelectedNodes()
{
	if (!GraphEditor.IsValid() || !EditingGraph)
	{
		return;
	}

	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
	if (SelectedNodes.IsEmpty())
	{
		return;
	}

	const FScopedTransaction Transaction(FText::FromString(TEXT("Delete Dialog Quest Nodes")));
	EditingGraph->Modify();

	for (UObject* SelectedObject : SelectedNodes)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(SelectedObject))
		{
			if (Node->CanUserDeleteNode())
			{
				Node->Modify();
				Node->DestroyNode();
			}
		}
	}

	CompileEditedAsset();
	ValidateEditedAsset();
}

bool FDQSAssetEditorToolkit::CanDeleteSelectedNodes() const
{
	if (!GraphEditor.IsValid())
	{
		return false;
	}

	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
	for (UObject* SelectedObject : SelectedNodes)
	{
		if (const UEdGraphNode* Node = Cast<UEdGraphNode>(SelectedObject))
		{
			if (Node->CanUserDeleteNode())
			{
				return true;
			}
		}
	}

	return false;
}

void FDQSAssetEditorToolkit::SetSimpleMode(const bool bUseSimpleMode)
{
	FDQSGraphEditorUtils::SetSimpleMode(EditingAsset, bUseSimpleMode);
	FDQSGraphEditorUtils::SetGuidedHelpEnabled(EditingAsset, bUseSimpleMode);
	if (DetailsView.IsValid())
	{
		DetailsView->ForceRefresh();
	}
	UpdateGuideTabVisibility();
	RefreshPanels();
}

bool FDQSAssetEditorToolkit::IsSimpleMode() const
{
	return FDQSGraphEditorUtils::IsSimpleMode(EditingAsset);
}

void FDQSAssetEditorToolkit::ToggleGuidedHelp()
{
	FDQSGraphEditorUtils::SetGuidedHelpEnabled(EditingAsset, !IsGuidedHelpEnabled());
	UpdateGuideTabVisibility();
	RefreshPanels();
}

bool FDQSAssetEditorToolkit::IsGuidedHelpEnabled() const
{
	return FDQSGraphEditorUtils::IsGuidedHelpEnabled(EditingAsset);
}

void FDQSAssetEditorToolkit::InsertStarterTemplate()
{
	if (UDialogueGraphAsset* DialogueAsset = Cast<UDialogueGraphAsset>(EditingAsset))
	{
		FDQSGraphEditorUtils::InsertDialogueSnippet(DialogueAsset, EDQSDialogueSnippet::QuestAcceptance);
	}
	else if (UQuestGraphAsset* QuestAsset = Cast<UQuestGraphAsset>(EditingAsset))
	{
		FDQSGraphEditorUtils::InsertQuestSnippet(QuestAsset, EDQSQuestSnippet::StartObjectiveComplete);
	}

	ValidateEditedAsset();
}

void FDQSAssetEditorToolkit::ApplyFirstFixAction()
{
	UObject* SelectedObject = nullptr;
	if (GraphEditor.IsValid())
	{
		const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
		if (!SelectedNodes.IsEmpty())
		{
			SelectedObject = *SelectedNodes.CreateConstIterator();
		}
	}

	if (UDQSDialogueEdGraphNode* SelectedDialogueNode = Cast<UDQSDialogueEdGraphNode>(SelectedObject))
	{
		if (SelectedDialogueNode->NodeData.NodeType == EDQSDialogueNodeType::Choice && SelectedDialogueNode->NodeData.Choices.Num() < 2)
		{
			SelectedDialogueNode->AddChoiceOutput();
			ValidateEditedAsset();
			return;
		}

		if (!SelectedDialogueNode->NodeData.SpeakerId.IsNone())
		{
			if (UDialogueGraphAsset* DialogueAsset = Cast<UDialogueGraphAsset>(EditingAsset))
			{
				if (!DialogueAsset->Speakers.ContainsByPredicate([SelectedDialogueNode](const FDQSSpeakerDefinition& Speaker) { return Speaker.SpeakerId == SelectedDialogueNode->NodeData.SpeakerId; }))
				{
					DialogueAsset->Modify();
					FDQSSpeakerDefinition& Speaker = DialogueAsset->Speakers.AddDefaulted_GetRef();
					Speaker.SpeakerId = SelectedDialogueNode->NodeData.SpeakerId;
					Speaker.DisplayName = FText::FromName(Speaker.SpeakerId);
					ValidateEditedAsset();
					return;
				}
			}
		}
	}

	if (UDQSQuestEdGraphNode* SelectedQuestNode = Cast<UDQSQuestEdGraphNode>(SelectedObject))
	{
		if (SelectedQuestNode->NodeData.NodeType == EDQSQuestNodeType::Objective && !SelectedQuestNode->NodeData.ObjectiveTag.IsValid())
		{
			SelectedQuestNode->Modify();
			SelectedQuestNode->NodeData.ObjectiveTag = FGameplayTag::RequestGameplayTag(FName(*FString::Printf(TEXT("Quest.%s.Objective"), *GetNameSafe(EditingAsset))), false);
			ValidateEditedAsset();
			return;
		}
	}

	if (UDialogueGraphAsset* DialogueAsset = Cast<UDialogueGraphAsset>(EditingAsset))
	{
		const bool bHasEnd = DialogueAsset->Nodes.ContainsByPredicate([](const FDQSDialogueNode& Node) { return Node.NodeType == EDQSDialogueNodeType::End; });
		if (!bHasEnd)
		{
			UDQSDialogueEdGraphNode* EndNode = FDQSGraphEditorUtils::CreateDialogueNode(DialogueAsset->EditorGraph, FVector2D(980.0f, 360.0f), EDQSDialogueNodeType::End, FText::FromString(TEXT("End")));
			for (UEdGraphNode* GraphNode : DialogueAsset->EditorGraph->Nodes)
			{
				if (UDQSDialogueEdGraphNode* DialogueNode = Cast<UDQSDialogueEdGraphNode>(GraphNode))
				{
					if (DialogueNode->NodeData.NodeType != EDQSDialogueNodeType::End)
					{
						bool bHasOutputLink = false;
						for (UEdGraphPin* Pin : DialogueNode->Pins)
						{
							if (Pin && Pin->Direction == EGPD_Output && Pin->LinkedTo.Num() > 0)
							{
								bHasOutputLink = true;
								break;
							}
						}

						if (!bHasOutputLink)
						{
							FDQSGraphEditorUtils::ConnectNodes(DialogueNode, 0, EndNode);
							break;
						}
					}
				}
			}

			ValidateEditedAsset();
			return;
		}
	}

	if (UQuestGraphAsset* QuestAsset = Cast<UQuestGraphAsset>(EditingAsset))
	{
		const bool bHasComplete = QuestAsset->Nodes.ContainsByPredicate([](const FDQSQuestNode& Node) { return Node.NodeType == EDQSQuestNodeType::Complete; });
		if (!bHasComplete)
		{
			FDQSGraphEditorUtils::CreateQuestNode(QuestAsset->EditorGraph, FVector2D(960.0f, 180.0f), EDQSQuestNodeType::Complete, FText::FromString(TEXT("Complete")));
			ValidateEditedAsset();
		}
	}
}

void FDQSAssetEditorToolkit::ApplyFixForIssue(FDQSValidationIssue Issue)
{
	if (Issue.NodeId.IsValid())
	{
		JumpToIssueNode(Issue);
	}
	ApplyFirstFixAction();
}

void FDQSAssetEditorToolkit::RefreshPanels()
{
	if (GuidanceBannerText.IsValid())
	{
		GuidanceBannerText->SetText(BuildGuidanceBannerText());
	}
	RefreshToolbarStatus();
	RefreshHelpPanel();
	RefreshValidationPanel();
	RefreshSimulationPanel();
}

void FDQSAssetEditorToolkit::RefreshToolbarStatus()
{
	if (StatusSummaryText.IsValid())
	{
		StatusSummaryText->SetText(BuildStatusSummaryText());
	}
}

void FDQSAssetEditorToolkit::UpdateGuideTabVisibility()
{
	const TSharedPtr<FTabManager> LocalTabManager = GetTabManager();
	if (!LocalTabManager.IsValid())
	{
		return;
	}

	const TSharedPtr<SDockTab> ExistingTab = LocalTabManager->FindExistingLiveTab(HelpTabId);
	if (IsGuidedHelpEnabled())
	{
		if (!ExistingTab.IsValid())
		{
			LocalTabManager->TryInvokeTab(HelpTabId);
		}
	}
	else if (ExistingTab.IsValid())
	{
		ExistingTab->RequestCloseTab();
	}
}

void FDQSAssetEditorToolkit::RefreshHelpPanel()
{
	if (!HelpScrollBox.IsValid())
	{
		return;
	}

	UObject* SelectedObject = nullptr;
	if (GraphEditor.IsValid())
	{
		const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
		if (!SelectedNodes.IsEmpty())
		{
			SelectedObject = *SelectedNodes.CreateConstIterator();
		}
	}
	TOptional<FDQSNodeHelpEntry> SelectedNodeHelp;
	if (const UDQSDialogueEdGraphNode* DialogueNode = Cast<UDQSDialogueEdGraphNode>(SelectedObject))
	{
		SelectedNodeHelp = FDQSGraphEditorUtils::GetDialogueNodeHelp(DialogueNode->NodeData.NodeType);
	}
	else if (const UDQSQuestEdGraphNode* QuestNode = Cast<UDQSQuestEdGraphNode>(SelectedObject))
	{
		SelectedNodeHelp = FDQSGraphEditorUtils::GetQuestNodeHelp(QuestNode->NodeData.NodeType);
	}

	DQSAssetEditorPanelUtils::PopulateHelpPanel(HelpScrollBox, ChecklistItems, SelectedNodeHelp);
}

void FDQSAssetEditorToolkit::RefreshValidationPanel()
{
	if (!ValidationScrollBox.IsValid())
	{
		return;
	}

	DQSAssetEditorPanelUtils::PopulateValidationPanel(
		ValidationScrollBox,
		ValidationIssues,
		BuildReadinessSummaryText(),
		GetBlockingIssueCount(),
		GetWarningIssueCount(),
		[this](const FDQSValidationIssue& Issue) { JumpToIssueNode(Issue); },
		[this](const FDQSValidationIssue& Issue) { ApplyFixForIssue(Issue); });
}

void FDQSAssetEditorToolkit::RefreshSimulationPanel()
{
	if (!SimulationStatusText.IsValid())
	{
		return;
	}

	if (!SimulatedNodeId.IsValid())
	{
		SimulationStatusText->SetText(FText::FromString(TEXT("Start simulation to preview the current graph path.")));
		UpdateSimulationActionLabels();
		return;
	}

	if (UEdGraphNode* GraphNode = FindNodeByGuid(SimulatedNodeId))
	{
		SimulationStatusText->SetText(DQSAssetEditorPanelUtils::BuildSimulationStatusText(GraphNode));
	}

	UpdateSimulationActionLabels();
}

void FDQSAssetEditorToolkit::HighlightFirstUnfinishedTask()
{
	for (const FDQSValidationIssue& Issue : ValidationIssues)
	{
		if (Issue.NodeId.IsValid())
		{
			JumpToIssueNode(Issue);
			return;
		}
	}
}

FText FDQSAssetEditorToolkit::BuildGuidanceBannerText() const
{
	for (const FDQSChecklistItem& Item : ChecklistItems)
	{
		if (!Item.bCompleted)
		{
			return FText::Format(FText::FromString(TEXT("{0}\nNext step: {1}")), Item.Label, Item.Guidance);
		}
	}

	return IsSimpleMode()
		? FText::FromString(TEXT("Guided Mode is active. Your first playable path looks complete. Use Validate or Insert Template to keep expanding."))
		: FText::FromString(TEXT("Full Editor is active. The full authoring surface is available."));
}

FText FDQSAssetEditorToolkit::BuildStatusSummaryText() const
{
	return FText::Format(
		FText::FromString(TEXT("{0} | {1} | {2} blocking | {3} warnings")),
		IsSimpleMode() ? FText::FromString(TEXT("Guided Mode")) : FText::FromString(TEXT("Full Editor")),
		BuildReadinessSummaryText(),
		FText::AsNumber(GetBlockingIssueCount()),
		FText::AsNumber(GetWarningIssueCount()));
}

FText FDQSAssetEditorToolkit::BuildReadinessSummaryText() const
{
	return GetBlockingIssueCount() == 0
		? FText::FromString(TEXT("First playable ready"))
		: FText::FromString(TEXT("Needs fixes"));
}

int32 FDQSAssetEditorToolkit::GetBlockingIssueCount() const
{
	return ValidationIssues.FilterByPredicate([](const FDQSValidationIssue& Issue)
	{
		return Issue.Severity == EDQSValidationSeverity::Blocking;
	}).Num();
}

int32 FDQSAssetEditorToolkit::GetWarningIssueCount() const
{
	return ValidationIssues.FilterByPredicate([](const FDQSValidationIssue& Issue)
	{
		return Issue.Severity == EDQSValidationSeverity::Warning;
	}).Num();
}

UEdGraphNode* FDQSAssetEditorToolkit::FindNodeByGuid(const FGuid& NodeId) const
{
	if (!EditingGraph || !NodeId.IsValid())
	{
		return nullptr;
	}

	for (UEdGraphNode* GraphNode : EditingGraph->Nodes)
	{
		if (const UDQSDialogueEdGraphNode* DialogueNode = Cast<UDQSDialogueEdGraphNode>(GraphNode))
		{
			if (DialogueNode->NodeData.NodeId == NodeId)
			{
				return GraphNode;
			}
		}
		else if (const UDQSQuestEdGraphNode* QuestNode = Cast<UDQSQuestEdGraphNode>(GraphNode))
		{
			if (QuestNode->NodeData.NodeId == NodeId)
			{
				return GraphNode;
			}
		}
	}

	return nullptr;
}

void FDQSAssetEditorToolkit::JumpToIssueNode(const FDQSValidationIssue& Issue) const
{
	if (!GraphEditor.IsValid())
	{
		return;
	}

	if (UEdGraphNode* Node = FindNodeByGuid(Issue.NodeId))
	{
		GraphEditor->ClearSelectionSet();
		GraphEditor->SetNodeSelection(Node, true);
		GraphEditor->JumpToNode(Node, false);
	}
}

void FDQSAssetEditorToolkit::StartSimulation()
{
	if (UDialogueGraphAsset* DialogueAsset = Cast<UDialogueGraphAsset>(EditingAsset))
	{
		FDQSGraphCompiler::CompileDialogueAsset(DialogueAsset);
		SimulatedNodeId = DialogueAsset->EntryNodeId;
	}
	else if (UQuestGraphAsset* QuestAsset = Cast<UQuestGraphAsset>(EditingAsset))
	{
		FDQSGraphCompiler::CompileQuestAsset(QuestAsset);
		SimulatedNodeId = QuestAsset->EntryNodeId;
	}

	RefreshSimulationPanel();
}

void FDQSAssetEditorToolkit::ResetSimulation()
{
	SimulatedNodeId.Invalidate();
	RefreshSimulationPanel();
}

void FDQSAssetEditorToolkit::StepSimulationPrimary()
{
	AdvanceSimulation(false);
}

void FDQSAssetEditorToolkit::StepSimulationAlternate()
{
	AdvanceSimulation(true);
}

void FDQSAssetEditorToolkit::AdvanceSimulation(const bool bUseAlternatePath)
{
	if (!SimulatedNodeId.IsValid())
	{
		StartSimulation();
		return;
	}

	if (UDialogueGraphAsset* DialogueAsset = Cast<UDialogueGraphAsset>(EditingAsset))
	{
		if (const FDQSDialogueNode* Node = DialogueAsset->FindNodeById(SimulatedNodeId))
		{
			if (Node->NodeType == EDQSDialogueNodeType::Choice)
			{
				SimulatedNodeId = Node->Choices.IsValidIndex(bUseAlternatePath ? 1 : 0) ? Node->Choices[bUseAlternatePath ? 1 : 0].TargetNodeId : FGuid();
			}
			else if (Node->NodeType == EDQSDialogueNodeType::Condition || Node->NodeType == EDQSDialogueNodeType::ConditionedStart)
			{
				SimulatedNodeId = bUseAlternatePath ? Node->AlternateNodeId : Node->NextNodeId;
			}
			else if (Node->NodeType == EDQSDialogueNodeType::Jump)
			{
				SimulatedNodeId = Node->JumpTargetNodeId;
			}
			else
			{
				SimulatedNodeId = Node->NextNodeId;
			}
		}
	}
	else if (UQuestGraphAsset* QuestAsset = Cast<UQuestGraphAsset>(EditingAsset))
	{
		if (const FDQSQuestNode* Node = QuestAsset->FindNodeById(SimulatedNodeId))
		{
			if (Node->NodeType == EDQSQuestNodeType::Condition || Node->NodeType == EDQSQuestNodeType::Branch)
			{
				SimulatedNodeId = bUseAlternatePath ? Node->AlternateNodeId : Node->NextNodeId;
			}
			else
			{
				SimulatedNodeId = Node->NextNodeId;
			}
		}
	}

	RefreshSimulationPanel();
}

void FDQSAssetEditorToolkit::UpdateSimulationActionLabels()
{
	DQSAssetEditorPanelUtils::UpdateSimulationButtonLabels(
		SimulationPrimaryButtonText,
		SimulationAlternateButtonText,
		FindNodeByGuid(SimulatedNodeId));
}




