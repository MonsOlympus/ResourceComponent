#include "SGEResourcesComponent.h"
#include "Runtime/Core/Public/Async/ParallelFor.h"
#include "Components/SSGEActorWidgetComponent.h"
#include "UI/SSGEActorWidget.h"

//class USSGEActorWidgetComponent;

class USSGEActorWidget;
class USSGESubWidget;

USGEResourcesComponent::USGEResourcesComponent()
{
	//UE_LOG(LogTemp, Warning, TEXT("Resource Component Constructed"));
	SetAutoActivate(true);

	bWantsInitializeComponent = true;

	//AutoResources.Add(FLimitedResource("Health", FVector(100.0f, 100.0f, 200.0f)));
	//AutoResources.Add(FLimitedResource("Armor", FVector(50.0f, 100.0f, 200.0f)));
}

bool USGEResourcesComponent::RegisterValueTrigger(FName ResourceName, FValueTriggerDelegate& NewValueTrigger, float value)
{
	int32 index = VerifyResource(ResourceName);
	if (index != -1)
	{
		Resources[index].SetValueTrigger(NewValueTrigger, value);
	}
	return false;
}

void USGEResourcesComponent::InitializeComponent()
{
	//for (auto* AutoResource : AutoResources)
	for (FLimitedResource& AutoResource : AutoResources)
	{
		RegisterResourceAuto(AutoResource);
		//RegisterResource(AutoResource.Name, AutoResource.Value, true);
	}

	AActor* TempOwner = GetOwner();
	if (TempOwner != NULL)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Resources Initialize: %s"), *TempOwner->GetName());
	}

	Super::InitializeComponent();
}

void USGEResourcesComponent::OnComponentCreated()
{
	AActor* TempOwner = GetOwner();
	if (TempOwner != NULL)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Resources Created: %s"), *TempOwner->GetName());
	}

//	for (auto AutoResource : AutoResources)
//	{
//		RegisterResourceAuto(AutoResource);
//	}

	/*AActor* TempOwner = GetOwner();
	if (TempOwner != NULL)
	{
		FVector MinSize;
		FVector MaxSize;
		TempOwner->GetActorBounds(false, MinSize, MaxSize);

		auto *NewWidget = LoadObject<UClass>(NULL, TEXT("/Game/Blueprints/ResourceWidget.ResourceWidget_C"), NULL, LOAD_None, NULL);
		ResourcesWidget = NewObject <USSGEActorWidgetComponent>(TempOwner);
		if (ResourcesWidget != nullptr && NewWidget != nullptr)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Health Widget Created"));
			ResourcesWidget->SetWidgetSpace(EWidgetSpace::World);
			ResourcesWidget->AttachToComponent(TempOwner->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			FVector LocationAdjust = ResourcesWidget->GetComponentLocation();
			LocationAdjust.Z += MinSize.Z + 10;
			ResourcesWidget->SetWorldLocation(LocationAdjust);
			ResourcesWidget->SetWidgetClass(NewWidget);
			ResourcesWidget->SetDrawAtDesiredSize(true);
			ResourcesWidget->CreationMethod = EComponentCreationMethod::Instance;
			ResourcesWidget->SetVisibility(true);
			ResourcesWidget->RegisterComponent();

			UE_LOG(LogTemp, Warning, TEXT("Resources Widget Component Created: %s"), *GetNameSafe(ResourcesWidget));
		}

		if (ResourcesWidget != NULL)
		{
			USSGEActorWidget* TempWidget = Cast<USSGEActorWidget>(ResourcesWidget->GetUserWidgetObject());
			if (ResourcesWidget->GetUserWidgetObject() != NULL)
			{
				UE_LOG(LogTemp, Warning, TEXT("Resources Widget: %s"), *ResourcesWidget->GetUserWidgetObject()->GetName());
			}

			if (TempWidget != NULL)
			{
				TempWidget->SetOwningActor(TempOwner);
				//UE_LOG(LogTemp, Warning, TEXT("Owner Set"));
			}
		}
	}*/

	Super::OnComponentCreated();
}

bool USGEResourcesComponent::SetResourceModifierTimer(FName ResourceName, int32 ModifierIndex, EModifierTimer NewTimer, float Time)
{
	if (GetOwner() != NULL)
	{
		FTimerManager* TimerManager = &GetOwner()->GetWorldTimerManager();

		return GetResource(ResourceName)->SetTimer(ModifierIndex, TimerManager, NewTimer, Time);
	}
	return false;
}

void USGEResourcesComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* TempOwner = GetOwner();
	if (TempOwner != NULL)
	{
		FVector MinSize;
		FVector MaxSize;
		TempOwner->GetActorBounds(false, MinSize, MaxSize);

		auto *NewWidget = LoadObject<UClass>(NULL, TEXT("/Game/Blueprints/ResourceWidget.ResourceWidget_C"), NULL, LOAD_None, NULL);
		ResourcesWidget = NewObject <USSGEActorWidgetComponent>(TempOwner);
		if (ResourcesWidget != nullptr && NewWidget != nullptr)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Health Widget Created"));
			ResourcesWidget->SetWidgetSpace(EWidgetSpace::World);
			ResourcesWidget->AttachToComponent(TempOwner->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			FVector LocationAdjust = ResourcesWidget->GetComponentLocation();
			LocationAdjust.Z += MinSize.Z + 10;
			ResourcesWidget->SetWorldLocation(LocationAdjust);
			ResourcesWidget->SetWidgetClass(NewWidget);
			ResourcesWidget->SetDrawAtDesiredSize(true);
			ResourcesWidget->CreationMethod = EComponentCreationMethod::Instance;
			ResourcesWidget->SetVisibility(true);
			ResourcesWidget->RegisterComponent();

			UE_LOG(LogTemp, Warning, TEXT("Resources Widget Component Created: %s"), *GetNameSafe(ResourcesWidget));
		}

		if (ResourcesWidget != NULL)
		{
			USSGEActorWidget* TempWidget = Cast<USSGEActorWidget>(ResourcesWidget->GetUserWidgetObject());
			if (ResourcesWidget->GetUserWidgetObject() != NULL)
			{
				UE_LOG(LogTemp, Warning, TEXT("Resources Widget: %s"), *ResourcesWidget->GetUserWidgetObject()->GetName());
			}

			if (TempWidget != NULL)
			{
				TempWidget->SetOwningActor(TempOwner);
				//UE_LOG(LogTemp, Warning, TEXT("Owner Set"));

				for (FResource Resource : Resources)
				{
					auto *NewWidgetClass = LoadObject<UClass>(NULL, TEXT("/Game/Blueprints/ResourceBarWidget.ResourceBarWidget_C"), NULL, LOAD_None, NULL);
					TempWidget->AddSubWidget(NewWidgetClass, TempWidget->Panel);
					UE_LOG(LogTemp, Warning, TEXT("New Resource Bar Added"));
				}
			}
		}
	}
}

int32 USGEResourcesComponent::VerifyResource(FName ResourceName)
{
	if (CachedResource.Name == ResourceName)
	{
		return CachedResource.Index;
	}
	else
	{
		/*bool bFound;
		FCriticalSection Mutex;
		ParallelFor(Resources.Num(), [&](int32 index) {
			if (Resources[index].Name == ResourceName)
			{
				Mutex.Lock();
				CachedResource.Name = ResourceName;
				CachedResource.Index = index;
				bFound = true;
				Mutex.Unlock();
			}
		});
		if(bFound)
			return true;*/

		if (Resources.Num() > ResourcesAllowedMaximum)
		{
			UE_LOG(LogTemp, Warning, TEXT("Maximum Number of Resources Exceeded!"));
		}

		for (int32 index = 0; index < Resources.Num(); index++)
		{
			if(Resources[index].Name == ResourceName)
			{
				CachedResource.Name = ResourceName;
				CachedResource.Index = index;
				return index;
			}
		}
	}
	return -1;
}

void USGEResourcesComponent::RegisterResourceAuto(const FLimitedResource& LimitedResource)
{
	RegisterResource(LimitedResource.Name, LimitedResource.Value, true);
}

int32 USGEResourcesComponent::RegisterResource(FName ResourceName, FVector Value, bool bActive)
{
	int32 index = VerifyResource(ResourceName);
	if (index == -1)
	{
		FResource NewResource = FResource(ResourceName, Value);
		UE_LOG(LogTemp, Warning, TEXT("New Resource Added: %s"), *ResourceName.ToString());
		return Resources.Add(NewResource);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Resource Found: %s"), *ResourceName.ToString());
	}

/*	if (ResourcesWidget != NULL)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Found Resources Widget"));
		USSGEActorWidget* TempWidget = Cast<USSGEActorWidget>(ResourcesWidget->GetUserWidgetObject());
		if (ResourcesWidget->GetUserWidgetObject() != NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("Resources Widget: %s"), *ResourcesWidget->GetUserWidgetObject()->GetName());
		}

		if (TempWidget != NULL)
		{
			auto *NewWidget = LoadObject<UClass>(NULL, TEXT("/Game/Blueprints/ResourceBarWidget.ResourceBarWidget_C"), NULL, LOAD_None, NULL);
			TempWidget->AddSubWidget(NewWidget->GetClass(), TempWidget->Panel);
			UE_LOG(LogTemp, Warning, TEXT("New Resource Bar Added"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Found Resources Widget"));
	}*/
	return -1;
}

bool USGEResourcesComponent::GetResourceByName(FName ResourceName, FVector& outValue)
{
	if (CachedResource.Name == ResourceName)
	{
		outValue = Resources[CachedResource.Index].GetValue();
		return true;
	}
	else
	{
		for (auto Resource : Resources)
		{
			if (Resource.Name == ResourceName)
			{
				outValue = Resource.GetValue();
				return true;
			}
		}
	}
	return false;
}

bool USGEResourcesComponent::GetResourceByIndex(int32 ResourceIndex, FVector& outValue)
{
	if (ResourceIndex <= Resources.Num())
	{
		outValue = Resources[ResourceIndex].GetValue();
		return true;
	}

	return false;
}

int32 USGEResourcesComponent::AddModifierToResource(FName ResourceName, FResourceModifier Modifier)
{
	if (CachedResource.Name == ResourceName)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Add Modifier to Cached"));
		return Resources[CachedResource.Index].AddModifer(Modifier);
	}
	else
	{
		for (int32 index = 0; index < Resources.Num(); index++)
		{
			if (Resources[index].Name == ResourceName)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Add Modifier to Resource"));
				int32 modIndex = Resources[index].AddModifer(Modifier);
				Resources[index].SetModifierIndex(modIndex);

				CachedResource.Name = ResourceName;
				CachedResource.Index = index;

				return modIndex;
			}
		}
	}
	return - 1.0f;
}

bool USGEResourcesComponent::RemoveModifierFromResource(FName ResourceName)
{
	return false;
}

USGEResourcesComponent* USGEResourcesComponent::GetResource_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *this->GetName());
	return this;
}

void USGEResourcesComponent::DestroyComponent(bool bPromoteChildren)
{
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *this->GetName());
	Super::DestroyComponent(true);
}

FResource* USGEResourcesComponent::GetResource(FName ResourceName)
{
	return &this->Resources[VerifyResource(ResourceName)];
}