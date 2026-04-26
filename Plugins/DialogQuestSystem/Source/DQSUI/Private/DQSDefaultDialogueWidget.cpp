#include "DQSDefaultDialogueWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "DQSSpeakerStyleAsset.h"
#include "DQSDialogueViewModel.h"
#include "DQSUIThemeAsset.h"
#include "Engine/Texture2D.h"
#include "Widgets/SWidget.h"

namespace
{
	void StyleButton(UButton* Button, const FLinearColor& Tint)
	{
		if (!Button)
		{
			return;
		}

		FButtonStyle Style = Button->GetStyle();
		Style.Normal.TintColor = FSlateColor(Tint);
		Style.Hovered.TintColor = FSlateColor(Tint * 1.08f);
		Style.Pressed.TintColor = FSlateColor(Tint * 0.92f);
		Style.Disabled.TintColor = FSlateColor(Tint * 0.85f);
		Button->SetStyle(Style);
		Button->SetBackgroundColor(Tint);
	}

	void ApplyFontIfSet(UTextBlock* TextBlock, const FSlateFontInfo& Font)
	{
		if (!TextBlock)
		{
			return;
		}

		const bool bHasUsableFontObject = Font.FontObject != nullptr;
		const bool bHasUsableTypeface = Font.TypefaceFontName != NAME_None;
		if (Font.Size > 0 && (bHasUsableFontObject || bHasUsableTypeface))
		{
			TextBlock->SetFont(Font);
		}
	}
}

void UDQSChoiceButtonProxy::HandleClick()
{
	if (OwnerWidget)
	{
		OwnerWidget->ChooseDialogueOption(ChoiceIndex);
	}
}

TSharedRef<SWidget> UDQSDefaultDialogueWidget::RebuildWidget()
{
	BuildDefaultLayout();
	return Super::RebuildWidget();
}

void UDQSDefaultDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BuildDefaultLayout();
	EnsureChoiceButtons();

	if (ViewModel)
	{
		ViewModel->OnStateChanged.AddDynamic(this, &UDQSDefaultDialogueWidget::HandleDialogueStateChanged);
	}

	if (ContinueButton)
	{
		ContinueButton->OnClicked.RemoveAll(this);
		ContinueButton->OnClicked.AddDynamic(this, &UDQSDefaultDialogueWidget::HandleContinueClicked);
	}

	HandleDialogueStateChanged();
}

void UDQSDefaultDialogueWidget::HandleContinueClicked()
{
	ContinueDialogue();
}

void UDQSDefaultDialogueWidget::HandleDialogueStateChanged()
{
	ApplyTheme();
	RefreshFromState();
}

