#include "DQSUISettings.h"

#include "DQSDialogueWidgetBase.h"
#include "DQSQuestLogWidgetBase.h"
#include "DQSQuestTrackerWidgetBase.h"
#include "DQSUIThemeAsset.h"

UDQSUISettings::UDQSUISettings()
{
	DefaultDialogueWidgetClass = TSoftClassPtr<UDQSDialogueWidgetBase>(FSoftClassPath(TEXT("/DialogQuestSystem/UI/WBP_DQSDialogue_Default.WBP_DQSDialogue_Default_C")));
	DefaultQuestTrackerWidgetClass = TSoftClassPtr<UDQSQuestTrackerWidgetBase>(FSoftClassPath(TEXT("/DialogQuestSystem/UI/WBP_DQSQuestTracker_Default.WBP_DQSQuestTracker_Default_C")));
	DefaultQuestLogWidgetClass = TSoftClassPtr<UDQSQuestLogWidgetBase>(FSoftClassPath(TEXT("/DialogQuestSystem/UI/WBP_DQSQuestLog_Default.WBP_DQSQuestLog_Default_C")));
	DefaultTheme = TSoftObjectPtr<UDQSUIThemeAsset>(FSoftObjectPath(TEXT("/DialogQuestSystem/UI/Themes/DA_DQS_DefaultUITheme.DA_DQS_DefaultUITheme")));
}

const UDQSUISettings* UDQSUISettings::Get()
{
	return GetDefault<UDQSUISettings>();
}

FName UDQSUISettings::GetCategoryName() const
{
	return TEXT("Plugins");
}
