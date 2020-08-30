#pragma once

#include "CoreMinimal.h"
#include "SGEActorSingletonComponent.h"
#include "SGEMutatorComponent.generated.h"

UCLASS(HideCategories = ("Rendering", "Input", "HLOD"))
class SHOOTERGAME_API USGEMutatorComponent : public USGEActorSingletonComponent
{
	GENERATED_BODY()

public:
	void OnComponentCreated() override;

};