void UDQSDefaultDialogueWidget::BuildDefaultLayout()
{
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return;
	}

	UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
	WidgetTree->RootWidget = RootCanvas;

	UOverlay* RootOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("RootOverlay"));
	if (UCanvasPanelSlot* RootOverlaySlot = RootCanvas->AddChildToCanvas(RootOverlay))
	{
		RootOverlaySlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		RootOverlaySlot->SetOffsets(FMargin(0.0f));
	}

	BackdropBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("BackdropBorder"));
	RootOverlay->AddChildToOverlay(BackdropBorder);

	DialogueWidthBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("DialogueWidthBox"));
	if (UOverlaySlot* WidthSlot = RootOverlay->AddChildToOverlay(DialogueWidthBox))
	{
		WidthSlot->SetHorizontalAlignment(HAlign_Center);
		WidthSlot->SetVerticalAlignment(VAlign_Bottom);
		WidthSlot->SetPadding(FMargin(32.0f, 24.0f, 32.0f, 30.0f));
	}

	DialogueFrame = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DialogueFrame"));
	DialogueWidthBox->AddChild(DialogueFrame);

	DialogueSurface = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DialogueSurface"));
	DialogueFrame->SetContent(DialogueSurface);

	UHorizontalBox* MainRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("MainRow"));
	DialogueSurface->SetContent(MainRow);

	PortraitSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("PortraitSizeBox"));
	if (UHorizontalBoxSlot* PortraitSlot = MainRow->AddChildToHorizontalBox(PortraitSizeBox))
	{
		PortraitSlot->SetPadding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
		PortraitSlot->SetVerticalAlignment(VAlign_Fill);
	}

	PortraitFrame = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("PortraitFrame"));
	PortraitSizeBox->AddChild(PortraitFrame);

	PortraitImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("PortraitImage"));
	PortraitFrame->SetContent(PortraitImage);

	UVerticalBox* ContentBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ContentBox"));
	if (UHorizontalBoxSlot* ContentSlot = MainRow->AddChildToHorizontalBox(ContentBox))
	{
		ContentSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}

	CaptionTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CaptionTextBlock"));
	CaptionTextBlock->SetText(FText::FromString(TEXT("CONVERSATION")));
	CaptionTextBlock->SetTextTransformPolicy(ETextTransformPolicy::ToUpper);
	if (UVerticalBoxSlot* CaptionSlot = ContentBox->AddChildToVerticalBox(CaptionTextBlock))
	{
		CaptionSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));
	}

	SpeakerTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SpeakerTextBlock"));
	SpeakerTextBlock->SetTextTransformPolicy(ETextTransformPolicy::ToUpper);
	SpeakerTextBlock->SetAutoWrapText(true);
	if (UVerticalBoxSlot* SpeakerSlot = ContentBox->AddChildToVerticalBox(SpeakerTextBlock))
	{
		SpeakerSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 14.0f));
	}

	BodyPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("BodyPanel"));
	if (UVerticalBoxSlot* BodyPanelSlot = ContentBox->AddChildToVerticalBox(BodyPanel))
	{
		BodyPanelSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
	}

	BodyScrollBox = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("BodyScrollBox"));
	BodyPanel->SetContent(BodyScrollBox);

	BodyTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("BodyTextBlock"));
	BodyTextBlock->SetAutoWrapText(true);
	BodyTextBlock->SetJustification(ETextJustify::Left);
	BodyScrollBox->AddChild(BodyTextBlock);

	ResponsesPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ResponsesPanel"));
	if (UVerticalBoxSlot* ResponsesPanelSlot = ContentBox->AddChildToVerticalBox(ResponsesPanel))
	{
		ResponsesPanelSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
	}

	UVerticalBox* ResponsesBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ResponsesBox"));
	ResponsesPanel->SetContent(ResponsesBox);

	ResponsesLabelText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ResponsesLabelText"));
	ResponsesLabelText->SetText(FText::FromString(TEXT("RESPONSES")));
	ResponsesLabelText->SetTextTransformPolicy(ETextTransformPolicy::ToUpper);
	if (UVerticalBoxSlot* ResponsesLabelSlot = ResponsesBox->AddChildToVerticalBox(ResponsesLabelText))
	{
		ResponsesLabelSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 10.0f));
	}

	ResponsesScrollBox = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("ResponsesScrollBox"));
	ResponsesBox->AddChildToVerticalBox(ResponsesScrollBox);

	ResponsesList = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ResponsesList"));
	ResponsesScrollBox->AddChild(ResponsesList);

	ContinueArea = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ContinueArea"));
	ContentBox->AddChildToVerticalBox(ContinueArea);

	UHorizontalBox* FooterRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("FooterRow"));
	ContinueArea->SetContent(FooterRow);

	HintTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HintTextBlock"));
	HintTextBlock->SetAutoWrapText(true);
	if (UHorizontalBoxSlot* HintSlot = FooterRow->AddChildToHorizontalBox(HintTextBlock))
	{
		HintSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		HintSlot->SetVerticalAlignment(VAlign_Center);
		HintSlot->SetPadding(FMargin(0.0f, 0.0f, 18.0f, 0.0f));
	}

	ContinueButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("ContinueButton"));
	if (UHorizontalBoxSlot* ContinueSlot = FooterRow->AddChildToHorizontalBox(ContinueButton))
	{
		ContinueSlot->SetHorizontalAlignment(HAlign_Right);
		ContinueSlot->SetVerticalAlignment(VAlign_Center);
	}

	ContinueButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ContinueButtonText"));
	ContinueButtonText->SetText(FText::FromString(TEXT("Continue")));
	ContinueButton->SetContent(ContinueButtonText);
}

