#include "DQSCreationWizard.h"

#include "Framework/Application/SlateApplication.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"

namespace
{
	bool PromptForOptionsWindow(const TSharedRef<SWidget>& Content, const FText& Title)
	{
		TSharedRef<SWindow> Window = SNew(SWindow)
			.Title(Title)
			.ClientSize(FVector2D(520.0f, 320.0f))
			.SupportsMinimize(false)
			.SupportsMaximize(false)
			[
				Content
			];

		FSlateApplication::Get().AddModalWindow(Window, nullptr);
		return true;
	}

	template <typename PatternType>
	TSharedRef<SWidget> BuildWizardLayout(
		const FString& AssetName,
		const TArray<TSharedPtr<FString>>& PatternOptions,
		const TSharedPtr<FString>& InitialPattern,
		FText* DisplayNameText,
		FText* SecondaryText,
		FName* SecondaryName,
		TSharedPtr<FString>* SelectedPattern,
		bool* bAccepted,
		const FText& SecondaryTextLabel,
		const FText& SecondaryNameLabel)
	{
		TSharedPtr<SEditableTextBox> DisplayNameBox;
		TSharedPtr<SEditableTextBox> SecondaryTextBox;
		TSharedPtr<SEditableTextBox> SecondaryNameBox;

		return SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight().Padding(16.0f, 16.0f, 16.0f, 8.0f)
			[
				SNew(STextBlock)
				.Text(FText::Format(FText::FromString(TEXT("Create a starter graph for {0}")), FText::FromString(AssetName)))
			]
			+ SVerticalBox::Slot().AutoHeight().Padding(16.0f, 8.0f)
			[
				SNew(STextBlock).Text(FText::FromString(TEXT("Display Name")))
			]
			+ SVerticalBox::Slot().AutoHeight().Padding(16.0f, 0.0f, 16.0f, 8.0f)
			[
				SAssignNew(DisplayNameBox, SEditableTextBox)
				.Text(*DisplayNameText)
			]
			+ SVerticalBox::Slot().AutoHeight().Padding(16.0f, 8.0f)
			[
				SNew(STextBlock).Text(FText::FromString(TEXT("Starter Pattern")))
			]
			+ SVerticalBox::Slot().AutoHeight().Padding(16.0f, 0.0f, 16.0f, 8.0f)
			[
				SNew(SComboBox<TSharedPtr<FString>>)
				.OptionsSource(&PatternOptions)
				.InitiallySelectedItem(InitialPattern)
				.OnGenerateWidget_Lambda([](const TSharedPtr<FString> Item)
				{
					return SNew(STextBlock).Text(FText::FromString(Item.IsValid() ? *Item : FString()));
				})
				.OnSelectionChanged_Lambda([SelectedPattern](const TSharedPtr<FString> NewSelection, ESelectInfo::Type)
				{
					*SelectedPattern = NewSelection;
				})
				[
					SNew(STextBlock).Text_Lambda([SelectedPattern]()
					{
						return FText::FromString((*SelectedPattern).IsValid() ? *(*SelectedPattern) : FString(TEXT("Select a pattern")));
					})
				]
			]
			+ SVerticalBox::Slot().AutoHeight().Padding(16.0f, 8.0f)
			[
				SNew(STextBlock).Text(SecondaryTextLabel)
			]
			+ SVerticalBox::Slot().AutoHeight().Padding(16.0f, 0.0f, 16.0f, 8.0f)
			[
				SAssignNew(SecondaryTextBox, SEditableTextBox)
				.Text(*SecondaryText)
			]
			+ SVerticalBox::Slot().AutoHeight().Padding(16.0f, 8.0f)
			[
				SNew(STextBlock).Text(SecondaryNameLabel)
			]
			+ SVerticalBox::Slot().AutoHeight().Padding(16.0f, 0.0f, 16.0f, 16.0f)
			[
				SAssignNew(SecondaryNameBox, SEditableTextBox)
				.Text(FText::FromName(*SecondaryName))
			]
			+ SVerticalBox::Slot().FillHeight(1.0f)
			[
				SNew(SBox)
			]
			+ SVerticalBox::Slot().AutoHeight().Padding(16.0f)
			[
				SNew(SUniformGridPanel)
				.SlotPadding(FMargin(8.0f, 0.0f))
				+ SUniformGridPanel::Slot(0, 0)
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Cancel")))
					.OnClicked_Lambda([bAccepted]()
					{
						*bAccepted = false;
						FSlateApplication::Get().RequestDestroyWindow(FSlateApplication::Get().GetActiveTopLevelWindow().ToSharedRef());
						return FReply::Handled();
					})
				]
				+ SUniformGridPanel::Slot(1, 0)
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("Create Starter Graph")))
					.OnClicked_Lambda([DisplayNameText, SecondaryText, SecondaryName, SelectedPattern, bAccepted, DisplayNameBox, SecondaryTextBox, SecondaryNameBox]()
					{
						*DisplayNameText = DisplayNameBox->GetText();
						*SecondaryText = SecondaryTextBox->GetText();
						*SecondaryName = FName(*SecondaryNameBox->GetText().ToString());
						*bAccepted = (*SelectedPattern).IsValid();
						FSlateApplication::Get().RequestDestroyWindow(FSlateApplication::Get().GetActiveTopLevelWindow().ToSharedRef());
						return FReply::Handled();
					})
				]
			];
	}
}

