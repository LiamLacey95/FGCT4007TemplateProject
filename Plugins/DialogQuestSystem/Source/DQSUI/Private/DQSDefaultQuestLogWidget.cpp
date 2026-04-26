#include "DQSDefaultQuestLogWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "DQSQuestGraphAsset.h"
#include "DQSQuestViewModel.h"
#include "DQSUIThemeAsset.h"
#include "Widgets/SWidget.h"

namespace
{
	FText GetQuestStatusText(const EDQSQuestStatus Status)
	{
		switch (Status)
		{
		case EDQSQuestStatus::Available:
			return FText::FromString(TEXT("Available"));
		case EDQSQuestStatus::Active:
			return FText::FromString(TEXT("Active"));
		case EDQSQuestStatus::Completed:
			return FText::FromString(TEXT("Completed"));
		case EDQSQuestStatus::Failed:
			return FText::FromString(TEXT("Failed"));
		default:
			return FText::FromString(TEXT("Inactive"));
		}
	}

	FText GetQuestDescriptionText(const FDQSQuestRuntimeState& State)
	{
		const UQuestGraphAsset* QuestAsset = State.QuestAsset.LoadSynchronous();
		if (!QuestAsset)
		{
			return FText::FromString(TEXT("Missing quest asset reference."));
		}

		if (const FDQSQuestNode* ActiveNode = QuestAsset->FindNodeById(State.ActiveNodeId))
		{
			if (!ActiveNode->Description.IsEmpty())
			{
				if (ActiveNode->NodeType == EDQSQuestNodeType::Objective && ActiveNode->ObjectiveTag.IsValid())
				{
					const int32 CurrentCount = State.ObjectiveProgress.FindRef(ActiveNode->ObjectiveTag.GetTagName());
					return FText::Format(
						FText::FromString(TEXT("{0} ({1}/{2})")),
						ActiveNode->Description,
						FText::AsNumber(CurrentCount),
						FText::AsNumber(ActiveNode->TargetCount));
				}

				return ActiveNode->Description;
			}
		}

		return QuestAsset->Metadata.Description.IsEmpty() ? FText::FromString(TEXT("No quest details available yet.")) : QuestAsset->Metadata.Description;
	}
}

TSharedRef<SWidget> UDQSDefaultQuestLogWidget::RebuildWidget()
{
	BuildDefaultLayout();
	return Super::RebuildWidget();
}

void UDQSDefaultQuestLogWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BuildDefaultLayout();
	SetQuestLogOpen(bStartOpened);

	if (ViewModel)
	{
		ViewModel->OnStateChanged.AddDynamic(this, &UDQSDefaultQuestLogWidget::HandleQuestStateChanged);
	}

	if (CloseButton)
	{
		CloseButton->OnClicked.RemoveAll(this);
		CloseButton->OnClicked.AddDynamic(this, &UDQSDefaultQuestLogWidget::HandleCloseClicked);
	}

	HandleQuestStateChanged();
}

void UDQSDefaultQuestLogWidget::OpenQuestLog()
{
	SetQuestLogOpen(true);
}

void UDQSDefaultQuestLogWidget::CloseQuestLog()
{
	SetQuestLogOpen(false);
}

void UDQSDefaultQuestLogWidget::ToggleQuestLog()
{
	SetQuestLogOpen(!bIsOpen);
}

void UDQSDefaultQuestLogWidget::HandleQuestStateChanged()
{
	SelectBestQuest();
	ApplyTheme();
	RefreshFromState();
}

void UDQSDefaultQuestLogWidget::HandleCloseClicked()
{
	CloseQuestLog();
}