void UDQSDefaultDialogueWidget::EnsureChoiceButtons()
{
	if (!ResponsesList || !WidgetTree)
	{
		return;
	}

	const int32 RequiredButtonCount = FMath::Max(GetChoiceCount(), 2);
	for (int32 Index = ChoiceButtons.Num(); Index < RequiredButtonCount; ++Index)
	{
		UButton* ChoiceButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), *FString::Printf(TEXT("ChoiceButton%d"), Index));
		UTextBlock* ChoiceText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *FString::Printf(TEXT("ChoiceText%d"), Index));
		UDQSChoiceButtonProxy* ChoiceProxy = NewObject<UDQSChoiceButtonProxy>(this);
		ChoiceProxy->OwnerWidget = this;
		ChoiceProxy->ChoiceIndex = Index;
		ChoiceText->SetAutoWrapText(true);
		ChoiceText->SetJustification(ETextJustify::Left);
		ChoiceButton->SetContent(ChoiceText);
		ChoiceButton->OnClicked.AddDynamic(ChoiceProxy, &UDQSChoiceButtonProxy::HandleClick);
		if (UVerticalBoxSlot* ChoiceSlot = ResponsesList->AddChildToVerticalBox(ChoiceButton))
		{
			ChoiceSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));
		}

		ChoiceButtons.Add(ChoiceButton);
		ChoiceButtonTexts.Add(ChoiceText);
		ChoiceButtonProxies.Add(ChoiceProxy);
	}
}

