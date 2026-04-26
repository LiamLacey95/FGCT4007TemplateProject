#pragma once

#include "CoreMinimal.h"
#include "DQSDialogueWidgetBase.h"
#include "DQSDefaultDialogueWidget.generated.h"

class UBorder;
class UButton;
class UImage;
class UScrollBox;
class USizeBox;
class UTextBlock;
class UVerticalBox;

UCLASS()
class DQSUI_API UDQSChoiceButtonProxy : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TObjectPtr<class UDQSDefaultDialogueWidget> OwnerWidget;

	UPROPERTY(Transient)
	int32 ChoiceIndex = INDEX_NONE;

	UFUNCTION()
	void HandleClick();
};

UCLASS(BlueprintType, Blueprintable)
class DQSUI_API UDQSDefaultDialogueWidget : public UDQSDialogueWidgetBase
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Style")
	FLinearColor FallbackAccentColor = FLinearColor(0.80f, 0.55f, 0.27f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Style")
	FLinearColor FallbackSurfaceColor = FLinearColor(0.07f, 0.08f, 0.11f, 0.94f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Style")
	FLinearColor FallbackPanelColor = FLinearColor(0.10f, 0.11f, 0.14f, 0.96f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Style")
	FLinearColor FallbackTextColor = FLinearColor(0.93f, 0.91f, 0.86f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Style")
	FLinearColor FallbackMutedTextColor = FLinearColor(0.67f, 0.66f, 0.62f, 1.0f);

protected:
	UFUNCTION()
	void HandleContinueClicked();

	UFUNCTION()
	void HandleDialogueStateChanged();

private:
	void BuildDefaultLayout();
	void RefreshFromState();
	void EnsureChoiceButtons();
	void ApplyTheme();

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UBorder> BackdropBorder;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UBorder> DialogueFrame;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UBorder> DialogueSurface;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<USizeBox> DialogueWidthBox;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<USizeBox> PortraitSizeBox;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UBorder> PortraitFrame;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UImage> PortraitImage;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UTextBlock> CaptionTextBlock;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UTextBlock> SpeakerTextBlock;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UBorder> BodyPanel;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UTextBlock> BodyTextBlock;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UScrollBox> BodyScrollBox;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UBorder> ResponsesPanel;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UTextBlock> ResponsesLabelText;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UScrollBox> ResponsesScrollBox;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UVerticalBox> ResponsesList;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UBorder> ContinueArea;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UTextBlock> HintTextBlock;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UButton> ContinueButton;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UTextBlock> ContinueButtonText;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UButton>> ChoiceButtons;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> ChoiceButtonTexts;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UDQSChoiceButtonProxy>> ChoiceButtonProxies;
};
