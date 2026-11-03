// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "ExperimentAbilitySysemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class EXPERIMENT_API UExperimentAbilitySysemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
protected:
	TArray<FGameplayAbilitySpec> LastActivatableAbilities;

	
public:
	// Sets default values for this component's properties
	UExperimentAbilitySysemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	void OnRep_ActivateAbilities() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
