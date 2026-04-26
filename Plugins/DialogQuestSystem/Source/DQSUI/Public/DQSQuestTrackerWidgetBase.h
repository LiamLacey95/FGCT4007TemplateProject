#pragma once

#include "Blueprint/UserWidget.h"
#include "DQSQuestTrackerWidgetBase.generated.h"

class UDQSQuestViewModel;
class UDQSUIThemeAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDQSQuestWidgetStateChangedSignature);

UCLASS(Abstract, Blueprintable)
class DQSUI_API UDQSQuestTrackerWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog Quest System")
	TObjectPtr<UDQSQuestViewModel> ViewModel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System|UI")
	TSoftObjectPtr<UDQSUIThemeAsset> ThemeOverride;

	UPROPERTY(BlueprintAssignable, Category = "Dialog Quest System|UI")
	FDQSQuestWidgetStateChangedSignature OnWidgetStateChanged;

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialog Quest System|UI")
	void OnQuestStateChanged();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialog Quest System|UI")
	void ApplyResolvedTheme(UDQSUIThemeAsset* Theme);

	UFUNCTION(BlueprintPure, Category = "Dialog Quest System|UI")
	UDQSUIThemeAsset* GetResolvedTheme() const;

protected:
	UFUNCTION()
	void HandleQuestViewModelStateChanged();
};
