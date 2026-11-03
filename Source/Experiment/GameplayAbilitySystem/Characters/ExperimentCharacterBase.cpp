// Fill out your copyright notice in the Description page of Project Settings.


#include "ExperimentCharacterBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Experiment/GameplayAbilitySystem/ExperimentAbilitySysemComponent.h"
#include "Experiment/GameplayAbilitySystem/Abilities/ExperimentGameplayAbility.h"
#include "Experiment/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"
#include "Experiment/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AExperimentCharacterBase::AExperimentCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//Add the ability system component
	AbilitySystemComponent =  CreateDefaultSubobject<UExperimentAbilitySysemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(AscReplicationMode);
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.0f);
	
	//Don't rotate when the controller rotates. Let that just affect the camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;
	
	//Add the basic attribute sets
	BasicAttributeSet = CreateDefaultSubobject<UBasicAttributeSet>(TEXT("BasicAttributeSet"));
	CombatAttributeSet = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("CombatAttributeSet"));
}

// Called when the game starts or when spawned
void AExperimentCharacterBase::BeginPlay()
{
	AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("State.Dead"))
	.AddUObject(this, &AExperimentCharacterBase::OnDeadTagChanged);
	Super::BeginPlay();
	
}

// Called every frame
void AExperimentCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AExperimentCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AExperimentCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this,this);
		GrantAbilities(StartingAbilities);
	}
}

void AExperimentCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this,this);
	}
}

UAbilitySystemComponent* AExperimentCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

TArray<FGameplayAbilitySpecHandle> AExperimentCharacterBase::GrantAbilities(
	TArray<TSubclassOf<UGameplayAbility>> AbilityToGrant, const int32 Level)
{
	if (!AbilitySystemComponent || !HasAuthority())
	{
		return TArray<FGameplayAbilitySpecHandle>();
	}

	TArray<FGameplayAbilitySpecHandle> AbilityHandles;

	for (TSubclassOf<UGameplayAbility> Ability : AbilityToGrant)
	{
		int32 InputID = -1;
		bool ShouldActivate = false;
		if (const UExperimentGameplayAbility* ExperimentAbilityCDO = GetDefault<UExperimentGameplayAbility>(Ability))
		{
			InputID = static_cast<int32>(ExperimentAbilityCDO->AbilityInputID);
			ShouldActivate = ExperimentAbilityCDO->AutoActivateWhenGranted;
		}
		
		FGameplayAbilitySpecHandle SpecHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(
			Ability, Level, InputID, this
		));
		AbilityHandles.Add(SpecHandle);
		if (ShouldActivate)
		{
			AbilitySystemComponent->TryActivateAbility(SpecHandle);
		}
	}
	SendAbilitiesChangedEvent();
	return AbilityHandles;
}

void AExperimentCharacterBase::RemoveAbilities(TArray<FGameplayAbilitySpecHandle> AbilitiesHandlesToRemove)
{
	if (!AbilitySystemComponent || !HasAuthority())
	{
		return;
	}
	for (FGameplayAbilitySpecHandle AbilityHandle : AbilitiesHandlesToRemove)
	{
		AbilitySystemComponent->ClearAbility(AbilityHandle);
	}
	SendAbilitiesChangedEvent();
}

void AExperimentCharacterBase::SendAbilitiesChangedEvent()
{
	FGameplayEventData EventData;
	EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Abilities.Changed"));
	EventData.Instigator = this;
	EventData.Target = this;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
}

void AExperimentCharacterBase::MultiSendGameplayEventToActor_Implementation(AActor* TargetActor,
	FGameplayEventData EventData)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, EventData.EventTag, EventData);
}

//for multiplay DROP ME 
void AExperimentCharacterBase::ServerSendGameplayEventToSelf_Implementation(FGameplayEventData EventData)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
}

void AExperimentCharacterBase::HandleDeath_Implementation()
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();

	FVector Impulse = GetActorForwardVector() * -20000;
	Impulse.Z = 15000;
	GetMesh()->AddImpulseAtLocation(Impulse, GetActorLocation());
}

void AExperimentCharacterBase::OnDeadTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		HandleDeath();
	}
}
