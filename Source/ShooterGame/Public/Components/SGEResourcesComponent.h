#pragma once

#include "CoreMinimal.h"
#include "SGEActorSingletonComponent.h"
#include "SGEResourcesInterface.h"
#include "SGEResourcesComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FValueTriggerDelegate);

UENUM(BlueprintType)
enum EModifierTimer
{
	MT_Instant,
	MT_Countdown,		//TOFF Timer / Stopwatch / Eggtimer
	MT_Charge,			//TON Timer
	//MT_Repeater,
	MT_Fade,				//Inverse of Charge?
	MT_Custom
};

UENUM(BlueprintType)
enum EResourceOperation
{
	AO_Additive,
	AO_Multiplicative,
	AO_Power,
	AO_Override,	//replace current value entirely, Invert is Reset
	AO_Clamp			//Upper bound with lower being Invert
//	AO_Reset		//to default value
};

struct FResource;

USTRUCT(BlueprintType)
struct SHOOTERGAME_API FResourceModifier
{
	GENERATED_BODY()

protected:
	uint8 StackHeight;

	UPROPERTY()
	TEnumAsByte<EResourceOperation> Modifier;

	TEnumAsByte <EModifierTimer> TimerType = EModifierTimer::MT_Instant;

	float Value;

public:
	FName ModifierName;

	bool bInvert;
	bool bIsStackable;

	int32 index;

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;

	FResourceModifier()
		: Value(0.0f), ModifierName("Default")
	{}

	FResourceModifier(const FName NewModifierName, const float NewValue, EResourceOperation NewModifier, bool bNewInvert) 
		: Modifier(NewModifier), Value(NewValue), ModifierName(NewModifierName), bInvert(bNewInvert)
	{}

	EModifierTimer GetTimerType()
	{
		return this->TimerType;
	}

	/*void SetTimer(EModifierTimer NewTimer, FTimerDynamicDelegate NewDelegate)
	{
		switch (TimerType)
		{
		case EModifierTimer::MT_Custom:
			//NewDelegate.BindDynamic(this, &USGEResourcesComponent::Repeater);
			break;
		}
	}*/

	EResourceOperation Get()
	{
		return Modifier;
	}

	const float GetValue()
	{
		return this->Value;
	}
};

USTRUCT()
struct FResource
{
	GENERATED_BODY()

protected:
	uint32 bActive : 1;
	uint32 bForced : 1;
	uint32 bInfinite : 1;

	/* Change to uint8 packed to save bandwith? */
	FVector Value = FVector::ZeroVector;

	float DefaultValue;

	TSparseArray<FResourceModifier> Modifiers;
	//TArray<FResourceModifier> Modifiers;

	TSparseArray<FTimerHandle> ModifierHandles;
	TSparseArray<FTimerDelegate> ModifierDelegates;

	TArray<uint32> ModifiersToRemove;

	FTimerHandle RemovalHandle;

	//TArray<EResourceOperation> Operations;

	FValueTriggerDelegate* ValueTrigger;

	float TriggerPoint;
	bool bTriggerGreaterThan;

public:
	FName Name; // Could also double for Stats Name

	TArray<FName> Events;

	FResource() 
		: Value(0.0f), Name("Default")
	{}

	FResource(const FName NewName, const FVector NewValue)
		: Value(NewValue), DefaultValue(NewValue.X), Name(NewName)
	{}

	const FVector GetValue()
	{
		return this->Value;
	}