void UDQSDefaultQuestLogWidget::BuildDefaultLayout()
{
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return;
	}

	UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("QuestLogRootCanvas"));
	WidgetTree->RootWidget = RootCanvas;

	UOverlay* RootOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("QuestLogRootOverlay"));
	if (UCanvasPanelSlot* RootOverlaySlot = RootCanvas->AddChildToCanvas(RootOverlay))
	{
		RootOverlaySlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		RootOverlaySlot->SetOffsets(FMargin(0.0f));
	}

	UBorder* BackdropBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("QuestLogBackdrop"));
	BackdropBorder->SetBrushColor(FLinearColor(0.01f, 0.02f, 0.03f, 0.32f));
	RootOverlay->AddChildToOverlay(BackdropBorder);

	QuestLogPanelSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("QuestLogPanelSize"));
	if (UOverlaySlot* PanelSlot = RootOverlay->AddChildToOverlay(QuestLogPanelSize))
	{
		PanelSlot->SetHorizontalAlignment(HAlign_Center);
		PanelSlot->SetVerticalAlignment(VAlign_Center);
		PanelSlot->SetPadding(FMargin(28.0f, 36.0f));
	}
	QuestLogPanelSize->SetWidthOverride(920.0f);
	QuestLogPanelSize->SetHeightOverride(560.0f);

	QuestLogChrome = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("QuestLogChrome"));
	QuestLogChrome->SetPadding(FMargin(18.0f));
	QuestLogPanelSize->AddChild(QuestLogChrome);

	UVerticalBox* RootBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("QuestLogRootBox"));
	QuestLogChrome->SetContent(RootBox);

	UHorizontalBox* HeaderRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HeaderRow"));
	if (UVerticalBoxSlot* HeaderSlot = RootBox->AddChildToVerticalBox(HeaderRow))
	{
		HeaderSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 14.0f));
	}

	HeaderText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HeaderText"));
	HeaderText->SetText(FText::FromString(TEXT("QUEST LOG")));
	HeaderText->SetTextTransformPolicy(ETextTransformPolicy::ToUpper);
	if (UHorizontalBoxSlot* HeaderTextSlot = HeaderRow->AddChildToHorizontalBox(HeaderText))
	{
		HeaderTextSlot->SetVerticalAlignment(VAlign_Center);
	}

	USpacer* Spacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass(), TEXT("HeaderSpacer"));
	if (UHorizontalBoxSlot* SpacerSlot = HeaderRow->AddChildToHorizontalBox(Spacer))
	{
		SpacerSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}

	CloseButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("CloseButton"));
	if (UHorizontalBoxSlot* CloseSlot = HeaderRow->AddChildToHorizontalBox(CloseButton))
	{
		CloseSlot->SetVerticalAlignment(VAlign_Center);
	}

	UTextBlock* CloseText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CloseButtonText"));
	CloseText->SetText(FText::FromString(TEXT("Close")));
	CloseButton->SetContent(CloseText);

	SubheaderText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SubheaderText"));
	SubheaderText->SetText(FText::FromString(TEXT("Review active, completed, and failed quests in one place.")));
	SubheaderText->SetAutoWrapText(true);
	if (UVerticalBoxSlot* SubheaderSlot = RootBox->AddChildToVerticalBox(SubheaderText))
	{
		SubheaderSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 14.0f));
	}

		UHorizontalBox* ContentRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("ContentRow"));
		RootBox->AddChildToVerticalBox(ContentRow);

		ListPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ListPanel"));
		if (UHorizontalBoxSlot* ListSlot = ContentRow->AddChildToHorizontalBox(ListPanel))
		{
			ListSlot->SetPadding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
			ListSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
		}
	ListPanel->SetPadding(FMargin(12.0f));

	UVerticalBox* ListBoxRoot = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ListBoxRoot"));
	ListPanel->SetContent(ListBoxRoot);

	UTextBlock* ListHeader = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ListHeader"));
	ListHeader->SetText(FText::FromString(TEXT("Quests")));
	ListHeader->SetTextTransformPolicy(ETextTransformPolicy::ToUpper);
	if (UVerticalBoxSlot* ListHeaderSlot = ListBoxRoot->AddChildToVerticalBox(ListHeader))
	{
		ListHeaderSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 10.0f));
	}

	QuestListBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("QuestListBox"));
	ListBoxRoot->AddChildToVerticalBox(QuestListBox);

	DetailPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DetailPanel"));
	if (UHorizontalBoxSlot* DetailSlot = ContentRow->AddChildToHorizontalBox(DetailPanel))
	{
		DetailSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}
	DetailPanel->SetPadding(FMargin(16.0f));

	UVerticalBox* DetailBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("DetailBox"));
	DetailPanel->SetContent(DetailBox);

	SelectedQuestTitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SelectedQuestTitleText"));
	SelectedQuestTitleText->SetAutoWrapText(true);
	if (UVerticalBoxSlot* TitleSlot = DetailBox->AddChildToVerticalBox(SelectedQuestTitleText))
	{
		TitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));
	}

	SelectedQuestStatusText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SelectedQuestStatusText"));
	SelectedQuestStatusText->SetTextTransformPolicy(ETextTransformPolicy::ToUpper);
	if (UVerticalBoxSlot* StatusSlot = DetailBox->AddChildToVerticalBox(SelectedQuestStatusText))
	{
		StatusSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
	}

	SelectedQuestDescriptionText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SelectedQuestDescriptionText"));
	SelectedQuestDescriptionText->SetAutoWrapText(true);
	if (UVerticalBoxSlot* DescriptionSlot = DetailBox->AddChildToVerticalBox(SelectedQuestDescriptionText))
	{
		DescriptionSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 14.0f));
	}

	SelectedObjectivesBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("SelectedObjectivesBox"));
	if (UVerticalBoxSlot* ObjectivesSlot = DetailBox->AddChildToVerticalBox(SelectedObjectivesBox))
	{
		ObjectivesSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));
	}

	EmptyStateText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EmptyStateText"));
	EmptyStateText->SetText(FText::FromString(TEXT("No quests have been started yet.")));
	EmptyStateText->SetAutoWrapText(true);
}

