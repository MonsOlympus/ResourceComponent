#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SGEResourcesInterface.generated.h"

class USSGEResourcesComponent;

UINTERFACE(Blueprintable)
class SHOOTERGAME_API USGEResourcesInterface : public UInterface
{
	GENERATED_BODY()
};

class SHOOTERGAME_API ISGEResourcesInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Resources")
	USGEResourcesComponent* GetResource();
	virtual USGEResourcesComponent* GetResource_Implementation();
};