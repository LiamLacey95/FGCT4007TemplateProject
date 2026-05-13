#pragma once

#include "Modules/ModuleManager.h"
#include "UObject/UObjectGlobals.h"

class FAssetTypeActions_Base;
class FObjectPreSaveContext;
class UObject;

class FDQSEditorModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterAssetTools();
	void UnregisterAssetTools();
	void RegisterCustomizations();
	void UnregisterCustomizations();
	void RegisterGraphCompileHooks();
	void UnregisterGraphCompileHooks();
	void HandleObjectPreSave(UObject* Object, FObjectPreSaveContext SaveContext);

	TArray<TSharedPtr<FAssetTypeActions_Base>> RegisteredAssetActions;
	FDelegateHandle ObjectPreSaveHandle;
};