void UDQSDefaultQuestLogWidget::ApplyTheme()
{
	const UDQSUIThemeAsset* Theme = GetResolvedTheme();
	const FLinearColor Accent = Theme ? Theme->QuestAccentColor : FallbackAccentColor;
	const FLinearColor Surface = Theme ? Theme->QuestSurfaceColor : FallbackSurfaceColor;
	const FLinearColor Text = Theme ? Theme->QuestTextColor : FallbackTextColor;
	const FLinearColor Muted = Theme ? Theme->QuestMutedTextColor : FallbackMutedTextColor;

	if (QuestLogChrome)
	{
		QuestLogChrome->SetBrushColor(Surface);
	}

	if (QuestLogPanelSize && Theme)
	{
		QuestLogPanelSize->SetWidthOverride(Theme->LogWidth);
		QuestLogPanelSize->SetHeightOverride(Theme->LogHeight);
	}

	if (HeaderText)
	{
		HeaderText->SetColorAndOpacity(FSlateColor(Accent));
	}

	if (SubheaderText)
	{
		SubheaderText->SetColorAndOpacity(FSlateColor(Muted));
	}

	if (SelectedQuestTitleText)
	{
		SelectedQuestTitleText->SetColorAndOpacity(FSlateColor(Text));
	}

	if (SelectedQuestStatusText)
	{
		SelectedQuestStatusText->SetColorAndOpacity(FSlateColor(Accent));
	}

	if (SelectedQuestDescriptionText)
	{
		SelectedQuestDescriptionText->SetColorAndOpacity(FSlateColor(Text));
	}

	if (EmptyStateText)
	{
		EmptyStateText->SetColorAndOpacity(FSlateColor(Muted));
	}

	if (ListPanel)
	{
		ListPanel->SetBrushColor(Theme ? Theme->QuestCardColor : FallbackCardColor);
	}

	if (DetailPanel)
	{
		DetailPanel->SetBrushColor(Theme ? Theme->QuestCardColor : FallbackCardColor);
	}
}

