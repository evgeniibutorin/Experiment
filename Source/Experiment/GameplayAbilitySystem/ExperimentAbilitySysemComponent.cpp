// Fill out your copyright notice in the Description page of Project Settings.


#include "ExperimentAbilitySysemComponent.h"

#include "Characters/ExperimentCharacterBase.h"


// Sets default values for this component's properties
UExperimentAbilitySysemComponent::UExperimentAbilitySysemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UExperimentAbilitySysemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UExperimentAbilitySysemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	AExperimentCharacterBase* Character = Cast<AExperimentCharacterBase>(GetOwner());
	if (!Character) return;


	bool bAbilitiesChanged = false;
	if (LastActivatableAbilities.Num() != ActivatableAbilities.Items.Num())
	{
		bAbilitiesChanged = true;
	}
	else
	{
		for (int32 i = 0; i < LastActivatableAbilities.Num(); i++)
		{
			if (LastActivatableAbilities[i].Ability != ActivatableAbilities.Items[i].Ability)
			{
				bAbilitiesChanged = true;
				break;
			}
		}
		if (bAbilitiesChanged)
		{
			Character->SendAbilitiesChangedEvent();
			LastActivatableAbilities = ActivatableAbilities.Items;
		}
	}
}


// Called every frame
void UExperimentAbilitySysemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

