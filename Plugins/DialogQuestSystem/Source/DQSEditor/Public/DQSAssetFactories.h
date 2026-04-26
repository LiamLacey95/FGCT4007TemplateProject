#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "DQSAssetFactories.generated.h"

UCLASS()
class DQSEDITOR_API UDialogueGraphAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	UDialogueGraphAssetFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

UCLASS()
class DQSEDITOR_API UQuestGraphAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	UQuestGraphAssetFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