	void UpdateValue(float NewValue, FName Coord = "X")
	{
		if (ValueTrigger != NULL)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Has Value Trigger"));
			if (Coord == "X" && ValueTrigger->IsBound())
			{
				this->Value.X = NewValue;
				if (bTriggerGreaterThan && this->Value.X > TriggerPoint)
				{
					UE_LOG(LogTemp, Warning, TEXT("Update Value Greater"));
					ValueTrigger->Broadcast();
				}
				else if (!bTriggerGreaterThan && this->Value.X <= TriggerPoint)
				{
					UE_LOG(LogTemp, Warning, TEXT("Update Value Less"));
					ValueTrigger->Broadcast();
				}
			}
		}
	}

	/*FTimerManager* GetTimerManager()
	{
		if (GetOwner() != NULL)
		{
			return GetOwner()->GetWorldTimerManager();
		}
	}*/

	void RecalculateModifiers()
	{
		float tempValue = 0.0f;
		FName tempCoord = "X";

		//if (this->Modifiers.GetMaxIndex() == 0)
		//{
			tempValue = this->DefaultValue;
			//UpdateValue(tempValue, tempCoord);
			UE_LOG(LogTemp, Warning, TEXT("Update Value: %f"), this->Value.X);
		//	return;
		//}

		//TODO: Make into normal for loop to ensure correct order, will we need to check for null entries?
		for (FResourceModifier Modifier : Modifiers)
		{
			//if (Modifier.ModifierName != "Null")
			//{
				switch (Modifier.Get())
				{
				case EResourceOperation::AO_Additive:
					tempValue = (Modifier.bInvert) ? this->Value.X - Modifier.GetValue() : this->Value.X + Modifier.GetValue();
					break;
				case EResourceOperation::AO_Multiplicative:
					tempValue = (Modifier.bInvert) ? this->Value.X / Modifier.GetValue() : this->Value.X * Modifier.GetValue();
					break;
				case EResourceOperation::AO_Override:
					tempValue = (Modifier.bInvert) ? this->DefaultValue : Modifier.GetValue();
					break;
				case EResourceOperation::AO_Clamp:
					tempValue = Modifier.GetValue();
					tempCoord = (Modifier.bInvert) ? "Y" : "Z";
					break;
				}
				FMath::Clamp(this->Value.X, this->Value.Y, this->Value.Z);
				UpdateValue(tempValue, tempCoord);
				//UE_LOG(LogTemp, Warning, TEXT("Update Value: %f"), this->Value.X);
			//}
		}
	}

	uint32 AddModifer(FResourceModifier NewModifier)
	{
		if (bInfinite)
			return -1.0f;

/*		if (!Operations.Contains(NewModifier.Get())){Operations.AddUnique(NewModifier.Get());}*/

		int32 modIndex = this->Modifiers.Add(NewModifier);
		RecalculateModifiers();
		return modIndex;
	}

	void SetModifierIndex(uint32 NewIndex)
	{
		Modifiers[NewIndex].index = NewIndex;
	}

	void SetValueTrigger(FValueTriggerDelegate& NewValueTrigger, float value, bool bGreaterThan = false)
	{
		this->TriggerPoint = value;
		this->bTriggerGreaterThan = bGreaterThan;
		
		this->ValueTrigger = &NewValueTrigger;
		UE_LOG(LogTemp, Warning, TEXT("SetValueTrigger: %f"), value);
	}

	/*void AssignEvent(FName NewEvent)
	{
		uint32 EventIndex = Events.Find(NewEvent);
		if (EventIndex != INDEX_NONE)
		{
			Events.Add(NewEvent);
			//GetOwner()->OnTakeAnyDamage.AddDynamic(this, &USGEHealthComponent::TakeDamage);
		}
	}*/

	void RemoveModifiers()
	{
		/*for (uint32 index : ModifiersToRemove)
		{
			if (ModifierHandles[index].IsValid() && FTimerManager* TimerManager = GetTimerManager())
				TimerManager->ClearTimer(ModifierHandles[index]);

			if (ModifierDelegates[index]->IsBound())
				ModifierDelegates[index]->UnBind();
		}
		ModifiersToRemove.Empty();*/
	}

	void Activate(bool bNewActivate)
	{
		this->bActive = bNewActivate;
	}

	bool SetTimer(int32 ModifierIndex, FTimerManager* TimerManager, EModifierTimer NewTimer, float Time, float Rate = 1.0f)
	{
		if (TimerManager == NULL || ModifierIndex == -1)
			return false;

		//UE_LOG(LogTemp, Warning, TEXT("TimerManager Found!"));

		float CurrentTime = Time;

		switch (NewTimer)
		{
		case EModifierTimer::MT_Instant:
			break;
		case EModifierTimer::MT_Countdown:
			this->Modifiers[ModifierIndex].TimerDelegate.BindLambda([this] {
				//Remove Modifier
			});
			TimerManager->SetTimer(this->Modifiers[ModifierIndex].TimerHandle, this->Modifiers[ModifierIndex].TimerDelegate, Time, false);
			break;
		case EModifierTimer::MT_Charge:
			//UE_LOG(LogTemp, Warning, TEXT("Charge Timer"));
			this->Modifiers[ModifierIndex].TimerDelegate.BindLambda([=, &CurrentTime]/*[this]*/ {
				//UE_LOG(LogTemp, Warning, TEXT("Timer"));
				if (CurrentTime - Rate >= 0)
					CurrentTime -= Rate;
				else
				{
					//Modifiers[ModifierIndex].ModifierName = "Null";
					//this->Modifiers.RemoveAtUninitialized(ModifierIndex);
					this->Modifiers.RemoveAt(ModifierIndex);
					UE_LOG(LogTemp, Warning, TEXT("Timer Cleared, Numer of Modifiers Remain: %i"), Modifiers.GetMaxIndex());
					RecalculateModifiers();
					TimerManager->PauseTimer(ModifierHandles[ModifierIndex]);
					//FIXME: SUCKER
					//TimerManager->SetTimer(RemovalHandle, FResource::RemoveModifiers(), 5.0f, false);
				}
			});
			TimerManager->SetTimer(this->Modifiers[ModifierIndex].TimerHandle, this->Modifiers[ModifierIndex].TimerDelegate, Rate, true);
			break;
		case EModifierTimer::MT_Fade:
			this->Modifiers[ModifierIndex].TimerDelegate.BindLambda([this] {
			});
			TimerManager->SetTimer(this->Modifiers[ModifierIndex].TimerHandle, this->Modifiers[ModifierIndex].TimerDelegate, Rate, true, Time);
			break;
		}

		return false;
	}
};

