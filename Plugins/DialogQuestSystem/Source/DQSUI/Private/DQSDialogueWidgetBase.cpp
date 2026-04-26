#include "DQSDialogueWidgetBase.h"

#include "DQSDialogueViewModel.h"
#include "DQSSpeakerStyleAsset.h"
#include "DQSUISettings.h"
#include "DQSUIThemeAsset.h"
#include "DialogQuestSubsystem.h"
#include "Engine/GameInstance.h"

void UDQSDialogueWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (!ViewModel)
	{
		ViewModel = NewObject<UDQSDialogueViewModel>(this);
	}

	ViewModel->InitializeFromWorldContext(this);
	ViewModel->OnStateChanged.RemoveAll(this);
	ViewModel->OnStateChanged.AddDynamic(this, &UDQSDialogueWidgetBase::HandleViewModelStateChanged);
	HandleViewModelStateChanged();
}

void UDQSDialogueWidgetBase::ContinueDialogue()
{
	if (UDialogQuestSubsystem* Subsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<UDialogQuestSubsystem>() : nullptr)
	{
		Subsystem->ContinueDialogue();
	}
}

void UDQSDialogueWidgetBase::ChooseDialogueOption(const int32 ChoiceIndex)
{
	if (UDialogQuestSubsystem* Subsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<UDialogQuestSubsystem>() : nullptr)
	{
		Subsystem->ChooseDialogueOption(ChoiceIndex);
	}
}

FText UDQSDialogueWidgetBase::GetSpeakerNameText() const
{
	return ViewModel ? ViewModel->GetCurrentLine().SpeakerName : FText::GetEmpty();
}

FText UDQSDialogueWidgetBase::GetDialogueBodyText() const
{
	return ViewModel ? ViewModel->GetCurrentLine().Text : FText::GetEmpty();
}

ESlateVisibility UDQSDialogueWidgetBase::GetDialogueRootVisibility() const
{
	return ViewModel && ViewModel->IsDialogueActive() ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
}

ESlateVisibility UDQSDialogueWidgetBase::GetContinueButtonVisibility() const
{
	if (!ViewModel || !ViewModel->IsDialogueActive())
	{
		return ESlateVisibility::Collapsed;
	}

	return ViewModel->GetCurrentLine().bCanContinue ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
}

bool UDQSDialogueWidgetBase::IsContinueButtonEnabled() const
{
	return ViewModel && ViewModel->IsDialogueActive() && ViewModel->GetCurrentLine().bCanContinue;
}

int32 UDQSDialogueWidgetBase::GetChoiceCount() const
{
	return ViewModel ? ViewModel->GetCurrentLine().Choices.Num() : 0;
}

FText UDQSDialogueWidgetBase::GetChoiceText(const int32 ChoiceIndex) const
{
	if (!ViewModel || !ViewModel->GetCurrentLine().Choices.IsValidIndex(ChoiceIndex))
	{
		return FText::GetEmpty();
	}

	return ViewModel->GetCurrentLine().Choices[ChoiceIndex].Text;
}

bool UDQSDialogueWidgetBase::IsChoiceEnabled(const int32 ChoiceIndex) const
{
	if (!ViewModel || !ViewModel->GetCurrentLine().Choices.IsValidIndex(ChoiceIndex))
	{
		return false;
	}

	return ViewModel->GetCurrentLine().Choices[ChoiceIndex].bIsAvailable;
}

FText UDQSDialogueWidgetBase::GetChoiceDisabledReason(const int32 ChoiceIndex) const
{
	if (!ViewModel || !ViewModel->GetCurrentLine().Choices.IsValidIndex(ChoiceIndex))
	{
		return FText::GetEmpty();
	}

	return ViewModel->GetCurrentLine().Choices[ChoiceIndex].DisabledReason;
}

ESlateVisibility UDQSDialogueWidgetBase::GetChoiceButtonVisibility(const int32 ChoiceIndex) const
{
	if (!ViewModel || !ViewModel->IsDialogueActive())
	{
		return ESlateVisibility::Collapsed;
	}

	return ViewModel->GetCurrentLine().Choices.IsValidIndex(ChoiceIndex) ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
}

ESlateVisibility UDQSDialogueWidgetBase::GetResponsesSectionVisibility() const
{
	return GetChoiceCount() > 0 && ViewModel && ViewModel->IsDialogueActive() ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
}

ESlateVisibility UDQSDialogueWidgetBase::GetPortraitVisibility() const
{
	return GetSpeakerPortrait().IsNull() ? ESlateVisibility::Collapsed : ESlateVisibility::Visible;
}

TSoftObjectPtr<UTexture2D> UDQSDialogueWidgetBase::GetSpeakerPortrait() const
{
	return ViewModel ? ViewModel->GetCurrentLine().Portrait : nullptr;
}

TSoftObjectPtr<UDQSSpeakerStyleAsset> UDQSDialogueWidgetBase::GetSpeakerStyle() const
{
	return ViewModel ? ViewModel->GetCurrentLine().SpeakerStyle : nullptr;
}

UDQSUIThemeAsset* UDQSDialogueWidgetBase::GetResolvedTheme() const
{
	if (!ThemeOverride.IsNull())
	{
		return ThemeOverride.LoadSynchronous();
	}

	if (const UDQSUISettings* Settings = UDQSUISettings::Get())
	{
		return Settings->DefaultTheme.LoadSynchronous();
	}

	return nullptr;
}

FText UDQSDialogueWidgetBase::GetChoice1Text() const
{
	return GetChoiceText(0);
}

FText UDQSDialogueWidgetBase::GetChoice2Text() const
{
	return GetChoiceText(1);
}

bool UDQSDialogueWidgetBase::IsChoice1Enabled() const
{
	return IsChoiceEnabled(0);
}

bool UDQSDialogueWidgetBase::IsChoice2Enabled() const
{
	return IsChoiceEnabled(1);
}

ESlateVisibility UDQSDialogueWidgetBase::GetChoice1Visibility() const
{
	return GetChoiceButtonVisibility(0);
}

ESlateVisibility UDQSDialogueWidgetBase::GetChoice2Visibility() const
{
	return GetChoiceButtonVisibility(1);
}

void UDQSDialogueWidgetBase::HandleViewModelStateChanged()
{
	ApplyResolvedTheme(GetResolvedTheme());
	OnDialogueStateChanged();
}
