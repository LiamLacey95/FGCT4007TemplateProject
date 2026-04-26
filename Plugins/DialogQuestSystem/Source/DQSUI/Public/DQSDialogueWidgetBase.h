#pragma once

#include "Blueprint/UserWidget.h"
#include "Components/SlateWrapperTypes.h"
#include "DQSDialogueWidgetBase.generated.h"

class UDQSDialogueViewModel;
class UDQSSpeakerStyleAsset;
class UDQSUIThemeAsset;
class UTexture2D;

UCLASS(Abstract, Blueprintable)
class DQSUI_API UDQSDialogueWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System|UI")
	void ContinueDialogue();

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System|UI")
	void ChooseDialogueOption(int32 ChoiceIndex);

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	TObjectPtr<UDQSDialogueViewModel> ViewModel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|UI")
	TSoftObjectPtr<UDQSUIThemeAsset> ThemeOverride;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI")
	FText GetSpeakerNameText() const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI")
	FText GetDialogueBodyText() const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI")
	ESlateVisibility GetDialogueRootVisibility() const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI")
	ESlateVisibility GetContinueButtonVisibility() const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI")
	bool IsContinueButtonEnabled() const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI")
	int32 GetChoiceCount() const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI")
	FText GetChoiceText(int32 ChoiceIndex) const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI")
	bool IsChoiceEnabled(int32 ChoiceIndex) const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI")
	FText GetChoiceDisabledReason(int32 ChoiceIndex) const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI")
	ESlateVisibility GetChoiceButtonVisibility(int32 ChoiceIndex) const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI")
	ESlateVisibility GetResponsesSectionVisibility() const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI")
	ESlateVisibility GetPortraitVisibility() const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI")
	TSoftObjectPtr<UTexture2D> GetSpeakerPortrait() const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI")
	TSoftObjectPtr<UDQSSpeakerStyleAsset> GetSpeakerStyle() const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI")
	UDQSUIThemeAsset* GetResolvedTheme() const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI", meta = (DeprecatedFunction, DeprecationMessage = "Use GetChoiceText with GetChoiceCount instead."))
	FText GetChoice1Text() const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI", meta = (DeprecatedFunction, DeprecationMessage = "Use GetChoiceText with GetChoiceCount instead."))
	FText GetChoice2Text() const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI", meta = (DeprecatedFunction, DeprecationMessage = "Use IsChoiceEnabled with a dynamic choice index instead."))
	bool IsChoice1Enabled() const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI", meta = (DeprecatedFunction, DeprecationMessage = "Use IsChoiceEnabled with a dynamic choice index instead."))
	bool IsChoice2Enabled() const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI", meta = (DeprecatedFunction, DeprecationMessage = "Use GetChoiceButtonVisibility with a dynamic choice index instead."))
	ESlateVisibility GetChoice1Visibility() const;

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI", meta = (DeprecatedFunction, DeprecationMessage = "Use GetChoiceButtonVisibility with a dynamic choice index instead."))
	ESlateVisibility GetChoice2Visibility() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialog Quest System|UI")
	void OnDialogueStateChanged();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialog Quest System|UI")
	void ApplyResolvedTheme(UDQSUIThemeAsset* Theme);

protected:
	UFUNCTION()
	void HandleViewModelStateChanged();
};
