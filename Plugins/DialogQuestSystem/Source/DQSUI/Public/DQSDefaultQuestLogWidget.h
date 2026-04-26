#pragma once

#include "CoreMinimal.h"
#include "DQSQuestLogWidgetBase.h"
#include "DQSDefaultQuestLogWidget.generated.h"

class UBorder;
class UButton;
class USizeBox;
class UTextBlock;
class UVerticalBox;

UCLASS(BlueprintType, Blueprintable)
class DQSUI_API UDQSDefaultQuestLogWidget : public UDQSQuestLogWidgetBase
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System|UI")
	void OpenQuestLog();

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System|UI")
	void CloseQuestLog();

	UFUNCTION(BlueprintCallable, Category = "Dialog Quest System|UI")
	void ToggleQuestLog();

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI")
	bool IsQuestLogOpen() const { return bIsOpen; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|UI")
	bool bStartOpened = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Style")
	FLinearColor FallbackAccentColor = FLinearColor(0.80f, 0.55f, 0.27f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Style")
	FLinearColor FallbackSurfaceColor = FLinearColor(0.05f, 0.08f, 0.10f, 0.95f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Style")
	FLinearColor FallbackCardColor = FLinearColor(0.11f, 0.13f, 0.16f, 0.96f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Style")
	FLinearColor FallbackTextColor = FLinearColor(0.94f, 0.93f, 0.89f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Style")
	FLinearColor FallbackMutedTextColor = FLinearColor(0.69f, 0.71f, 0.68f, 1.0f);

protected:
	UFUNCTION()
	void HandleQuestStateChanged();

	UFUNCTION()
	void HandleCloseClicked();

private:
	void BuildDefaultLayout();
	void RefreshFromState();
	void ApplyTheme();
	void SetQuestLogOpen(bool bInOpen);
	void SelectBestQuest();

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UBorder> QuestLogChrome;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<USizeBox> QuestLogPanelSize;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UTextBlock> HeaderText;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UTextBlock> SubheaderText;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UButton> CloseButton;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UBorder> ListPanel;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UVerticalBox> QuestListBox;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UBorder> DetailPanel;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UTextBlock> SelectedQuestTitleText;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UTextBlock> SelectedQuestStatusText;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UTextBlock> SelectedQuestDescriptionText;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UVerticalBox> SelectedObjectivesBox;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UTextBlock> EmptyStateText;

	UPROPERTY(Transient)
	bool bIsOpen = false;

	UPROPERTY(Transient)
	FName SelectedQuestId = NAME_None;
};
