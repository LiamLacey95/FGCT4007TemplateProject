#pragma once

#include "CoreMinimal.h"

class UDialogueGraphAsset;
class UQuestGraphAsset;

class DQSEDITOR_API FDQSGraphCompiler
{
public:
	static void CompileDialogueAsset(UDialogueGraphAsset* Asset);
	static void CompileQuestAsset(UQuestGraphAsset* Asset);
	static void ValidateDialogueAsset(UDialogueGraphAsset* Asset, TArray<FText>& OutMessages);
	static void ValidateQuestAsset(UQuestGraphAsset* Asset, TArray<FText>& OutMessages);
};
