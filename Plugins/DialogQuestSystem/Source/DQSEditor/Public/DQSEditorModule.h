#pragma once

#include "Modules/ModuleManager.h"

class FAssetTypeActions_Base;

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

	TArray<TSharedPtr<FAssetTypeActions_Base>> RegisteredAssetActions;
};
