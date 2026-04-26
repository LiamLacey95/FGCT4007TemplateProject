#include "DQSEditorModule.h"

#include "AssetToolsModule.h"
#include "AssetTypeActions_Base.h"
#include "DQSAssetEditorToolkit.h"
#include "DQSAssetFactories.h"
#include "DQSEdGraphNodes.h"
#include "DQSEdGraphNodeDetails.h"
#include "DQSNodeStructDetails.h"
#include "DQSDialogueGraphAsset.h"
#include "DQSQuestGraphAsset.h"
#include "IAssetTools.h"
#include "PropertyEditorModule.h"

namespace
{
	class FDialogueGraphAssetActions final : public FAssetTypeActions_Base
	{
	public:
		virtual FText GetName() const override { return FText::FromString(TEXT("Dialogue Graph")); }
		virtual FColor GetTypeColor() const override { return FColor(230, 170, 60); }
		virtual UClass* GetSupportedClass() const override { return UDialogueGraphAsset::StaticClass(); }
		virtual uint32 GetCategories() override { return EAssetTypeCategories::Gameplay; }

		virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor) override
		{
			for (UObject* Object : InObjects)
			{
				if (UDialogueGraphAsset* Asset = Cast<UDialogueGraphAsset>(Object))
				{
					TSharedRef<FDQSAssetEditorToolkit> Editor = MakeShared<FDQSAssetEditorToolkit>();
					Editor->InitDialogueEditor(EToolkitMode::Standalone, EditWithinLevelEditor, Asset);
				}
			}
		}
	};

	class FQuestGraphAssetActions final : public FAssetTypeActions_Base
	{
	public:
		virtual FText GetName() const override { return FText::FromString(TEXT("Quest Graph")); }
		virtual FColor GetTypeColor() const override { return FColor(80, 180, 120); }
		virtual UClass* GetSupportedClass() const override { return UQuestGraphAsset::StaticClass(); }
		virtual uint32 GetCategories() override { return EAssetTypeCategories::Gameplay; }

		virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor) override
		{
			for (UObject* Object : InObjects)
			{
				if (UQuestGraphAsset* Asset = Cast<UQuestGraphAsset>(Object))
				{
					TSharedRef<FDQSAssetEditorToolkit> Editor = MakeShared<FDQSAssetEditorToolkit>();
					Editor->InitQuestEditor(EToolkitMode::Standalone, EditWithinLevelEditor, Asset);
				}
			}
		}
	};
}

void FDQSEditorModule::StartupModule()
{
	RegisterAssetTools();
	RegisterCustomizations();
}

void FDQSEditorModule::ShutdownModule()
{
	UnregisterCustomizations();
	UnregisterAssetTools();
}

void FDQSEditorModule::RegisterAssetTools()
{
	IAssetTools& AssetTools = FAssetToolsModule::GetModule().Get();

	RegisteredAssetActions.Add(MakeShared<FDialogueGraphAssetActions>());
	RegisteredAssetActions.Add(MakeShared<FQuestGraphAssetActions>());

	for (const TSharedPtr<FAssetTypeActions_Base>& Action : RegisteredAssetActions)
	{
		AssetTools.RegisterAssetTypeActions(Action.ToSharedRef());
	}
}

void FDQSEditorModule::UnregisterAssetTools()
{
	if (!FModuleManager::Get().IsModuleLoaded(TEXT("AssetTools")))
	{
		return;
	}

	IAssetTools& AssetTools = FAssetToolsModule::GetModule().Get();
	for (const TSharedPtr<FAssetTypeActions_Base>& Action : RegisteredAssetActions)
	{
		if (Action.IsValid())
		{
			AssetTools.UnregisterAssetTypeActions(Action.ToSharedRef());
		}
	}

	RegisteredAssetActions.Reset();
}

void FDQSEditorModule::RegisterCustomizations()
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	PropertyEditorModule.RegisterCustomClassLayout(UDQSDialogueEdGraphNode::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FDQSDialogueEdGraphNodeDetails::MakeInstance));
	PropertyEditorModule.RegisterCustomClassLayout(UDQSQuestEdGraphNode::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FDQSQuestEdGraphNodeDetails::MakeInstance));
	PropertyEditorModule.RegisterCustomPropertyTypeLayout(FDQSDialogueNode::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDQSDialogueNodeDetails::MakeInstance));
	PropertyEditorModule.RegisterCustomPropertyTypeLayout(FDQSQuestNode::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDQSQuestNodeDetails::MakeInstance));
	PropertyEditorModule.RegisterCustomPropertyTypeLayout(FDQSDialogueChoice::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FDQSDialogueChoiceDetails::MakeInstance));
	PropertyEditorModule.NotifyCustomizationModuleChanged();
}

void FDQSEditorModule::UnregisterCustomizations()
{
	if (!FModuleManager::Get().IsModuleLoaded(TEXT("PropertyEditor")))
	{
		return;
	}

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	PropertyEditorModule.UnregisterCustomClassLayout(UDQSDialogueEdGraphNode::StaticClass()->GetFName());
	PropertyEditorModule.UnregisterCustomClassLayout(UDQSQuestEdGraphNode::StaticClass()->GetFName());
	PropertyEditorModule.UnregisterCustomPropertyTypeLayout(FDQSDialogueNode::StaticStruct()->GetFName());
	PropertyEditorModule.UnregisterCustomPropertyTypeLayout(FDQSQuestNode::StaticStruct()->GetFName());
	PropertyEditorModule.UnregisterCustomPropertyTypeLayout(FDQSDialogueChoice::StaticStruct()->GetFName());
	PropertyEditorModule.NotifyCustomizationModuleChanged();
}

IMPLEMENT_MODULE(FDQSEditorModule, DQSEditor)
