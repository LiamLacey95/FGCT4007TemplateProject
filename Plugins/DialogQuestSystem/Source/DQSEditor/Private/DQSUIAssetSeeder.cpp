#include "DQSUIAssetSeeder.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "WidgetBlueprint.h"
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
#include "DQSDefaultDialogueWidget.h"
#include "DQSDefaultQuestLogWidget.h"
#include "DQSDefaultQuestTrackerWidget.h"
#include "DQSUIThemeAsset.h"
#include "WidgetBlueprintFactory.h"
#include "HAL/FileManager.h"
#include "Interfaces/IPluginManager.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

namespace
{
	bool SaveAssetPackage(UPackage* Package, UObject* Asset, const FString& PackageFilePath)
	{
		if (!Package || !Asset)
		{
			return false;
		}

		Package->MarkPackageDirty();
		FAssetRegistryModule::AssetCreated(Asset);

		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_NoError;
		return UPackage::SavePackage(Package, Asset, *PackageFilePath, SaveArgs);
	}

	template <typename AssetType>
	AssetType* CreateAssetPackage(const FString& PackageName, const FString& PackageFilePath, const FName AssetName)
	{
		if (AssetType* ExistingAsset = LoadObject<AssetType>(nullptr, *FString::Printf(TEXT("%s.%s"), *PackageName, *AssetName.ToString())))
		{
			return ExistingAsset;
		}

		UPackage* Package = CreatePackage(*PackageName);
		if (!Package)
		{
			return nullptr;
		}

		AssetType* Asset = NewObject<AssetType>(Package, AssetName, RF_Public | RF_Standalone | RF_Transactional);
		return SaveAssetPackage(Package, Asset, PackageFilePath) ? Asset : nullptr;
	}

	UWidgetBlueprint* CreateWidgetBlueprint(const FString& PackageName, const FString& PackageFilePath, const FName AssetName, UClass* ParentClass)
	{
		if (UWidgetBlueprint* ExistingAsset = LoadObject<UWidgetBlueprint>(nullptr, *FString::Printf(TEXT("%s.%s"), *PackageName, *AssetName.ToString())))
		{
			return ExistingAsset;
		}

		UPackage* Package = CreatePackage(*PackageName);
		if (!Package)
		{
			return nullptr;
		}

		UWidgetBlueprintFactory* Factory = NewObject<UWidgetBlueprintFactory>();
		Factory->ParentClass = ParentClass;
		UWidgetBlueprint* WidgetBlueprint = Cast<UWidgetBlueprint>(Factory->FactoryCreateNew(UWidgetBlueprint::StaticClass(), Package, AssetName, RF_Public | RF_Standalone | RF_Transactional, nullptr, GWarn));
		if (!WidgetBlueprint)
		{
			return nullptr;
		}

		return SaveAssetPackage(Package, WidgetBlueprint, PackageFilePath) ? WidgetBlueprint : nullptr;
	}

	void FinalizeWidgetBlueprint(UWidgetBlueprint* WidgetBlueprint, const FString& PackageFilePath)
	{
		if (!WidgetBlueprint)
		{
			return;
		}

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(WidgetBlueprint);
		FKismetEditorUtilities::CompileBlueprint(WidgetBlueprint);
		SaveAssetPackage(WidgetBlueprint->GetPackage(), WidgetBlueprint, PackageFilePath);
	}

	void ResetWidgetBlueprintTree(UWidgetBlueprint* WidgetBlueprint)
	{
		if (!WidgetBlueprint)
		{
			return;
		}

		WidgetBlueprint->Modify();
		WidgetBlueprint->WidgetTree = NewObject<UWidgetTree>(WidgetBlueprint, UWidgetTree::StaticClass(), TEXT("WidgetTree"), RF_Transactional);
	}

