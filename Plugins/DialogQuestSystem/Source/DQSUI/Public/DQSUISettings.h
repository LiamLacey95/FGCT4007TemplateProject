#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "DQSUISettings.generated.h"

class UDQSDialogueWidgetBase;
class UDQSQuestLogWidgetBase;
class UDQSQuestTrackerWidgetBase;
class UDQSUIThemeAsset;

UCLASS(Config = Game, DefaultConfig, BlueprintType, meta = (DisplayName = "Dialog Quest System UI"))
class DQSUI_API UDQSUISettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UDQSUISettings();

	static const UDQSUISettings* Get();

	virtual FName GetCategoryName() const override;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Dialog Quest System|Defaults")
	TSoftClassPtr<UDQSDialogueWidgetBase> DefaultDialogueWidgetClass;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Dialog Quest System|Defaults")
	TSoftClassPtr<UDQSQuestTrackerWidgetBase> DefaultQuestTrackerWidgetClass;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Dialog Quest System|Defaults")
	TSoftClassPtr<UDQSQuestLogWidgetBase> DefaultQuestLogWidgetClass;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Dialog Quest System|Defaults")
	TSoftObjectPtr<UDQSUIThemeAsset> DefaultTheme;
};
