#include "DQSAssetFactories.h"

#include "DQSCreationWizard.h"
#include "DQSDialogueGraphAsset.h"
#include "DQSEdGraphNodes.h"
#include "DQSGraphEditorUtils.h"
#include "DQSGraphCompiler.h"
#include "DQSQuestGraphAsset.h"

UDialogueGraphAssetFactory::UDialogueGraphAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UDialogueGraphAsset::StaticClass();
}

UObject* UDialogueGraphAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	FDQSDialogueCreationOptions CreationOptions;
	if (!FDQSCreationWizard::PromptForDialogueOptions(Name.ToString(), CreationOptions))
	{
		return nullptr;
	}

	UDialogueGraphAsset* Asset = NewObject<UDialogueGraphAsset>(InParent, Class, Name, Flags | RF_Transactional);
	Asset->Metadata.DisplayName = CreationOptions.DisplayName.IsEmpty() ? FText::FromName(Name) : CreationOptions.DisplayName;

#if WITH_EDITORONLY_DATA
	Asset->EditorGraph = NewObject<UDQSDialogueEdGraph>(Asset, TEXT("DialogueGraph"), RF_Transactional);
	Asset->EditorGraph->Schema = UDQSDialogueGraphSchema::StaticClass();
	Asset->EditorGraph->SetFlags(RF_Transactional);
	FDQSGraphEditorUtils::ScaffoldDialogueAsset(Asset, CreationOptions);
#endif

	FDQSGraphCompiler::CompileDialogueAsset(Asset);
	return Asset;
}

UQuestGraphAssetFactory::UQuestGraphAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UQuestGraphAsset::StaticClass();
}

UObject* UQuestGraphAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	FDQSQuestCreationOptions CreationOptions;
	if (!FDQSCreationWizard::PromptForQuestOptions(Name.ToString(), CreationOptions))
	{
		return nullptr;
	}

	UQuestGraphAsset* Asset = NewObject<UQuestGraphAsset>(InParent, Class, Name, Flags | RF_Transactional);
	Asset->Metadata.DisplayName = CreationOptions.DisplayName.IsEmpty() ? FText::FromName(Name) : CreationOptions.DisplayName;

#if WITH_EDITORONLY_DATA
	Asset->EditorGraph = NewObject<UDQSQuestEdGraph>(Asset, TEXT("QuestGraph"), RF_Transactional);
	Asset->EditorGraph->Schema = UDQSQuestGraphSchema::StaticClass();
	Asset->EditorGraph->SetFlags(RF_Transactional);
	FDQSGraphEditorUtils::ScaffoldQuestAsset(Asset, CreationOptions);
#endif

	FDQSGraphCompiler::CompileQuestAsset(Asset);
	return Asset;
}