struct FCachedResource
{
	FName Name;
	uint8 Index = -1;
};

USTRUCT(BlueprintType)
struct FLimitedResource
{
	GENERATED_BODY()

	UPROPERTY()
	FName Name;

	UPROPERTY()
	FVector Value;

	FLimitedResource()
		: Name("Default"), Value(FVector(0.0f, 0.0f, 0.0f))
	{}

	FLimitedResource(const FName NewName, const FVector NewValue)
		: Name(NewName), Value(NewValue)
	{}
};


class USSGEActorWidget;
class USSGEActorWidgetComponent;

UCLASS(HideCategories = ("Rendering", "Input", "HLOD"), ClassGroup = (SGE), meta = (BlueprintSpawnableComponent))
class SHOOTERGAME_API USGEResourcesComponent : public USGEActorSingletonComponent, public ISGEResourcesInterface
{
	GENERATED_BODY()

protected:
	//NewSerializeArray??
	TArray<FResource> Resources;

	//TMap<FAttribute, FAttributeNode> Modifiers;
	
	USSGEActorWidgetComponent* ResourcesWidget;

	uint8 ResourcesAllowedMaximum = 32;
	uint8 ModifiersAllowedMaximum = 16;

	FCachedResource CachedResource;

	FName GetResourceName(int32 ResourceIndex);

	int32 VerifyResource(FName ResourceName);

	void RegisterResourceAuto(const FLimitedResource& LimitedResource);

public:

	TArray<FLimitedResource> AutoResources;
	
	USGEResourcesComponent();
	void OnComponentCreated() override;
	virtual void InitializeComponent() override;

	virtual void DestroyComponent(bool bPromoteChildren = false) override;

	FResource* GetResource(FName ResourceName);

	UFUNCTION(BlueprintCallable, Category = "Resource")
	int32 RegisterResource(FName ResourceName, FVector Value, bool bActive = true);

	bool UnRegisterResourceByName(FName ResourceName);
	bool UnRegisterResourceByIndex(int32 ResourceIndex);

	void ResourceActivateByName(FName ResourceName, bool bActivate);
	void ResourceActivateByIndex(int32 ResourceIndex, bool bActivate);

	//UFUNCTION(BlueprintCallable, Category = "Resource")
	bool RegisterValueTrigger(FName ResourceName, FValueTriggerDelegate& NewValueTrigger, float value);

	UFUNCTION(BlueprintCallable, Category = "Resource")
	bool GetResourceByName(FName ResourceName, FVector& outValue);

	UFUNCTION(BlueprintCallable, Category = "Resource")
	bool GetResourceByIndex(int32 ResourceIndex, FVector& outValue);

	UFUNCTION(BlueprintCallable, Category = "Resource")
	int32 AddModifierToResource(FName ResourceName, FResourceModifier Modifier);

	UFUNCTION(BlueprintCallable, Category = "Resource")
	bool RemoveModifierFromResource(FName ResourceName);

	virtual USGEResourcesComponent* GetResource_Implementation() override;

	bool SetResourceModifierTimer(FName ResourceName, int32 ModifierIndex, EModifierTimer NewTimer, float Time);

protected:
	virtual void BeginPlay() override;
};