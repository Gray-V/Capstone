// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponEquipmentComponent.h"
#include "WeaponBase.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values for this component's properties
UWeaponEquipmentComponent::UWeaponEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// Default socket name - customize this to match your skeleton
	WeaponSocketName = FName("hand_r_socket");
	CurrentWeapon = nullptr;
}

// Called when the game starts
void UWeaponEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get reference to the character's mesh
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		CharacterMesh = OwnerCharacter->GetMesh();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponEquipmentComponent: Owner is not a Character!"));
	}
}

bool UWeaponEquipmentComponent::EquipWeapon(AWeaponBase* NewWeapon)
{
	// Validation checks
	if (!NewWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipWeapon: NewWeapon is null!"));
		return false;
	}

	if (!CharacterMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("EquipWeapon: CharacterMesh is null!"));
		return false;
	}

	// Check if socket exists
	if (!CharacterMesh->DoesSocketExist(WeaponSocketName))
	{
		UE_LOG(LogTemp, Error, TEXT("EquipWeapon: Socket '%s' does not exist on mesh!"), *WeaponSocketName.ToString());
		return false;
	}

	// Unequip current weapon if one is equipped
	if (CurrentWeapon)
	{
		UnequipWeapon();
	}

	// Attach the new weapon
	if (AttachWeaponToSocket(NewWeapon))
	{
		CurrentWeapon = NewWeapon;
		
		// Notify the weapon that it has been equipped
		if (CurrentWeapon)
		{
			// Call any weapon-specific equip logic here
			// Example: CurrentWeapon->OnEquipped();
		}

		// Fire Blueprint event
		OnWeaponEquipped(CurrentWeapon);
		
		UE_LOG(LogTemp, Log, TEXT("Successfully equipped weapon: %s"), *NewWeapon->GetName());
		return true;
	}

	return false;
}

bool UWeaponEquipmentComponent::UnequipWeapon()
{
	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnequipWeapon: No weapon currently equipped!"));
		return false;
	}

	AWeaponBase* WeaponToUnequip = CurrentWeapon;
	
	// Detach the weapon
	DetachWeapon(CurrentWeapon);
	
	// Clear the current weapon reference
	CurrentWeapon = nullptr;
	
	// Fire Blueprint event
	OnWeaponUnequipped(WeaponToUnequip);
	
	UE_LOG(LogTemp, Log, TEXT("Successfully unequipped weapon: %s"), *WeaponToUnequip->GetName());
	return true;
}

bool UWeaponEquipmentComponent::AttachWeaponToSocket(AWeaponBase* Weapon)
{
	if (!Weapon || !CharacterMesh)
	{
		return false;
	}

	// Set up attachment rules
	FAttachmentTransformRules AttachmentRules(
		EAttachmentRule::SnapToTarget,  // Location
		EAttachmentRule::SnapToTarget,  // Rotation
		EAttachmentRule::KeepWorld,     // Scale
		true                             // Weld simulated bodies
	);

	// Attach to socket
	bool bAttached = Weapon->AttachToComponent(
		CharacterMesh,
		AttachmentRules,
		WeaponSocketName
	);

	if (bAttached)
	{
		// Disable physics if enabled
		if (UPrimitiveComponent* WeaponRoot = Cast<UPrimitiveComponent>(Weapon->GetRootComponent()))
		{
			WeaponRoot->SetSimulatePhysics(false);
			WeaponRoot->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	return bAttached;
}

void UWeaponEquipmentComponent::DetachWeapon(AWeaponBase* Weapon)
{
	if (!Weapon)
	{
		return;
	}

	// Set up detachment rules
	FDetachmentTransformRules DetachmentRules(
		EDetachmentRule::KeepWorld,
		true
	);

	// Detach from parent
	Weapon->DetachFromActor(DetachmentRules);
	
	// Optionally re-enable physics or destroy the weapon
	// Example: Weapon->Destroy();
	// Or: EnablePhysicsOnWeapon(Weapon);
}