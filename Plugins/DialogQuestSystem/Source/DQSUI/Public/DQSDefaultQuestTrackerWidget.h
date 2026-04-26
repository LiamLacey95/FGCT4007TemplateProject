#pragma once

#include "CoreMinimal.h"
#include "DQSQuestTrackerWidgetBase.h"
#include "DQSDefaultQuestTrackerWidget.generated.h"

class UBorder;
class UTextBlock;
class UVerticalBox;

UCLASS(BlueprintType, Blueprintable)
class DQSUI_API UDQSDefaultQuestTrackerWidget : public UDQSQuestTrackerWidgetBase
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Style")
	FLinearColor FallbackAccentColor = FLinearColor(0.80f, 0.55f, 0.27f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Style")
	FLinearColor FallbackSurfaceColor = FLinearColor(0.05f, 0.08f, 0.10f, 0.94f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Style")
	FLinearColor FallbackTextColor = FLinearColor(0.94f, 0.93f, 0.89f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Style")
	FLinearColor FallbackMutedTextColor = FLinearColor(0.69f, 0.71f, 0.68f, 1.0f);

protected:
	UFUNCTION()
	void HandleQuestStateChanged();

private:
	void BuildDefaultLayout();
	void RefreshFromState();
	void ApplyTheme();

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UBorder> TrackerPanel;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UTextBlock> HeaderText;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UBorder> LatestPanel;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UTextBlock> LatestTitleText;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UTextBlock> LatestMessageText;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UTextBlock> TrackedLabelText;

	UPROPERTY(meta = (BindWidgetOptional), Transient)
	TObjectPtr<UVerticalBox> ActiveQuestList;
};
