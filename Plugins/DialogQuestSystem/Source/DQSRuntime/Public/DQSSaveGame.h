#pragma once

#include "GameFramework/SaveGame.h"
#include "DQSTypes.h"
#include "DQSSaveGame.generated.h"

UCLASS()
class DQSRUNTIME_API UDQSSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog Quest System")
	FDQSSaveSnapshot Snapshot;
};
