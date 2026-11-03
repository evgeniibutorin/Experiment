// Fill out your copyright notice in the Description page of Project Settings.


#include "ExperimentGameplayAbility.h"

UExperimentGameplayAbility::UExperimentGameplayAbility()
{
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("GameplayAbility.Active")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
}

void UExperimentGameplayAbility::SetAbilityLevel(int32 NewLevel)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetCurrentAbilitySpec())
	{
		AbilitySpec->Level = NewLevel;
	}
}

bool UExperimentGameplayAbility::HasPC() const
{
	const APawn* PawnObject = Cast<APawn>(GetAvatarActorFromActorInfo());
	if (!PawnObject)
	{
		return false;
	}
	return PawnObject->GetController()->IsA<APlayerController>();
}