void UDQSDefaultDialogueWidget::ApplyTheme()
{
	const UDQSUIThemeAsset* Theme = GetResolvedTheme();
	const UDQSSpeakerStyleAsset* SpeakerStyle = GetSpeakerStyle().LoadSynchronous();

	const FLinearColor Accent = SpeakerStyle && SpeakerStyle->bOverrideAccentColor ? SpeakerStyle->AccentColor : (Theme ? Theme->AccentColor : FallbackAccentColor);
	const FLinearColor Surface = Theme ? Theme->SurfaceColor : FallbackSurfaceColor;
	const FLinearColor Panel = Theme ? Theme->PanelColor : FallbackPanelColor;
	const FLinearColor Text = Theme ? Theme->TextColor : FallbackTextColor;
	const FLinearColor Muted = Theme ? Theme->MutedTextColor : FallbackMutedTextColor;
	const FLinearColor Choice = Theme ? Theme->ChoiceColor : FallbackPanelColor;
	const FLinearColor DisabledChoice = Theme ? Theme->DisabledChoiceColor : FLinearColor(0.24f, 0.18f, 0.17f, 0.94f);

	if (BackdropBorder)
	{
		BackdropBorder->SetBrushColor(Theme ? Theme->BackdropColor : FLinearColor(0.02f, 0.02f, 0.04f, 0.54f));
	}

	if (DialogueWidthBox)
	{
		DialogueWidthBox->SetWidthOverride(Theme ? Theme->DialogueWidth : 1120.0f);
		DialogueWidthBox->SetMaxDesiredHeight(Theme ? Theme->DialogueMaxHeight : 420.0f);
	}

	if (DialogueFrame)
	{
		DialogueFrame->SetBrush(Theme ? Theme->FrameBrush : FSlateBrush());
		DialogueFrame->SetBrushColor(Theme ? Theme->FrameColor : Accent);
		DialogueFrame->SetPadding(FMargin(2.0f));
	}

	if (DialogueSurface)
	{
		DialogueSurface->SetBrush(Theme ? Theme->PanelBrush : FSlateBrush());
		DialogueSurface->SetBrushColor(Surface);
		DialogueSurface->SetPadding(Theme ? Theme->PanelPadding : FMargin(22.0f, 18.0f, 22.0f, 18.0f));
	}

	if (PortraitSizeBox)
	{
		const FVector2D PortraitSize = SpeakerStyle ? SpeakerStyle->PortraitSize : (Theme ? Theme->PortraitSize : FVector2D(220.0f, 260.0f));
		PortraitSizeBox->SetWidthOverride(PortraitSize.X);
		PortraitSizeBox->SetHeightOverride(PortraitSize.Y);
	}

	if (PortraitFrame)
	{
		PortraitFrame->SetBrush(Theme ? Theme->PortraitFrameBrush : FSlateBrush());
		PortraitFrame->SetBrushColor(Panel);
		PortraitFrame->SetPadding(FMargin(8.0f));
	}

	if (CaptionTextBlock)
	{
		CaptionTextBlock->SetVisibility(ESlateVisibility::Collapsed);
		CaptionTextBlock->SetColorAndOpacity(FSlateColor(Muted));
		ApplyFontIfSet(CaptionTextBlock, Theme ? Theme->CaptionFont : FSlateFontInfo());
	}

	if (SpeakerTextBlock)
	{
		SpeakerTextBlock->SetColorAndOpacity(FSlateColor(Accent));
		ApplyFontIfSet(SpeakerTextBlock, SpeakerStyle ? SpeakerStyle->SpeakerFont : (Theme ? Theme->SpeakerFont : FSlateFontInfo()));
	}

	if (BodyTextBlock)
	{
		BodyTextBlock->SetColorAndOpacity(FSlateColor(Text));
		ApplyFontIfSet(BodyTextBlock, SpeakerStyle ? SpeakerStyle->BodyFont : (Theme ? Theme->BodyFont : FSlateFontInfo()));
	}

	if (BodyPanel)
	{
		BodyPanel->SetBrushColor(Panel);
		BodyPanel->SetPadding(Theme ? Theme->BodyPadding : FMargin(18.0f, 16.0f));
	}

	if (BodyScrollBox)
	{
		BodyScrollBox->SetAlwaysShowScrollbar(false);
	}

	if (ResponsesPanel)
	{
		ResponsesPanel->SetBrushColor(Panel);
		ResponsesPanel->SetPadding(Theme ? Theme->ChoicePadding : FMargin(14.0f, 12.0f));
	}

	if (ResponsesLabelText)
	{
		ResponsesLabelText->SetColorAndOpacity(FSlateColor(Muted));
		ApplyFontIfSet(ResponsesLabelText, Theme ? Theme->CaptionFont : FSlateFontInfo());
	}

	if (ResponsesScrollBox)
	{
		ResponsesScrollBox->SetAlwaysShowScrollbar(false);
	}

	if (ContinueArea)
	{
		ContinueArea->SetBrushColor(Panel);
		ContinueArea->SetPadding(FMargin(14.0f, 12.0f));
	}

	if (HintTextBlock)
	{
		HintTextBlock->SetVisibility(ESlateVisibility::Collapsed);
		HintTextBlock->SetColorAndOpacity(FSlateColor(Muted));
		ApplyFontIfSet(HintTextBlock, Theme ? Theme->BodyFont : FSlateFontInfo());
	}

	if (ContinueButtonText)
	{
		ContinueButtonText->SetColorAndOpacity(FSlateColor(FLinearColor(0.12f, 0.08f, 0.03f, 1.0f)));
		ApplyFontIfSet(ContinueButtonText, Theme ? Theme->ButtonFont : FSlateFontInfo());
	}

	for (int32 Index = 0; Index < ChoiceButtons.Num(); ++Index)
	{
		StyleButton(ChoiceButtons[Index], Choice);
		if (ChoiceButtonTexts.IsValidIndex(Index))
		{
			ChoiceButtonTexts[Index]->SetColorAndOpacity(FSlateColor(Text));
			ApplyFontIfSet(ChoiceButtonTexts[Index], Theme ? Theme->ChoiceFont : FSlateFontInfo());
		}
	}

	if (ContinueButton)
	{
		StyleButton(ContinueButton, Accent);
	}

	for (int32 Index = 0; Index < ChoiceButtons.Num(); ++Index)
	{
		if (ChoiceButtons.IsValidIndex(Index) && !IsChoiceEnabled(Index))
		{
			StyleButton(ChoiceButtons[Index], DisabledChoice);
		}
	}
}

