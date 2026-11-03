// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ExperimentGameplayAbility.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	None UMETA(DisplayName = "None"), // 0
	PrimaryAbility UMETA(DisplayName = "Primary Ability"), // 1
	SecondaryAbility UMETA(DisplayName = "Secondary Ability"), // 2
	DefensiveAbility UMETA(DisplayName = "Defensive Ability"), // 3
	MovementAbility UMETA(DisplayName = "Movement Ability"), // 4
};

UCLASS()
class EXPERIMENT_API UExperimentGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UExperimentGameplayAbility();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	bool ShouldShowInAbilitiesBar = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Activation")
	bool AutoActivateWhenGranted = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	EAbilityInputID AbilityInputID = EAbilityInputID::None;
	
	UFUNCTION(BlueprintCallable, Category="Ability")
	void SetAbilityLevel(int32 NewLevel);

private:
	UFUNCTION(BlueprintCallable, Category="Helpers")
	bool HasPC() const;
};