	void BuildDialogueWidgetBlueprint(UWidgetBlueprint* WidgetBlueprint)
	{
		if (!WidgetBlueprint || !WidgetBlueprint->WidgetTree)
		{
			return;
		}

		UWidgetTree* WidgetTree = WidgetBlueprint->WidgetTree;

		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
		WidgetTree->RootWidget = RootCanvas;

		UOverlay* RootOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("RootOverlay"));
		if (UCanvasPanelSlot* RootOverlaySlot = RootCanvas->AddChildToCanvas(RootOverlay))
		{
			RootOverlaySlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
			RootOverlaySlot->SetOffsets(FMargin(0.0f));
		}

		UBorder* BackdropBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("BackdropBorder"));
		RootOverlay->AddChildToOverlay(BackdropBorder);

		USizeBox* DialogueWidthBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("DialogueWidthBox"));
		DialogueWidthBox->SetWidthOverride(1120.0f);
		DialogueWidthBox->SetMaxDesiredHeight(420.0f);
		if (UOverlaySlot* WidthSlot = RootOverlay->AddChildToOverlay(DialogueWidthBox))
		{
			WidthSlot->SetHorizontalAlignment(HAlign_Center);
			WidthSlot->SetVerticalAlignment(VAlign_Bottom);
			WidthSlot->SetPadding(FMargin(32.0f, 24.0f, 32.0f, 30.0f));
		}

		UBorder* DialogueFrame = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DialogueFrame"));
		DialogueFrame->SetPadding(FMargin(2.0f));
		DialogueWidthBox->AddChild(DialogueFrame);

		UBorder* DialogueSurface = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DialogueSurface"));
		DialogueFrame->SetContent(DialogueSurface);

		UHorizontalBox* MainRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("MainRow"));
		DialogueSurface->SetContent(MainRow);

		USizeBox* PortraitSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("PortraitSizeBox"));
		PortraitSizeBox->SetWidthOverride(220.0f);
		PortraitSizeBox->SetHeightOverride(260.0f);
		if (UHorizontalBoxSlot* PortraitSlot = MainRow->AddChildToHorizontalBox(PortraitSizeBox))
		{
			PortraitSlot->SetPadding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
		}

		UBorder* PortraitFrame = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("PortraitFrame"));
		PortraitFrame->SetPadding(FMargin(8.0f));
		PortraitSizeBox->AddChild(PortraitFrame);

		UImage* PortraitImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("PortraitImage"));
		PortraitFrame->SetContent(PortraitImage);

		UVerticalBox* ContentBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ContentBox"));
		if (UHorizontalBoxSlot* ContentSlot = MainRow->AddChildToHorizontalBox(ContentBox))
		{
			ContentSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		}

		UTextBlock* CaptionTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CaptionTextBlock"));
		CaptionTextBlock->SetText(FText::FromString(TEXT("CONVERSATION")));
		CaptionTextBlock->SetTextTransformPolicy(ETextTransformPolicy::ToUpper);
		if (UVerticalBoxSlot* CaptionSlot = ContentBox->AddChildToVerticalBox(CaptionTextBlock))
		{
			CaptionSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));
		}

		UTextBlock* SpeakerTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SpeakerTextBlock"));
		SpeakerTextBlock->SetText(FText::FromString(TEXT("SPEAKER")));
		SpeakerTextBlock->SetTextTransformPolicy(ETextTransformPolicy::ToUpper);
		if (UVerticalBoxSlot* SpeakerSlot = ContentBox->AddChildToVerticalBox(SpeakerTextBlock))
		{
			SpeakerSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 14.0f));
		}

		UBorder* BodyPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("BodyPanel"));
		BodyPanel->SetPadding(FMargin(18.0f, 16.0f));
		if (UVerticalBoxSlot* BodyPanelSlot = ContentBox->AddChildToVerticalBox(BodyPanel))
		{
			BodyPanelSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
		}

		UScrollBox* BodyScrollBox = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("BodyScrollBox"));
		BodyPanel->SetContent(BodyScrollBox);

		UTextBlock* BodyTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("BodyTextBlock"));
		BodyTextBlock->SetAutoWrapText(true);
		BodyTextBlock->SetText(FText::FromString(TEXT("Dialogue body text appears here.")));
		BodyScrollBox->AddChild(BodyTextBlock);

		UBorder* ResponsesPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ResponsesPanel"));
		ResponsesPanel->SetPadding(FMargin(14.0f, 12.0f));
		if (UVerticalBoxSlot* ResponsesSlot = ContentBox->AddChildToVerticalBox(ResponsesPanel))
		{
			ResponsesSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
		}

		UVerticalBox* ResponsesBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ResponsesBox"));
		ResponsesPanel->SetContent(ResponsesBox);

		UTextBlock* ResponsesLabelText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ResponsesLabelText"));
		ResponsesLabelText->SetText(FText::FromString(TEXT("RESPONSES")));
		ResponsesLabelText->SetTextTransformPolicy(ETextTransformPolicy::ToUpper);
		if (UVerticalBoxSlot* ResponsesLabelSlot = ResponsesBox->AddChildToVerticalBox(ResponsesLabelText))
		{
			ResponsesLabelSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 10.0f));
		}

		UScrollBox* ResponsesScrollBox = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("ResponsesScrollBox"));
		ResponsesBox->AddChildToVerticalBox(ResponsesScrollBox);

		UVerticalBox* ResponsesList = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ResponsesList"));
		ResponsesScrollBox->AddChild(ResponsesList);

		UBorder* ContinueArea = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ContinueArea"));
		ContinueArea->SetPadding(FMargin(14.0f, 12.0f));
		ContentBox->AddChildToVerticalBox(ContinueArea);

		UHorizontalBox* FooterRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("FooterRow"));
		ContinueArea->SetContent(FooterRow);

		UTextBlock* HintTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HintTextBlock"));
		HintTextBlock->SetText(FText::FromString(TEXT("Continue when you're ready.")));
		HintTextBlock->SetAutoWrapText(true);
		if (UHorizontalBoxSlot* HintSlot = FooterRow->AddChildToHorizontalBox(HintTextBlock))
		{
			HintSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			HintSlot->SetPadding(FMargin(0.0f, 0.0f, 18.0f, 0.0f));
		}

		UButton* ContinueButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("ContinueButton"));
		if (UHorizontalBoxSlot* ContinueSlot = FooterRow->AddChildToHorizontalBox(ContinueButton))
		{
			ContinueSlot->SetVerticalAlignment(VAlign_Center);
		}

		UTextBlock* ContinueButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ContinueButtonText"));
		ContinueButtonText->SetText(FText::FromString(TEXT("Continue")));
		ContinueButton->SetContent(ContinueButtonText);
	}

	void BuildTrackerWidgetBlueprint(UWidgetBlueprint* WidgetBlueprint)
	{
		if (!WidgetBlueprint || !WidgetBlueprint->WidgetTree)
		{
			return;
		}

		UWidgetTree* WidgetTree = WidgetBlueprint->WidgetTree;
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("QuestTrackerRoot"));
		WidgetTree->RootWidget = RootCanvas;

		USizeBox* PanelSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("QuestTrackerPanelSize"));
		PanelSize->SetWidthOverride(320.0f);
		if (UCanvasPanelSlot* PanelSlot = RootCanvas->AddChildToCanvas(PanelSize))
		{
			PanelSlot->SetAnchors(FAnchors(1.0f, 0.0f, 1.0f, 0.0f));
			PanelSlot->SetAlignment(FVector2D(1.0f, 0.0f));
			PanelSlot->SetPosition(FVector2D(-28.0f, 26.0f));
			PanelSlot->SetAutoSize(true);
		}

		UBorder* TrackerPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("TrackerPanel"));
		TrackerPanel->SetPadding(FMargin(16.0f));
		PanelSize->AddChild(TrackerPanel);

		UVerticalBox* ContentBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("TrackerContentBox"));
		TrackerPanel->SetContent(ContentBox);

		UTextBlock* HeaderText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HeaderText"));
		HeaderText->SetText(FText::FromString(TEXT("ACTIVE CASES")));
		HeaderText->SetTextTransformPolicy(ETextTransformPolicy::ToUpper);
		if (UVerticalBoxSlot* HeaderSlot = ContentBox->AddChildToVerticalBox(HeaderText))
		{
			HeaderSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 10.0f));
		}

		UBorder* LatestPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("LatestPanel"));
		LatestPanel->SetPadding(FMargin(12.0f));
		if (UVerticalBoxSlot* LatestSlot = ContentBox->AddChildToVerticalBox(LatestPanel))
		{
			LatestSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
		}

		UVerticalBox* LatestBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("LatestBox"));
		LatestPanel->SetContent(LatestBox);

		UTextBlock* LatestTitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("LatestTitleText"));
		LatestTitleText->SetText(FText::FromString(TEXT("No active quests")));
		if (UVerticalBoxSlot* TitleSlot = LatestBox->AddChildToVerticalBox(LatestTitleText))
		{
			TitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 4.0f));
		}

		UTextBlock* LatestMessageText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("LatestMessageText"));
		LatestMessageText->SetText(FText::FromString(TEXT("Quest updates will appear here.")));
		LatestMessageText->SetAutoWrapText(true);
		LatestBox->AddChildToVerticalBox(LatestMessageText);

		UTextBlock* TrackedLabelText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TrackedLabelText"));
		TrackedLabelText->SetText(FText::FromString(TEXT("Tracked")));
		TrackedLabelText->SetTextTransformPolicy(ETextTransformPolicy::ToUpper);
		if (UVerticalBoxSlot* TrackedLabelSlot = ContentBox->AddChildToVerticalBox(TrackedLabelText))
		{
			TrackedLabelSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));
		}

		UVerticalBox* ActiveQuestList = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ActiveQuestList"));
		ContentBox->AddChildToVerticalBox(ActiveQuestList);
	}

	void BuildQuestLogWidgetBlueprint(UWidgetBlueprint* WidgetBlueprint)
	{
		if (!WidgetBlueprint || !WidgetBlueprint->WidgetTree)
		{
			return;
		}

		UWidgetTree* WidgetTree = WidgetBlueprint->WidgetTree;
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

		USizeBox* PanelSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("QuestLogPanelSize"));
		PanelSize->SetWidthOverride(920.0f);
		PanelSize->SetHeightOverride(560.0f);
		if (UOverlaySlot* PanelSlot = RootOverlay->AddChildToOverlay(PanelSize))
		{
			PanelSlot->SetHorizontalAlignment(HAlign_Center);
			PanelSlot->SetVerticalAlignment(VAlign_Center);
			PanelSlot->SetPadding(FMargin(28.0f, 36.0f));
		}

		UBorder* QuestLogChrome = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("QuestLogChrome"));
		QuestLogChrome->SetPadding(FMargin(18.0f));
		PanelSize->AddChild(QuestLogChrome);

		UVerticalBox* RootBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("RootBox"));
		QuestLogChrome->SetContent(RootBox);

		UHorizontalBox* HeaderRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HeaderRow"));
		if (UVerticalBoxSlot* HeaderSlot = RootBox->AddChildToVerticalBox(HeaderRow))
		{
			HeaderSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 14.0f));
		}

		UTextBlock* HeaderText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HeaderText"));
		HeaderText->SetText(FText::FromString(TEXT("QUEST LOG")));
		HeaderText->SetTextTransformPolicy(ETextTransformPolicy::ToUpper);
		HeaderRow->AddChildToHorizontalBox(HeaderText);

		USpacer* Spacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass(), TEXT("HeaderSpacer"));
		if (UHorizontalBoxSlot* SpacerSlot = HeaderRow->AddChildToHorizontalBox(Spacer))
		{
			SpacerSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		}

		UButton* CloseButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("CloseButton"));
		HeaderRow->AddChildToHorizontalBox(CloseButton);
		UTextBlock* CloseText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CloseButtonText"));
		CloseText->SetText(FText::FromString(TEXT("Close")));
		CloseButton->SetContent(CloseText);

		UTextBlock* SubheaderText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SubheaderText"));
		SubheaderText->SetText(FText::FromString(TEXT("Review active, completed, and failed quests in one place.")));
		SubheaderText->SetAutoWrapText(true);
		if (UVerticalBoxSlot* SubheaderSlot = RootBox->AddChildToVerticalBox(SubheaderText))
		{
			SubheaderSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 14.0f));
		}

		UHorizontalBox* ContentRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("ContentRow"));
		RootBox->AddChildToVerticalBox(ContentRow);

		UBorder* ListPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ListPanel"));
		ListPanel->SetPadding(FMargin(12.0f));
		if (UHorizontalBoxSlot* ListSlot = ContentRow->AddChildToHorizontalBox(ListPanel))
		{
			ListSlot->SetPadding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
		}

		UVerticalBox* ListRoot = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ListRoot"));
		ListPanel->SetContent(ListRoot);
		UTextBlock* ListLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ListLabel"));
		ListLabel->SetText(FText::FromString(TEXT("Quests")));
		ListLabel->SetTextTransformPolicy(ETextTransformPolicy::ToUpper);
		if (UVerticalBoxSlot* LabelSlot = ListRoot->AddChildToVerticalBox(ListLabel))
		{
			LabelSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 10.0f));
		}
		UVerticalBox* QuestListBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("QuestListBox"));
		ListRoot->AddChildToVerticalBox(QuestListBox);

		UBorder* DetailPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DetailPanel"));
		DetailPanel->SetPadding(FMargin(16.0f));
		if (UHorizontalBoxSlot* DetailSlot = ContentRow->AddChildToHorizontalBox(DetailPanel))
		{
			DetailSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		}

		UVerticalBox* DetailBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("DetailBox"));
		DetailPanel->SetContent(DetailBox);

		UTextBlock* SelectedQuestTitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SelectedQuestTitleText"));
		SelectedQuestTitleText->SetText(FText::FromString(TEXT("No quests")));
		SelectedQuestTitleText->SetAutoWrapText(true);
		if (UVerticalBoxSlot* TitleSlot = DetailBox->AddChildToVerticalBox(SelectedQuestTitleText))
		{
			TitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));
		}

		UTextBlock* SelectedQuestStatusText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SelectedQuestStatusText"));
		SelectedQuestStatusText->SetTextTransformPolicy(ETextTransformPolicy::ToUpper);
		if (UVerticalBoxSlot* StatusSlot = DetailBox->AddChildToVerticalBox(SelectedQuestStatusText))
		{
			StatusSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
		}

		UTextBlock* SelectedQuestDescriptionText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SelectedQuestDescriptionText"));
		SelectedQuestDescriptionText->SetAutoWrapText(true);
		if (UVerticalBoxSlot* DescriptionSlot = DetailBox->AddChildToVerticalBox(SelectedQuestDescriptionText))
		{
			DescriptionSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 14.0f));
		}

		UVerticalBox* SelectedObjectivesBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("SelectedObjectivesBox"));
		if (UVerticalBoxSlot* ObjectiveSlot = DetailBox->AddChildToVerticalBox(SelectedObjectivesBox))
		{
			ObjectiveSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));
		}

		UTextBlock* EmptyStateText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EmptyStateText"));
		EmptyStateText->SetText(FText::FromString(TEXT("No quests have been started yet.")));
		EmptyStateText->SetAutoWrapText(true);
	}
}