void UDQSDefaultDialogueWidget::RefreshFromState()
{
	if (!BackdropBorder || !DialogueFrame || !DialogueSurface || !SpeakerTextBlock || !BodyTextBlock || !ContinueArea || !ContinueButton || !HintTextBlock)
	{
		return;
	}

	EnsureChoiceButtons();

	const bool bDialogueActive = ViewModel != nullptr && ViewModel->IsDialogueActive();
	SetVisibility(ESlateVisibility::Visible);

	const ESlateVisibility ChromeVisibility = bDialogueActive ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	BackdropBorder->SetVisibility(ChromeVisibility);
	DialogueFrame->SetVisibility(ChromeVisibility);
	DialogueSurface->SetVisibility(ChromeVisibility);

	SpeakerTextBlock->SetText(GetSpeakerNameText());
	BodyTextBlock->SetText(GetDialogueBodyText());

	const UDQSUIThemeAsset* Theme = GetResolvedTheme();
	if (BodyScrollBox)
	{
		BodyScrollBox->SetScrollBarVisibility(ESlateVisibility::Collapsed);
		if (Theme)
		{
			if (USizeBox* ParentSize = Cast<USizeBox>(BodyScrollBox->GetParent()))
			{
				ParentSize->SetMaxDesiredHeight(Theme->BodyMaxHeight);
			}
		}
	}

	UTexture2D* PortraitTexture = GetSpeakerPortrait().LoadSynchronous();
	if (PortraitImage)
	{
		if (PortraitTexture)
		{
			PortraitImage->SetBrushFromTexture(PortraitTexture, true);
		}
		else
		{
			PortraitImage->SetBrush(FSlateBrush());
		}
	}
	if (PortraitSizeBox)
	{
		PortraitSizeBox->SetVisibility(bDialogueActive && PortraitTexture ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	const int32 ChoiceCount = GetChoiceCount();
	const bool bShowResponses = bDialogueActive && ChoiceCount > 0;
	const bool bShowContinue = bDialogueActive && GetContinueButtonVisibility() == ESlateVisibility::Visible && ChoiceCount == 0;

	if (ResponsesPanel)
	{
		ResponsesPanel->SetVisibility(bShowResponses ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (ContinueArea)
	{
		ContinueArea->SetVisibility(bShowContinue ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	for (int32 Index = 0; Index < ChoiceButtons.Num(); ++Index)
	{
		UButton* ChoiceButton = ChoiceButtons[Index];
		UTextBlock* ChoiceText = ChoiceButtonTexts.IsValidIndex(Index) ? ChoiceButtonTexts[Index] : nullptr;
		if (!ChoiceButton || !ChoiceText)
		{
			continue;
		}

		const bool bHasChoice = bShowResponses && Index < ChoiceCount;
		const bool bEnabled = bHasChoice && IsChoiceEnabled(Index);
		ChoiceButton->SetVisibility(bHasChoice ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		ChoiceButton->SetIsEnabled(bEnabled);
		ChoiceButton->SetToolTipText(GetChoiceDisabledReason(Index));

		if (bHasChoice)
		{
			ChoiceText->SetText(FText::Format(FText::FromString(TEXT("{0}. {1}")), FText::AsNumber(Index + 1), GetChoiceText(Index)));
		}
	}

	ContinueButton->SetVisibility(bShowContinue ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	ContinueButton->SetIsEnabled(IsContinueButtonEnabled());
}
