#include "SGEMutatorComponent.h"

void USGEMutatorComponent::OnComponentCreated()
{
	AActor* TempOwner = GetOwner();
	if (TempOwner != NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Owner: %s"), *TempOwner->GetName());
	
		USGEResourcesComponent* tempComponent = Cast<USGEResourcesComponent>(TempOwner->GetComponentByClass(USGEResourcesComponent::StaticClass()));
		if (tempComponent != NULL)
		{
			tempComponent->RegisterResource("Health", FVector(100.0f, 100.0f, 200.0f), true);
			tempComponent->RegisterResource("Armor", FVector(50.0f, 100.0f, 200.0f), true);
		}
		else
		{
			tempComponent = NewObject<USGEResourcesComponent>(TempOwner, USGEResourcesComponent::StaticClass());
			if (tempComponent != NULL)
			{
				//tempComponent->AttachToComponent(tempActor->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				tempComponent->CreationMethod = EComponentCreationMethod::Instance;
				tempComponent->RegisterComponent();

				tempComponent->RegisterResource("Health", FVector(100.0f, 100.0f, 200.0f), true);
				tempComponent->RegisterResource("Armor", FVector(50.0f, 100.0f, 200.0f), true);
			}

		}
	}
}