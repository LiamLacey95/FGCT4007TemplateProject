#include "DQSDefaultQuestTrackerWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "DQSQuestGraphAsset.h"
#include "DQSQuestViewModel.h"
#include "DQSUIThemeAsset.h"
#include "Widgets/SWidget.h"

TSharedRef<SWidget> UDQSDefaultQuestTrackerWidget::RebuildWidget()
{
	BuildDefaultLayout();
	return Super::RebuildWidget();
}

void UDQSDefaultQuestTrackerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BuildDefaultLayout();

	if (ViewModel)
	{
		ViewModel->OnStateChanged.AddDynamic(this, &UDQSDefaultQuestTrackerWidget::HandleQuestStateChanged);
	}

	HandleQuestStateChanged();
}

void UDQSDefaultQuestTrackerWidget::HandleQuestStateChanged()
{
	ApplyTheme();
	RefreshFromState();
}

void UDQSDefaultQuestTrackerWidget::BuildDefaultLayout()
{
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return;
	}

	UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("QuestTrackerRoot"));
	WidgetTree->RootWidget = RootCanvas;

	USizeBox* PanelSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("QuestTrackerPanelSize"));
	if (UCanvasPanelSlot* PanelSlot = RootCanvas->AddChildToCanvas(PanelSize))
	{
		PanelSlot->SetAnchors(FAnchors(1.0f, 0.0f, 1.0f, 0.0f));
		PanelSlot->SetAlignment(FVector2D(1.0f, 0.0f));
		PanelSlot->SetPosition(FVector2D(-28.0f, 26.0f));
		PanelSlot->SetAutoSize(true);
	}

	TrackerPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("TrackerPanel"));
	TrackerPanel->SetPadding(FMargin(16.0f));
	PanelSize->AddChild(TrackerPanel);

	UVerticalBox* ContentBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("TrackerContentBox"));
	TrackerPanel->SetContent(ContentBox);

	HeaderText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HeaderText"));
	HeaderText->SetText(FText::FromString(TEXT("ACTIVE CASES")));
	HeaderText->SetTextTransformPolicy(ETextTransformPolicy::ToUpper);
	if (UVerticalBoxSlot* HeaderSlot = ContentBox->AddChildToVerticalBox(HeaderText))
	{
		HeaderSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 10.0f));
	}

	LatestPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("LatestPanel"));
	LatestPanel->SetPadding(FMargin(12.0f));
	if (UVerticalBoxSlot* LatestSlot = ContentBox->AddChildToVerticalBox(LatestPanel))
	{
		LatestSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
	}

	UVerticalBox* LatestBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("LatestBox"));
	LatestPanel->SetContent(LatestBox);

	LatestTitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("LatestTitleText"));
	if (UVerticalBoxSlot* LatestTitleSlot = LatestBox->AddChildToVerticalBox(LatestTitleText))
	{
		LatestTitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 4.0f));
	}

	LatestMessageText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("LatestMessageText"));
	LatestMessageText->SetAutoWrapText(true);
	LatestBox->AddChildToVerticalBox(LatestMessageText);

	TrackedLabelText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TrackedLabelText"));
	TrackedLabelText->SetText(FText::FromString(TEXT("Tracked")));
	TrackedLabelText->SetTextTransformPolicy(ETextTransformPolicy::ToUpper);
	if (UVerticalBoxSlot* TrackedLabelSlot = ContentBox->AddChildToVerticalBox(TrackedLabelText))
	{
		TrackedLabelSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));
	}

	ActiveQuestList = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ActiveQuestList"));
	ContentBox->AddChildToVerticalBox(ActiveQuestList);
}

void UDQSDefaultQuestTrackerWidget::ApplyTheme()
{
	const UDQSUIThemeAsset* Theme = GetResolvedTheme();
	const FLinearColor Accent = Theme ? Theme->QuestAccentColor : FallbackAccentColor;
	const FLinearColor Surface = Theme ? Theme->QuestSurfaceColor : FallbackSurfaceColor;
	const FLinearColor Text = Theme ? Theme->QuestTextColor : FallbackTextColor;
	const FLinearColor Muted = Theme ? Theme->QuestMutedTextColor : FallbackMutedTextColor;

	if (TrackerPanel)
	{
		TrackerPanel->SetBrushColor(Surface);
	}

	if (HeaderText)
	{
		HeaderText->SetColorAndOpacity(FSlateColor(Accent));
	}

	if (TrackedLabelText)
	{
		TrackedLabelText->SetColorAndOpacity(FSlateColor(Muted));
	}

	if (LatestPanel)
	{
		LatestPanel->SetBrushColor(Theme ? Theme->QuestCardColor : FallbackSurfaceColor);
	}

	if (LatestTitleText)
	{
		LatestTitleText->SetColorAndOpacity(FSlateColor(Text));
	}

	if (LatestMessageText)
	{
		LatestMessageText->SetColorAndOpacity(FSlateColor(Muted));
	}
}

void UDQSDefaultQuestTrackerWidget::RefreshFromState()
{
	if (!TrackerPanel || !LatestTitleText || !LatestMessageText || !ActiveQuestList || !ViewModel || !WidgetTree)
	{
		return;
	}

	const FDQSQuestStatusUpdate& LatestUpdate = ViewModel->GetLatestUpdate();
	if (!LatestUpdate.Title.IsEmpty())
	{
		LatestTitleText->SetText(LatestUpdate.Title);
		LatestMessageText->SetText(LatestUpdate.Message);
	}
	else
	{
		LatestTitleText->SetText(FText::FromString(TEXT("No active quests")));
		LatestMessageText->SetText(FText::FromString(TEXT("Quest updates will appear here.")));
	}

	ActiveQuestList->ClearChildren();

	int32 ActiveQuestCount = 0;
	for (const TPair<FName, FDQSQuestRuntimeState>& Pair : ViewModel->GetQuestStates())
	{
		const FDQSQuestRuntimeState& State = Pair.Value;
		if (State.Status != EDQSQuestStatus::Active)
		{
			continue;
		}

		++ActiveQuestCount;
		FText QuestTitle = FText::FromName(State.QuestId);
		if (const UQuestGraphAsset* QuestAsset = State.QuestAsset.LoadSynchronous())
		{
			if (!QuestAsset->Metadata.DisplayName.IsEmpty())
			{
				QuestTitle = QuestAsset->Metadata.DisplayName;
			}
		}

		UBorder* QuestRow = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
		QuestRow->SetBrushColor(FLinearColor(0.12f, 0.13f, 0.16f, 0.96f));
		QuestRow->SetPadding(FMargin(10.0f));

		UTextBlock* QuestLine = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		QuestLine->SetAutoWrapText(true);
		QuestLine->SetColorAndOpacity(FSlateColor(FallbackTextColor));
		QuestLine->SetText(QuestTitle);
		QuestRow->SetContent(QuestLine);

		if (UVerticalBoxSlot* QuestSlot = ActiveQuestList->AddChildToVerticalBox(QuestRow))
		{
			QuestSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));
		}
	}

	if (ActiveQuestCount == 0)
	{
		UTextBlock* EmptyLine = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		EmptyLine->SetColorAndOpacity(FSlateColor(FallbackMutedTextColor));
		EmptyLine->SetText(FText::FromString(TEXT("No active quests right now.")));
		ActiveQuestList->AddChildToVerticalBox(EmptyLine);
	}
}