bool FDQSCreationWizard::PromptForDialogueOptions(const FString& AssetName, FDQSDialogueCreationOptions& OutOptions)
{
	TArray<TSharedPtr<FString>> PatternOptions;
	PatternOptions.Add(MakeShared<FString>(TEXT("Linear conversation")));
	PatternOptions.Add(MakeShared<FString>(TEXT("Accept / decline")));
	PatternOptions.Add(MakeShared<FString>(TEXT("Branching conversation")));
	TSharedPtr<FString> SelectedPattern = PatternOptions[1];
	bool bAccepted = false;

	TSharedRef<SWidget> Content = BuildWizardLayout<EDQSDialogueStarterPattern>(
		AssetName,
		PatternOptions,
		SelectedPattern,
		&OutOptions.DisplayName,
		&OutOptions.SpeakerDisplayName,
		&OutOptions.SpeakerId,
		&SelectedPattern,
		&bAccepted,
		FText::FromString(TEXT("Speaker Display Name")),
		FText::FromString(TEXT("Speaker Id")));

	OutOptions.DisplayName = FText::FromString(AssetName);
	PromptForOptionsWindow(Content, FText::FromString(TEXT("New Dialogue Starter")));
	if (bAccepted)
	{
		OutOptions.Pattern = SelectedPattern == PatternOptions[0] ? EDQSDialogueStarterPattern::LinearConversation
			: (SelectedPattern == PatternOptions[2] ? EDQSDialogueStarterPattern::BranchingConversation : EDQSDialogueStarterPattern::AcceptDecline);
	}

	return bAccepted;
}

bool FDQSCreationWizard::PromptForQuestOptions(const FString& AssetName, FDQSQuestCreationOptions& OutOptions)
{
	TArray<TSharedPtr<FString>> PatternOptions;
	PatternOptions.Add(MakeShared<FString>(TEXT("Simple objective")));
	PatternOptions.Add(MakeShared<FString>(TEXT("Accept and complete loop")));
	PatternOptions.Add(MakeShared<FString>(TEXT("Branching quest")));
	TSharedPtr<FString> SelectedPattern = PatternOptions[0];
	bool bAccepted = false;

	FText SecondaryText = OutOptions.ObjectiveText;
	FName SecondaryName = OutOptions.ObjectiveTag;
	TSharedRef<SWidget> Content = BuildWizardLayout<EDQSQuestStarterPattern>(
		AssetName,
		PatternOptions,
		SelectedPattern,
		&OutOptions.DisplayName,
		&SecondaryText,
		&SecondaryName,
		&SelectedPattern,
		&bAccepted,
		FText::FromString(TEXT("Objective Text")),
		FText::FromString(TEXT("Objective Tag")));

	OutOptions.DisplayName = FText::FromString(AssetName);
	PromptForOptionsWindow(Content, FText::FromString(TEXT("New Quest Starter")));
	if (bAccepted)
	{
		OutOptions.ObjectiveText = SecondaryText;
		OutOptions.ObjectiveTag = SecondaryName;
		OutOptions.Pattern = SelectedPattern == PatternOptions[2] ? EDQSQuestStarterPattern::BranchingQuest
			: (SelectedPattern == PatternOptions[1] ? EDQSQuestStarterPattern::AcceptAndCompleteLoop : EDQSQuestStarterPattern::SimpleObjective);
	}

	return bAccepted;
}
