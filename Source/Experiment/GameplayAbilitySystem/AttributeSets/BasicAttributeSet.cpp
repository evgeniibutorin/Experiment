// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UBasicAttributeSet::UBasicAttributeSet()
{
	// Initialize default values for attributes
	Health = 100.f;
	MaxHealth = 100.f;
	Stamina = 100.f;
	MaxStamina = 100.f;
	Damage = 0.f;
	Shield = 0.f;
	MaxShield = 100.f;
}

void UBasicAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
}

void UBasicAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	} else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	} else if (Attribute == GetShieldAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxShield());
	}
}

void UBasicAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		float TotalDamage = GetDamage();
		SetDamage(0.f);

		float CurrentShield = GetShield();
		if (CurrentShield > 0.f)
		{
			SetShield(CurrentShield - TotalDamage);
			float RemainingDamage = TotalDamage - CurrentShield;
			if (RemainingDamage > 0.f)
			{
				SetHealth(GetHealth() - RemainingDamage);
				if (Data.EffectSpec.Def->GetAssetTags().HasTag(FGameplayTag::RequestGameplayTag("Effects.HitReaction"))
					&& Data.EvaluatedData.Magnitude != 0.f)
				{
					FGameplayTagContainer HitReactionTagContainer;
					HitReactionTagContainer.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.HitReaction"));
					GetOwningAbilitySystemComponent()->TryActivateAbilitiesByTag(HitReactionTagContainer);
				}
			}
		}
		else
		{
			SetHealth(GetHealth() - TotalDamage);
			if (Data.EffectSpec.Def->GetAssetTags().HasTag(FGameplayTag::RequestGameplayTag("Effects.HitReaction"))
				&& Data.EvaluatedData.Magnitude != 0.f)
			{
				FGameplayTagContainer HitReactionTagContainer;
				HitReactionTagContainer.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.HitReaction"));
				GetOwningAbilitySystemComponent()->TryActivateAbilitiesByTag(HitReactionTagContainer);
			}
		}
	}

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Ensure Health does not exceed MaxHealth
		SetHealth(GetHealth());
	}

	// Handle Stamina changes
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		// Ensure Stamina does not exceed MaxStamina
		SetStamina(GetStamina());
	}
}

void UBasicAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	if (Attribute == GetHealthAttribute() && NewValue <=0.f)
	{
		FGameplayTagContainer DeathAbilityTagContainer;
		DeathAbilityTagContainer.AddTag(FGameplayTag::RequestGameplayTag("GameplayAbility.Death"));
		GetOwningAbilitySystemComponent()->TryActivateAbilitiesByTag(DeathAbilityTagContainer);
	}

	if (Attribute == GetShieldAttribute())
	{
		if (NewValue > 0.f && OldValue <= 0.f)
		{
			GetOwningAbilitySystemComponent()->AddGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.ShieldUp"));
		}
		else if (NewValue <= 0.f && OldValue > 0.f)
		{
			GetOwningAbilitySystemComponent()->RemoveGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.ShieldUp"));
			GetOwningAbilitySystemComponent()->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.ShieldDown"));
		}
	}
}
