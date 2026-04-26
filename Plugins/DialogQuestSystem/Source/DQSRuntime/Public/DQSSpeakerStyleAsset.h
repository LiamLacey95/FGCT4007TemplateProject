#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Fonts/SlateFontInfo.h"
#include "Styling/SlateBrush.h"
#include "DQSSpeakerStyleAsset.generated.h"

class UTexture2D;

UCLASS(BlueprintType)
class DQSRUNTIME_API UDQSSpeakerStyleAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Speaker")
	TSoftObjectPtr<UTexture2D> PortraitTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Speaker")
	FSlateBrush PortraitBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Speaker")
	FVector2D PortraitSize = FVector2D(220.0f, 260.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Speaker")
	bool bOverrideAccentColor = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Speaker", meta = (EditCondition = "bOverrideAccentColor"))
	FLinearColor AccentColor = FLinearColor(0.80f, 0.55f, 0.27f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Speaker")
	FSlateFontInfo SpeakerFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|Speaker")
	FSlateFontInfo BodyFont;
};