void DQSUIAssetSeeder::EnsurePluginUIAssets()
{
	IPluginManager& PluginManager = IPluginManager::Get();
	const TSharedPtr<IPlugin> Plugin = PluginManager.FindPlugin(TEXT("DialogQuestSystem"));
	if (!Plugin.IsValid())
	{
		return;
	}

	const FString PluginContentDir = Plugin->GetContentDir();
	const FString UIDir = PluginContentDir / TEXT("UI");
	const FString ThemeDir = UIDir / TEXT("Themes");
	IFileManager::Get().MakeDirectory(*UIDir, true);
	IFileManager::Get().MakeDirectory(*ThemeDir, true);

	const FString ThemePackageName = TEXT("/DialogQuestSystem/UI/Themes/DA_DQS_DefaultUITheme");
	const FString ThemePackageFile = ThemeDir / TEXT("DA_DQS_DefaultUITheme.uasset");
	if (!LoadObject<UDQSUIThemeAsset>(nullptr, TEXT("/DialogQuestSystem/UI/Themes/DA_DQS_DefaultUITheme.DA_DQS_DefaultUITheme")))
	{
		if (UDQSUIThemeAsset* ThemeAsset = CreateAssetPackage<UDQSUIThemeAsset>(ThemePackageName, ThemePackageFile, TEXT("DA_DQS_DefaultUITheme")))
		{
			SaveAssetPackage(ThemeAsset->GetPackage(), ThemeAsset, ThemePackageFile);
		}
	}

	const FString DialoguePackageName = TEXT("/DialogQuestSystem/UI/WBP_DQSDialogue_Default");
	const FString DialoguePackageFile = UIDir / TEXT("WBP_DQSDialogue_Default.uasset");
	if (UWidgetBlueprint* DialogueBlueprint = CreateWidgetBlueprint(DialoguePackageName, DialoguePackageFile, TEXT("WBP_DQSDialogue_Default"), UDQSDefaultDialogueWidget::StaticClass()))
	{
		ResetWidgetBlueprintTree(DialogueBlueprint);
		BuildDialogueWidgetBlueprint(DialogueBlueprint);
		FinalizeWidgetBlueprint(DialogueBlueprint, DialoguePackageFile);
	}

	const FString TrackerPackageName = TEXT("/DialogQuestSystem/UI/WBP_DQSQuestTracker_Default");
	const FString TrackerPackageFile = UIDir / TEXT("WBP_DQSQuestTracker_Default.uasset");
	if (UWidgetBlueprint* TrackerBlueprint = CreateWidgetBlueprint(TrackerPackageName, TrackerPackageFile, TEXT("WBP_DQSQuestTracker_Default"), UDQSDefaultQuestTrackerWidget::StaticClass()))
	{
		ResetWidgetBlueprintTree(TrackerBlueprint);
		BuildTrackerWidgetBlueprint(TrackerBlueprint);
		FinalizeWidgetBlueprint(TrackerBlueprint, TrackerPackageFile);
	}

	const FString QuestLogWidgetPackageName = TEXT("/DialogQuestSystem/UI/WBP_DQSQuestLog_Default");
	const FString QuestLogWidgetPackageFile = UIDir / TEXT("WBP_DQSQuestLog_Default.uasset");
	if (UWidgetBlueprint* QuestLogWidgetBlueprint = CreateWidgetBlueprint(QuestLogWidgetPackageName, QuestLogWidgetPackageFile, TEXT("WBP_DQSQuestLog_Default"), UDQSDefaultQuestLogWidget::StaticClass()))
	{
		ResetWidgetBlueprintTree(QuestLogWidgetBlueprint);
		BuildQuestLogWidgetBlueprint(QuestLogWidgetBlueprint);
		FinalizeWidgetBlueprint(QuestLogWidgetBlueprint, QuestLogWidgetPackageFile);
	}
}
