#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Styling/SlateBrush.h"
#include "Fonts/SlateFontInfo.h"
#include "DQSUIThemeAsset.generated.h"

UCLASS(BlueprintType)
class DQSRUNTIME_API UDQSUIThemeAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Color")
	FLinearColor BackdropColor = FLinearColor(0.02f, 0.02f, 0.04f, 0.54f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Color")
	FLinearColor SurfaceColor = FLinearColor(0.07f, 0.08f, 0.11f, 0.94f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Color")
	FLinearColor PanelColor = FLinearColor(0.10f, 0.11f, 0.14f, 0.96f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Color")
	FLinearColor FrameColor = FLinearColor(0.47f, 0.34f, 0.19f, 0.95f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Color")
	FLinearColor AccentColor = FLinearColor(0.80f, 0.55f, 0.27f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Color")
	FLinearColor TextColor = FLinearColor(0.93f, 0.91f, 0.86f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Color")
	FLinearColor MutedTextColor = FLinearColor(0.67f, 0.66f, 0.62f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Color")
	FLinearColor ChoiceColor = FLinearColor(0.17f, 0.16f, 0.14f, 0.96f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Color")
	FLinearColor DisabledChoiceColor = FLinearColor(0.24f, 0.18f, 0.17f, 0.94f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Brush")
	FSlateBrush PanelBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Brush")
	FSlateBrush FrameBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Brush")
	FSlateBrush AccentBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Brush")
	FSlateBrush PortraitFrameBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Font")
	FSlateFontInfo CaptionFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Font")
	FSlateFontInfo SpeakerFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Font")
	FSlateFontInfo BodyFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Font")
	FSlateFontInfo ChoiceFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Font")
	FSlateFontInfo ButtonFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Layout")
	FMargin PanelPadding = FMargin(22.0f, 18.0f, 22.0f, 18.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Layout")
	FMargin BodyPadding = FMargin(18.0f, 16.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Layout")
	FMargin ChoicePadding = FMargin(14.0f, 12.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Layout")
	float DialogueWidth = 1120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Layout")
	float DialogueMaxHeight = 420.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Layout")
	FVector2D PortraitSize = FVector2D(220.0f, 260.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Layout")
	float BodyMaxHeight = 220.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Dialogue|Layout")
	float ChoiceListMaxHeight = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Quest|Color")
	FLinearColor QuestSurfaceColor = FLinearColor(0.05f, 0.08f, 0.10f, 0.94f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Quest|Color")
	FLinearColor QuestCardColor = FLinearColor(0.11f, 0.13f, 0.16f, 0.96f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Quest|Color")
	FLinearColor QuestAccentColor = FLinearColor(0.80f, 0.55f, 0.27f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Quest|Color")
	FLinearColor QuestTextColor = FLinearColor(0.94f, 0.93f, 0.89f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Quest|Color")
	FLinearColor QuestMutedTextColor = FLinearColor(0.69f, 0.71f, 0.68f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Quest|Layout")
	float TrackerWidth = 320.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Quest|Layout")
	float LogWidth = 880.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Quest|Layout")
	float LogHeight = 580.0f;
};