void UDQSDefaultQuestLogWidget::RefreshFromState()
{
	if (!QuestLogChrome || !QuestListBox || !SelectedQuestTitleText || !SelectedQuestStatusText || !SelectedQuestDescriptionText || !SelectedObjectivesBox || !ViewModel || !WidgetTree)
	{
		return;
	}

	QuestLogChrome->SetVisibility(bIsOpen ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	QuestListBox->ClearChildren();
	SelectedObjectivesBox->ClearChildren();

	if (ViewModel->GetQuestStates().Num() == 0)
	{
		QuestListBox->AddChildToVerticalBox(EmptyStateText);
		SelectedQuestTitleText->SetText(FText::FromString(TEXT("No quests")));
		SelectedQuestStatusText->SetText(FText::GetEmpty());
		SelectedQuestDescriptionText->SetText(FText::FromString(TEXT("Start a quest to populate the journal.")));
		return;
	}

	const FDQSQuestRuntimeState* SelectedState = ViewModel->GetQuestStates().Find(SelectedQuestId);
	if (!SelectedState)
	{
		SelectedQuestId = ViewModel->GetQuestStates().CreateConstIterator()->Key;
		SelectedState = ViewModel->GetQuestStates().Find(SelectedQuestId);
	}

	for (const TPair<FName, FDQSQuestRuntimeState>& Pair : ViewModel->GetQuestStates())
	{
		const FDQSQuestRuntimeState& State = Pair.Value;
		FText QuestTitle = FText::FromName(State.QuestId);
		if (const UQuestGraphAsset* QuestAsset = State.QuestAsset.LoadSynchronous())
		{
			if (!QuestAsset->Metadata.DisplayName.IsEmpty())
			{
				QuestTitle = QuestAsset->Metadata.DisplayName;
			}
		}

		UBorder* Row = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
		Row->SetBrushColor(Pair.Key == SelectedQuestId ? FLinearColor(0.20f, 0.17f, 0.10f, 0.98f) : FallbackCardColor);
		Row->SetPadding(FMargin(10.0f));

		UVerticalBox* RowBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
		Row->SetContent(RowBox);

		UTextBlock* TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		TitleText->SetText(QuestTitle);
		TitleText->SetColorAndOpacity(FSlateColor(FallbackTextColor));
		TitleText->SetAutoWrapText(true);
		RowBox->AddChildToVerticalBox(TitleText);

		UTextBlock* StatusText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		StatusText->SetText(GetQuestStatusText(State.Status));
		StatusText->SetColorAndOpacity(FSlateColor(FallbackMutedTextColor));
		if (UVerticalBoxSlot* StatusSlot = RowBox->AddChildToVerticalBox(StatusText))
		{
			StatusSlot->SetPadding(FMargin(0.0f, 4.0f, 0.0f, 0.0f));
		}

		if (UVerticalBoxSlot* RowSlot = QuestListBox->AddChildToVerticalBox(Row))
		{
			RowSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));
		}
	}

	if (SelectedState)
	{
		FText QuestTitle = FText::FromName(SelectedState->QuestId);
		if (const UQuestGraphAsset* QuestAsset = SelectedState->QuestAsset.LoadSynchronous())
		{
			if (!QuestAsset->Metadata.DisplayName.IsEmpty())
			{
				QuestTitle = QuestAsset->Metadata.DisplayName;
			}

			for (const FDQSQuestNode& Node : QuestAsset->Nodes)
			{
				if (Node.NodeType == EDQSQuestNodeType::Objective && !Node.Description.IsEmpty())
				{
					UTextBlock* ObjectiveText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
					ObjectiveText->SetAutoWrapText(true);
					ObjectiveText->SetColorAndOpacity(FSlateColor(FallbackTextColor));

					FText DisplayText = Node.Description;
					if (Node.ObjectiveTag.IsValid())
					{
						const int32 CurrentCount = SelectedState->ObjectiveProgress.FindRef(Node.ObjectiveTag.GetTagName());
						DisplayText = FText::Format(
							FText::FromString(TEXT("- {0} ({1}/{2})")),
							Node.Description,
							FText::AsNumber(CurrentCount),
							FText::AsNumber(Node.TargetCount));
					}
					else
					{
						DisplayText = FText::Format(FText::FromString(TEXT("- {0}")), Node.Description);
					}

					ObjectiveText->SetText(DisplayText);
					if (UVerticalBoxSlot* ObjectiveSlot = SelectedObjectivesBox->AddChildToVerticalBox(ObjectiveText))
					{
						ObjectiveSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));
					}
				}
			}
		}

		SelectedQuestTitleText->SetText(QuestTitle);
		SelectedQuestStatusText->SetText(GetQuestStatusText(SelectedState->Status));
		SelectedQuestDescriptionText->SetText(GetQuestDescriptionText(*SelectedState));
	}
}

void UDQSDefaultQuestLogWidget::SetQuestLogOpen(const bool bInOpen)
{
	bIsOpen = bInOpen;
	SetVisibility(bIsOpen ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (QuestLogChrome)
	{
		QuestLogChrome->SetVisibility(bIsOpen ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UDQSDefaultQuestLogWidget::SelectBestQuest()
{
	if (!ViewModel)
	{
		return;
	}

	if (!SelectedQuestId.IsNone() && ViewModel->GetQuestStates().Contains(SelectedQuestId))
	{
		return;
	}

	if (!ViewModel->GetLatestUpdate().QuestId.IsNone() && ViewModel->GetQuestStates().Contains(ViewModel->GetLatestUpdate().QuestId))
	{
		SelectedQuestId = ViewModel->GetLatestUpdate().QuestId;
		return;
	}

	for (const TPair<FName, FDQSQuestRuntimeState>& Pair : ViewModel->GetQuestStates())
	{
		if (Pair.Value.Status == EDQSQuestStatus::Active)
		{
			SelectedQuestId = Pair.Key;
			return;
		}
	}

	if (ViewModel->GetQuestStates().Num() > 0)
	{
		SelectedQuestId = ViewModel->GetQuestStates().CreateConstIterator()->Key;
	}
}
