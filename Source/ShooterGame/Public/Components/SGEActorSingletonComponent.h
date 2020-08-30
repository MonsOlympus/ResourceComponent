#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SGEActorSingletonComponent.generated.h"

UCLASS(ClassGroup=SGE, BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class SHOOTERGAME_API USGEActorSingletonComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USGEActorSingletonComponent();

};