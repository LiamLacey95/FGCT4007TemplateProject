#include "DQSQuestTrackerWidgetBase.h"

#include "DQSUISettings.h"
#include "DQSUIThemeAsset.h"
#include "DQSQuestViewModel.h"

void UDQSQuestTrackerWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (!ViewModel)
	{
		ViewModel = NewObject<UDQSQuestViewModel>(this);
	}

	ViewModel->InitializeFromWorldContext(this);
	ViewModel->OnStateChanged.RemoveAll(this);
	ViewModel->OnStateChanged.AddDynamic(this, &UDQSQuestTrackerWidgetBase::HandleQuestViewModelStateChanged);
	HandleQuestViewModelStateChanged();
}

UDQSUIThemeAsset* UDQSQuestTrackerWidgetBase::GetResolvedTheme() const
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

void UDQSQuestTrackerWidgetBase::HandleQuestViewModelStateChanged()
{
	ApplyResolvedTheme(GetResolvedTheme());
	OnQuestStateChanged();
	OnWidgetStateChanged.Broadcast();
}
