#pragma once

#include "Modules/ModuleManager.h"

class FDQSRuntimeModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
