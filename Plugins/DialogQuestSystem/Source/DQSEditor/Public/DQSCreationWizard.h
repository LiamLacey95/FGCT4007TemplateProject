#pragma once

#include "CoreMinimal.h"
#include "DQSGraphEditorUtils.h"

class DQSEDITOR_API FDQSCreationWizard
{
public:
	static bool PromptForDialogueOptions(const FString& AssetName, FDQSDialogueCreationOptions& OutOptions);
	static bool PromptForQuestOptions(const FString& AssetName, FDQSQuestCreationOptions& OutOptions);
};
